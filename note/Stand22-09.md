================================================================================
                     NAVGET PROJEKT-SNAPSHOT (Stand: Sept. 2026)
================================================================================

1. HARDWARE & PIN-BELEGUNG (FUNKTIONIEREND)
-------------------------------------------
- Mikrocontroller: ESP32-S3
- Display: 1.28" GC9A01A Runddisplay (240x240 px)
- Pins:
  #define TFT_DC    8
  #define TFT_CS    9
  #define TFT_SCLK  10
  #define TFT_MOSI  11
  #define TFT_RST   12
  #define TFT_MISO  -1
  #define TFT_BL    40

2. WICHTIGSTE ERKENNTNISSE & GELÖSTE PROBLEME
---------------------------------------------
A) Treiber-Problem: Die Bibliothek 'TFT_eSPI' führte auf dem ESP32-S3 zu 
   "Guru Meditation" Abstürzen. 
   -> LÖSUNG: Wir nutzen stattdessen 'Adafruit_GFX' + 'Adafruit_GC9A01A'. 
      Diese laufen stabil und schnell.

B) Speicher-Problem (Sketch zu groß > 1.3MB): EEZ Studio exportierte die 
   17 Manöver-Bilder (200x200px) standardmäßig als 32-Bit 'ARGB8888'. 
   Das verbrauchte ca. 2.7 MB nur für die Bilder!
   -> LÖSUNG: In EEZ Studio das Bildformat auf 'RGB565' (16-Bit) umstellen 
      und neu exportieren. Zusätzlich in der Arduino IDE unter 
      "Werkzeuge" -> "Partition Scheme" auf "Huge App (3MB No OTA)" stellen.

C) LVGL v9 Kompatibilität: EEZ generierte teils LVGL v8 Syntax.
   -> LÖSUNG: In 'images.h' musste 'lv_img_dsc_t' zu 'lv_image_dsc_t' 
      geändert werden. Fehlende 'vars.cpp' wurde umgangen, indem wir die 
      UI-Objekte (objects.distance, objects.battery etc.) direkt im .ino 
      Code ansteuern, statt über EEZ-Variablen.

D) Bildaufbau (Teearing): Das Display baute sich stückweise von oben nach 
   unten auf.
   -> LÖSUNG: Ein größerer Draw-Buffer (240 * 80 Pixel) in Kombination mit 
      LV_DISPLAY_RENDER_MODE_PARTIAL verhindert dies fast vollständig, 
      ohne zu viel RAM zu verbrauchen.

3. ARDUINO IDE EINSTELLUNGEN (WICHTIG!)
---------------------------------------
- Board: "ESP32S3 Dev Module"
- Partition Scheme: "Huge App (3MB No OTA)"  (oder "Default 4MB with spiffs")
- Bibliotheken installiert: 
  1. lvgl (Version 9.x)
  2. Adafruit GFX Library
  3. Adafruit GC9A01A

4. DER AKTUELLE, OPTIMIERTE .INO CODE
-------------------------------------
(Kopiere diesen Code 1:1 in deine v2.ino)

#include <lvgl.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include "ui.h"
#include "images.h"

extern objects_t objects;

#define TFT_DC    8
#define TFT_CS    9
#define TFT_SCLK  10
#define TFT_MOSI  11
#define TFT_RST   12
#define TFT_MISO  -1
#define TFT_BL    40

#define MY_DISP_HOR_RES 240
#define MY_DISP_VER_RES 240

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST, TFT_MISO);

// Optimierter Buffer: 80 Zeilen (verhindert Teearing, spart aber RAM)
static lv_color_t buf[MY_DISP_HOR_RES * 80];

enum UiState { STATE_NAV_TARGET = 0, STATE_NAV_TURN = 1, STATE_NAV_SPEED = 2 };
UiState current_state = STATE_NAV_TURN;

const lv_image_dsc_t* maneuver_images[] = {
    &img_01_straight, &img_02_slight_left, &img_03_left, &img_04_sharp_left,
    &img_05_slight_right, &img_06_right, &img_07_sharp_right, &img_08_u_turn_left,
    &img_09_u_turn_right, &img_10_roundabout_right, &img_11_roundabout_upper_right,
    &img_12_roundabout_straight, &img_13_roundabout_upper_left, &img_14_roundabout_left,
    &img_15_roundabout_lower_left, &img_16_roundabout_lower_right, &img_17_roundabout_u_turn
};
int current_maneuver_index = 0;
bool music_playing = false;

void set_ui_state(UiState state) {
    current_state = state;
    lv_obj_add_flag(objects.nav_target, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.nav_turn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.arc_speed, LV_OBJ_FLAG_HIDDEN);

    switch (state) {
        case STATE_NAV_TARGET: lv_obj_clear_flag(objects.nav_target, LV_OBJ_FLAG_HIDDEN); break;
        case STATE_NAV_TURN: lv_obj_clear_flag(objects.nav_turn, LV_OBJ_FLAG_HIDDEN); break;
        case STATE_NAV_SPEED: lv_obj_clear_flag(objects.arc_speed, LV_OBJ_FLAG_HIDDEN); break;
    }
}

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.writePixels((uint16_t *)px_map, w * h, true);
    tft.endWrite();
    lv_display_flush_ready(disp);
}

void boot_to_main_timer_cb(lv_timer_t * timer) {
    loadScreen(SCREEN_ID_MAIN); 
    set_ui_state(current_state);
    lv_timer_del(timer);
}

void setup() {
    Serial.begin(115200);
    tft.begin();
    tft.setRotation(0);
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    tft.fillScreen(0x0000);

    lv_init();
    lv_display_t *disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    create_screens();
    loadScreen(SCREEN_ID_BOOT);
    lv_timer_create(boot_to_main_timer_cb, 3000, NULL);

    lv_label_set_text(objects.street, "Hauptstrasse");
    lv_label_set_text(objects.distance, "250");
    lv_label_set_text(objects.distance_entity, "m");
    lv_label_set_text(objects.lbl_speed, "0");
    lv_label_set_text(objects.lbl_speed_entity, "km/h");
    lv_bar_set_value(objects.battery, 100, LV_ANIM_OFF);

    lv_obj_add_flag(objects.phone, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.music, LV_OBJ_FLAG_HIDDEN);
}

void loop() {
    lv_tick_inc(5);
    lv_timer_handler();
    ui_tick(); 
    delay(5);
    
    if (Serial.available()) {
        char cmd = Serial.read();
        switch (cmd) {
            case 't': set_ui_state(STATE_NAV_TARGET); break;
            case 'u': set_ui_state(STATE_NAV_TURN); break;
            case 's': set_ui_state(STATE_NAV_SPEED); break;
            case 'n': 
                current_maneuver_index = (current_maneuver_index + 1) % 17;
                lv_image_set_src(objects.nav_turn, maneuver_images[current_maneuver_index]);
                lv_label_set_text(objects.distance, "150");
                break;
            case 'i': 
                lv_image_set_src(objects.phone, &img_incoming); 
                lv_obj_clear_flag(objects.phone, LV_OBJ_FLAG_HIDDEN); 
                break;
            case 'x': 
                lv_obj_add_flag(objects.phone, LV_OBJ_FLAG_HIDDEN); 
                break;
            case 'm': 
                music_playing = !music_playing;
                lv_image_set_src(objects.music, music_playing ? &img_play : &img_pause);
                if(music_playing) lv_obj_clear_flag(objects.music, LV_OBJ_FLAG_HIDDEN);
                break;
            case 'r': {
                static int rotation = 0;
                rotation = (rotation + 450) % 3600; // 45 Grad Schritte
                lv_image_set_rotation(objects.nav_target, rotation);
                break;
            }
            case '+': {
                int32_t batt = lv_bar_get_value(objects.battery);
                if (batt < 100) lv_bar_set_value(objects.battery, batt + 10, LV_ANIM_ON);
                break;
            }
            case '-': {
                int32_t batt = lv_bar_get_value(objects.battery);
                if (batt > 0) lv_bar_set_value(objects.battery, batt - 10, LV_ANIM_ON);
                break;
            }
        }
    }
}

5. CHECKLISTE FÜR DEN WIEDEREINSTIEG
------------------------------------
Wenn du das Projekt später fortsetzt, gehe genau in dieser Reihenfolge vor:

[ ] 1. EEZ Studio öffnen: Gehe zu den Bildeigenschaften (Assets/Images) und 
      stelle das Export-Format von 'ARGB8888' auf 'RGB565' um.
[ ] 2. EEZ Studio: Projekt neu in den Ordner 'v2' exportieren.
[ ] 3. Arduino IDE: Gehe zu "Werkzeuge" -> "Partition Scheme" und wähle 
      "Huge App (3MB No OTA)".
[ ] 4. Arduino IDE: Kopiere den obigen .ino Code in deine 'v2.ino'.
[ ] 5. Kompilieren & Hochladen.
[ ] 6. Seriellen Monitor (115200 Baud) öffnen und Befehle testen:
      't', 'u', 's' (Zustandswechsel)
      'n' (Nächstes Manöver)
      '+' / '-' (Akku)
      'r' (NavTarget drehen)

================================================================================
Viel Erfolg beim Weitermachen! Du hast die härteste Phase (Treiber-Konflikte 
und Linker-Fehler) bereits erfolgreich hinter dir gebracht.
================================================================================