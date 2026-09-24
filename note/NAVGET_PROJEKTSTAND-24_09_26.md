# 📦 NavGet Projekt-Snapshot (Stand: September 2026)

## 1. Hardware & Pin-Belegung
- **Mikrocontroller:** ESP32-S3 (ESP32-S3-DevKitC-1-N8, 8MB Flash, 320KB RAM)
- **Display:** 1.28" GC9A01A Runddisplay (240x240 px, SPI)
- **Pins (funktionierend):**
  ```cpp
  #define TFT_DC    8
  #define TFT_CS    9
  #define TFT_SCLK  10
  #define TFT_MOSI  11
  #define TFT_RST   12
  #define TFT_MISO  -1
  #define TFT_BL    40   // Backlight (PWM)
  ```

## 2. Software-Stack
- **IDE:** PlatformIO in Visual Studio Code (nicht mehr Arduino IDE!)
- **Framework:** Arduino auf ESP32-S3
- **Bibliotheken:**
  - `lvgl@^9.2.2` (LVGL v9 – **nicht** v8!)
  - `adafruit/Adafruit GFX Library@^1.12.6`
  - `adafruit/Adafruit GC9A01A@^1.1.0`
- **UI-Generator:** EEZ Studio (Export für LVGL v9)

## 3. Wichtige Erkenntnisse (Lessons Learned)

### A. Arduino IDE vs. PlatformIO
- **Arduino IDE** zwingt alle `.c`-Dateien in C++ → viele Fehler bei EEZ-Export
- **PlatformIO** behandelt `.c` als C und `.cpp` als C++ → sauberer Build
- **Wechsel zu PlatformIO hat 90% der Fehler eliminiert**

### B. EEZ Studio Export
- Immer LVGL-Version auf **v9** stellen (nicht v8!)
- Nach Export: **keine** `.c` → `.cpp` Umbenennung mehr nötig (PlatformIO erkennt das automatisch)
- Bilder in EEZ auf **70x70** verkleinert (ursprünglich 200x200 → Flash-Speicher-Problem)

### C. `lv_conf.h` muss richtig liegen
- Datei gehört in den **`include/`** Ordner (nicht `src/`)
- `platformio.ini` braucht: `-DLV_CONF_INCLUDE_SIMPLE -I include`
- Montserrat-Fonts aktivieren: `#define LV_FONT_MONTSERRAT_40 1` etc.
- **Clean Build** (Mülleimer-Icon) nach jeder Änderung in `lv_conf.h`

### D. Screens.c Fixes (nach jedem EEZ-Export)
1. `event_handler_cb_main_arc()` neutralisieren (verhindert Linker-Fehler `set_var_speed`):
   ```c
   static void event_handler_cb_main_arc(lv_event_t *e) {
       (void)e;
   }
   ```
2. `tick_screen_main()` leeren:
   ```c
   void tick_screen_main() {
   }
   ```
3. `tick_screen_func_t` mit `(void)` deklarieren:
   ```c
   typedef void (*tick_screen_func_t)(void);
   ```

### E. LVGL v9 API-Änderungen
| LVGL v8 | LVGL v9 |
|---------|---------|
| `lv_img_dsc_t` | `lv_image_dsc_t` |
| `lv_img_set_src()` | `lv_image_set_src()` |
| `lv_img_set_zoom()` | `lv_image_set_scale()` |
| `lv_scr_load()` | `lv_screen_load_anim()` |
| `LV_IMG_CF_TRUE_COLOR_ALPHA` | `LV_COLOR_FORMAT_ARGB8888` |

### F. Display-Performance
- **Full-Screen-Buffer** (`240*240*2` Bytes) → kein Teearing, aber langsamer
- **Partial-Buffer** (`240*40*2` Bytes) + `LV_DISPLAY_RENDER_MODE_PARTIAL` → flüssiger
- **SPI-Speed erhöhen:** `tft.setSPISpeed(80000000);` (80 MHz)
- **`delay(0)`** statt `delay(5)` im Loop → flüssige Spinner-Animation

### G. `platformio.ini` (funktionierend)
```ini
[env:esp32-s3-devkitc-1]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
monitor_speed = 115200

lib_deps = 
    lvgl/lvgl@^9.2.2
    adafruit/Adafruit GFX Library@^1.12.6
    adafruit/Adafruit GC9A01A@^1.1.0

build_flags = 
    -DLV_CONF_INCLUDE_SIMPLE
    -I include
```

---

## 4. UI-Struktur (aktuell)

### Immer aktiv
| Objekt | Typ | Zweck |
|--------|-----|-------|
| `battery` | Image | Statisches Akku-Rahmen-Bild |
| `batterylevel` | Bar | Akkustand 0-100 |
| `playpause` | Image | Musik-Icon (Play/Pause) |
| `call` | Image | Telefon-Icon (Incoming/Outgoing/Ended) |
| `arc` | Arc | Geschwindigkeits-Bogen (in ALLEN Modi aktiv) |

### NavTurn (Modus 0)
- `direction` (Image, wechselnd) – 17 Manöver-Bilder
- `distance` (Label)
- `distance_entity` (Label)
- `street` (Label)

### NavSpeed (Modus 1)
- `speed_num` (Label)
- `speed_entity` (Label)

### NavTarget (Modus 2)
- `navi` (Image, rotierend) – zeigt `img_01_straight` rotiert

---

## 5. Serial-Befehle (Test-Interface)

| Befehl | Wirkung |
|--------|---------|
| `t` | Modus NavTurn |
| `s` | Modus NavSpeed |
| `v` | Modus NavTarget |
| `0-9`, `a-g` | Manöver 1-17 |
| `U` / `D` | Speed +10 / -10 |
| `+` / `-` | Akku +10% / -10% |
| `i` / `o` / `k` / `x` | Phone: Incoming / Outgoing / Ended / Aus |
| `m` | Musik Play/Pause Toggle |
| `H0`-`H100` | Helligkeit 0-100% |
| `h` | Helligkeit anzeigen |
| `r` | Navi +45° rotieren |
| `R` | Navi-Rotation Reset |
| `?` | Hilfe anzeigen |

---

## 6. Aktuelle `main.cpp` (funktionierend)

```cpp
#include <lvgl.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include "ui.h"
#include "images.h"

extern objects_t objects;

#define TFT_DC      8
#define TFT_CS      9
#define TFT_SCLK    10
#define TFT_MOSI    11
#define TFT_RST     12
#define TFT_MISO    -1
#define TFT_BL      40

#define MY_DISP_HOR_RES 240
#define MY_DISP_VER_RES 240

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST, TFT_MISO);

// Partial Buffer (schneller als Full-Screen)
static lv_color_t buf[MY_DISP_HOR_RES * 40];

void print_help();
void handleSerialCommands();

// Helligkeit (PWM)
#define PWM_CHANNEL   0
#define PWM_FREQ      5000
#define PWM_RES_BITS  8
int current_brightness = 100;

void set_brightness(int percent) {
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    current_brightness = percent;
    uint8_t duty = map(percent, 0, 100, 0, 255);
    ledcWrite(PWM_CHANNEL, duty);
    Serial.printf("Helligkeit: %d%% (PWM=%d)\n", percent, duty);
}

// Zustands-Management
enum UiState { STATE_NAV_TURN = 0, STATE_NAV_SPEED = 1, STATE_NAV_TARGET = 2 };
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
    lv_obj_add_flag(objects.direction, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.distance, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.distance_entity, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.street, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.speed_entity, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.speed_num, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.navi, LV_OBJ_FLAG_HIDDEN);

    switch (state) {
        case STATE_NAV_TURN:
            lv_obj_clear_flag(objects.direction, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(objects.distance, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(objects.distance_entity, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(objects.street, LV_OBJ_FLAG_HIDDEN);
            break;
        case STATE_NAV_SPEED:
            lv_obj_clear_flag(objects.speed_entity, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(objects.speed_num, LV_OBJ_FLAG_HIDDEN);
            break;
        case STATE_NAV_TARGET:
            lv_obj_clear_flag(objects.navi, LV_OBJ_FLAG_HIDDEN);
            break;
    }
    lv_obj_invalidate(lv_scr_act());
    lv_refr_now(NULL);
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
    Serial.println("→ Timer abgelaufen, wechsle zu Main Screen");
    lv_screen_load_anim(objects.main, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    set_ui_state(current_state);
    lv_timer_del(timer);
}

void print_help() {
    Serial.println("\n=== BEFEHLSÜBERSICHT ===");
    Serial.println("MODI:  t=NavTurn | s=NavSpeed | v=NavTarget");
    Serial.println("MANÖVER: 0-9, a-g");
    Serial.println("SPEED: U=+10 | D=-10");
    Serial.println("AKKU: + / -");
    Serial.println("PHONE: i=incoming | o=outgoing | k=ended | x=aus");
    Serial.println("MUSIK: m=toggle");
    Serial.println("HELLIGKEIT: H0-H100 | h=Anzeige");
    Serial.println("ROTATION: r=+45° | R=Reset");
    Serial.println("========================\n");
}

void setup() {
    Serial.begin(115200);
    Serial.println("=== NAVGET SYSTEM START ===");
    
    tft.begin();
    tft.setRotation(0);
    tft.setSPISpeed(80000000);  // 80 MHz
    
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES_BITS);
    ledcAttachPin(TFT_BL, PWM_CHANNEL);
    set_brightness(100);
    
    tft.fillScreen(0x0000);

    lv_init();
    lv_display_t *disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    create_screens();
    
    // === POSITIONEN UND SKALIERUNG ANPASSEN ===
    lv_obj_set_pos(objects.distance, 0, 30);
    lv_obj_set_pos(objects.distance_entity, 0, 30);
    lv_obj_set_align(objects.distance_entity, LV_ALIGN_TOP_MID);
    lv_obj_set_width(objects.street, 180);
    lv_obj_set_pos(objects.street, 0, -30);
    lv_label_set_long_mode(objects.street, LV_LABEL_LONG_SCROLL_CIRCULAR);
    
    lv_image_set_scale(objects.navi, 500);
    lv_image_set_scale(objects.direction, 700);
    lv_image_set_scale(objects.call, 100);
    lv_image_set_scale(objects.playpause, 100);
    // ==========================================
    
    lv_screen_load_anim(objects.boot, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    Serial.println("→ Boot-Screen geladen (mit Spinner), warte 3 Sekunden...");
    lv_timer_create(boot_to_main_timer_cb, 3000, NULL);

    lv_label_set_text(objects.street, "Hauptstrasse");
    lv_label_set_text(objects.distance, "250");
    lv_label_set_text(objects.distance_entity, "m");
    lv_label_set_text(objects.speed_num, "0");
    lv_label_set_text(objects.speed_entity, "km/h");
    lv_bar_set_value(objects.batterylevel, 100, LV_ANIM_OFF);
    lv_arc_set_range(objects.arc, 0, 100);
    lv_arc_set_value(objects.arc, 0);

    lv_obj_add_flag(objects.call, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.playpause, LV_OBJ_FLAG_HIDDEN);

    Serial.println("System bereit.");
    print_help();
}

void loop() {
    lv_tick_inc(1);
    lv_timer_handler();
    ui_tick();
    if (Serial.available()) {
        handleSerialCommands();
    }
    delay(0);
}

void handleSerialCommands() {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() == 0) return;
    char cmd = input.charAt(0);
    
    switch (cmd) {
        case 't': Serial.println("→ NavTurn"); set_ui_state(STATE_NAV_TURN); break;
        case 's': Serial.println("→ NavSpeed"); set_ui_state(STATE_NAV_SPEED); break;
        case 'v': Serial.println("→ NavTarget"); set_ui_state(STATE_NAV_TARGET); break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': {
            int idx = 0;
            if (cmd >= '0' && cmd <= '9') idx = cmd - '0';
            else idx = 10 + (cmd - 'a');
            if (idx < 17) {
                current_maneuver_index = idx;
                lv_image_set_src(objects.direction, maneuver_images[idx]);
                Serial.printf("→ Manöver %d gesetzt\n", idx + 1);
            }
            break;
        }
        case 'U': {
            int32_t cur = lv_arc_get_value(objects.arc);
            if (cur < 100) {
                lv_arc_set_value(objects.arc, cur + 10);
                char buf[8]; snprintf(buf, sizeof(buf), "%d", cur + 10);
                lv_label_set_text(objects.speed_num, buf);
                Serial.printf("→ Speed: %d\n", cur + 10);
            }
            break;
        }
        case 'D': {
            int32_t cur = lv_arc_get_value(objects.arc);
            if (cur > 0) {
                lv_arc_set_value(objects.arc, cur - 10);
                char buf[8]; snprintf(buf, sizeof(buf), "%d", cur - 10);
                lv_label_set_text(objects.speed_num, buf);
                Serial.printf("→ Speed: %d\n", cur - 10);
            }
            break;
        }
        case '+': {
            int32_t batt = lv_bar_get_value(objects.batterylevel);
            if (batt < 100) {
                lv_bar_set_value(objects.batterylevel, batt + 10, LV_ANIM_ON);
                Serial.printf("→ Akku: %d%%\n", batt + 10);
            }
            break;
        }
        case '-': {
            int32_t batt = lv_bar_get_value(objects.batterylevel);
            if (batt > 0) {
                lv_bar_set_value(objects.batterylevel, batt - 10, LV_ANIM_ON);
                Serial.printf("→ Akku: %d%%\n", batt - 10);
            }
            break;
        }
        case 'i':
            lv_image_set_src(objects.call, &img_incoming);
            lv_obj_clear_flag(objects.call, LV_OBJ_FLAG_HIDDEN);
            Serial.println("→ Phone: Incoming"); break;
        case 'o':
            lv_image_set_src(objects.call, &img_outgoing);
            lv_obj_clear_flag(objects.call, LV_OBJ_FLAG_HIDDEN);
            Serial.println("→ Phone: Outgoing"); break;
        case 'k':
            lv_image_set_src(objects.call, &img_ended);
            lv_obj_clear_flag(objects.call, LV_OBJ_FLAG_HIDDEN);
            Serial.println("→ Phone: Ended"); break;
        case 'x':
            lv_obj_add_flag(objects.call, LV_OBJ_FLAG_HIDDEN);
            Serial.println("→ Phone: Aus"); break;
        case 'm':
            music_playing = !music_playing;
            lv_image_set_src(objects.playpause, music_playing ? &img_play : &img_pause);
            lv_obj_clear_flag(objects.playpause, LV_OBJ_FLAG_HIDDEN);
            Serial.printf("→ Musik: %s\n", music_playing ? "PLAY" : "PAUSE"); break;
        case 'H': {
            int val = input.substring(1).toInt();
            set_brightness(val);
            break;
        }
        case 'h':
            Serial.printf("→ Aktuelle Helligkeit: %d%%\n", current_brightness); break;
        case 'r': {
            static int rotation = 0;
            rotation = (rotation + 450) % 3600;
            lv_image_set_rotation(objects.navi, rotation);
            Serial.printf("→ Navi Rotation: %d°\n", rotation / 10); break;
        }
        case 'R':
            lv_image_set_rotation(objects.navi, 0);
            Serial.println("→ Navi Rotation reset"); break;
        case '?':
            print_help(); break;
        default:
            Serial.printf("Unbekannter Befehl: '%s' (Tippe '?' für Hilfe)\n", input.c_str()); break;
    }
}
```

---

## 7. Aktuelle `screens.c` (Auszug – kritische Teile)

```c
// Event-Handler neutralisiert (verhindert Linker-Fehler)
static void event_handler_cb_main_arc(lv_event_t *e) {
    (void)e;
}

// Boot-Screen mit Spinner
void create_screen_boot() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.boot = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 41, 98);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xc9f601), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "Nav");
        }
        // ... weitere Labels "Get" und "Ride focused" ...
        {
            // Spinner
            lv_obj_t *spinner = lv_spinner_create(parent_obj);
            lv_obj_set_pos(spinner, 0, 0);
            lv_obj_set_size(spinner, 240, 240);
            lv_obj_set_style_align(spinner, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_opa(spinner, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_color(spinner, lv_color_hex(0xc9f601), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_width(spinner, 12, LV_PART_INDICATOR | LV_STATE_DEFAULT);
        }
    }
    tick_screen_boot();
}

// WICHTIG: tick_screen_main leer lassen!
void tick_screen_main() {
}

// C-Standard-konformer Typedef
typedef void (*tick_screen_func_t)(void);
static tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_boot,
    tick_screen_main
};
```

---

## 8. Checkliste für den Wiedereinstieg

### Nach einem EEZ-Export immer:
- [ ] In `screens.c`: `event_handler_cb_main_arc()` neutralisieren
- [ ] In `screens.c`: `tick_screen_main()` leeren
- [ ] In `screens.c`: `typedef void (*tick_screen_func_t)(void);` mit `(void)`
- [ ] In `main.cpp`: Skalierung/Positionen nach `create_screens()` setzen

### Bei Build-Problemen:
- [ ] `lv_conf.h` liegt in `include/` (nicht `src/`)
- [ ] Montserrat-Fonts aktiviert (`LV_FONT_MONTSERRAT_40 1` etc.)
- [ ] Clean Build (Mülleimer) durchgeführt
- [ ] Gelbe Warnungen (`deprecated`) ignorieren – sind keine Fehler

### Bei Display-Problemen:
- [ ] Richtiger COM-Port im Serial Monitor
- [ ] `monitor_speed = 115200` in `platformio.ini`
- [ ] Backlight-Pin 40 via PWM (`ledcSetup` + `ledcAttachPin`)

---

## 9. Offene To-Dos / Ideen

- [ ] `street`-Text an Rundung anpassen (aktuell 180px breit, y=-30)
- [ ] Manöver-Bilder dynamisch aus Bluetooth/Serial empfangen
- [ ] Akku-Wert aus echter Batterie-Spannung lesen
- [ ] GPS-Geschwindigkeit für `arc` und `speed_num` anbinden
- [ ] Telefon-Status via Bluetooth (z.B. A2DP/HFP)
- [ ] Musik-Status via Bluetooth (AVRCP)

---

**Speichern als:** `NAVGET_PROJEKTSTAND.md` im Projektordner  
**Zuletzt aktualisiert:** September 2026  
**Status:** ✅ Display läuft, UI funktioniert, alle Modi testbar