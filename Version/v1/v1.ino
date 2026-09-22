/*
  ============================================================
  NavGet - LVGL UI Starter
  ESP32-S3 + GC9A01A 240x240
  LVGL 9.x
  ============================================================

  Ziel dieses Sketches:

  - Hardware-Anbindung des GC9A01A bleibt wie bei deinem
    funktionierenden ersten Sketch.

  - Die komplette Oberfläche wird jetzt von LVGL verwaltet.

  - Du kannst Positionen einfach über X/Y bzw. ALIGN ändern.

  - LVGL kümmert sich selbst darum, nur geänderte Bereiche
    neu zu rendern.

  - Bootscreen erscheint nur EINMAL.

  - Danach:
      Akku
      Distanz
      Telefon
      Navigation/Zielrichtung
      Musik
      Straßenname

    als unabhängige LVGL-Objekte.

  ------------------------------------------------------------
  Layout
  ------------------------------------------------------------

                         AKKU
                       DISTANZ


     TELEFON           MANÖVER            MUSIK



                      STRASSE


  ------------------------------------------------------------
  Serial Test
  ------------------------------------------------------------

  1 = Turn-by-Turn / Zielrichtung
  2 = Telefonstatus
  3 = Play/Pause

  n = nächstes Manöver

  a = Zielrichtung 10° links
  d = Zielrichtung 10° rechts

  i = eingehender Anruf
  c = Gespräch aktiv
  x = kein Anruf

  + = Akku höher
  - = Akku niedriger
*/


#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

#include <lvgl.h>

#include <math.h>


// ============================================================
// DISPLAY
// ============================================================

#define TFT_DC    8
#define TFT_CS    9
#define TFT_SCLK  10
#define TFT_MOSI  11
#define TFT_RST   12
#define TFT_MISO  -1
#define TFT_BL    40


#define SCREEN_WIDTH   240
#define SCREEN_HEIGHT  240


Adafruit_GC9A01A tft(
  TFT_CS,
  TFT_DC,
  TFT_MOSI,
  TFT_SCLK,
  TFT_RST,
  TFT_MISO
);


// ============================================================
// LVGL DRAW BUFFER
//
// 30 Zeilen reichen für PARTIAL Rendering.
// ============================================================

static uint16_t lvBuffer[
  SCREEN_WIDTH * 30
];


lv_display_t* lvDisplay;


// ============================================================
// COLORS
// ============================================================

const uint32_t COLOR_BACKGROUND =
  0x07090A;

const uint32_t COLOR_NAV =
  0xBEFF34;

const uint32_t COLOR_TEXT =
  0xF5F7F9;

const uint32_t COLOR_MUTED =
  0x747E86;

const uint32_t COLOR_PHONE_GREEN =
  0x45E164;

const uint32_t COLOR_PHONE_RED =
  0xF53C44;

const uint32_t COLOR_PHONE_GREY =
  0x596168;


// ============================================================
// OPTIONAL FONTS
//
// Wenn du in lv_conf.h größere Montserrat-Fonts aktivierst,
// verwendet der Sketch sie automatisch.
//
// LV_FONT_MONTSERRAT_18 1
// LV_FONT_MONTSERRAT_24 1
// LV_FONT_MONTSERRAT_32 1
// ============================================================

const lv_font_t* fontMedium() {

#if LV_FONT_MONTSERRAT_18
  return &lv_font_montserrat_18;
#else
  return LV_FONT_DEFAULT;
#endif
}


const lv_font_t* fontLarge() {

#if LV_FONT_MONTSERRAT_24
  return &lv_font_montserrat_24;
#elif LV_FONT_MONTSERRAT_18
  return &lv_font_montserrat_18;
#else
  return LV_FONT_DEFAULT;
#endif
}


const lv_font_t* fontHuge() {

#if LV_FONT_MONTSERRAT_32
  return &lv_font_montserrat_32;
#elif LV_FONT_MONTSERRAT_24
  return &lv_font_montserrat_24;
#else
  return LV_FONT_DEFAULT;
#endif
}


// ============================================================
// LVGL OBJECTS
// ============================================================

lv_obj_t* bootScreen;
lv_obj_t* mainScreen;

lv_obj_t* batteryLabel;
lv_obj_t* distanceLabel;
lv_obj_t* streetLabel;

lv_obj_t* phoneLabel;
lv_obj_t* musicLabel;

lv_obj_t* navigationArea;

lv_obj_t* navLine1;
lv_obj_t* navLine2;
lv_obj_t* navLine3;
lv_obj_t* navLine4;

lv_obj_t* roundaboutLabel;


// ============================================================
// NAVIGATION LINE POINTS
//
// Müssen global/static bleiben.
// LVGL speichert Pointer darauf.
// ============================================================

static lv_point_precise_t line1Points[2];
static lv_point_precise_t line2Points[2];
static lv_point_precise_t line3Points[2];
static lv_point_precise_t line4Points[2];


// ============================================================
// STATE
// ============================================================

enum NavigationMode {

  NAV_TURN,

  NAV_TARGET
};


enum Maneuver {

  MANEUVER_STRAIGHT,

  MANEUVER_LEFT,

  MANEUVER_RIGHT,

  MANEUVER_KEEP_LEFT,

  MANEUVER_KEEP_RIGHT,

  MANEUVER_ROUNDABOUT
};


enum CallState {

  CALL_IDLE,

  CALL_INCOMING,

  CALL_ACTIVE,

  CALL_REJECTED
};


NavigationMode navigationMode =
  NAV_TURN;


Maneuver currentManeuver =
  MANEUVER_RIGHT;


CallState callState =
  CALL_IDLE;


bool musicPlaying =
  false;


int batteryPercent =
  75;


float targetDirection =
  35.0f;


String currentDistance =
  "350 m";


String currentStreet =
  "Musterstrasse";


// ============================================================
// DEMO
// ============================================================

struct DemoStep {

  Maneuver maneuver;

  const char* distance;

  const char* street;

  float targetAngle;
};


DemoStep demoRoute[] = {

  {
    MANEUVER_STRAIGHT,
    "1.2 km",
    "Landstrasse",
    0
  },

  {
    MANEUVER_RIGHT,
    "350 m",
    "Musterstrasse",
    35
  },

  {
    MANEUVER_LEFT,
    "180 m",
    "Hauptstrasse",
    -45
  },

  {
    MANEUVER_KEEP_RIGHT,
    "650 m",
    "B 27",
    20
  },

  {
    MANEUVER_ROUNDABOUT,
    "90 m",
    "2. Ausfahrt",
    70
  }
};


constexpr size_t DEMO_COUNT =
  sizeof(demoRoute)
  /
  sizeof(demoRoute[0]);


size_t demoIndex =
  1;


// ============================================================
// LVGL -> DISPLAY FLUSH
//
// LVGL rendert nur den Bereich "area".
//
// Dieser wird direkt an den GC9A01A geschickt.
// ============================================================

void displayFlush(
  lv_display_t* display,
  const lv_area_t* area,
  uint8_t* pixelMap
) {

  uint16_t width =
    area->x2
    -
    area->x1
    +
    1;


  uint16_t height =
    area->y2
    -
    area->y1
    +
    1;


  uint32_t pixelCount =
    width
    *
    height;


  tft.startWrite();


  tft.setAddrWindow(
    area->x1,
    area->y1,
    width,
    height
  );


  tft.writePixels(
    reinterpret_cast<uint16_t*>(pixelMap),
    pixelCount,
    true,
    false
  );


  tft.endWrite();


  lv_display_flush_ready(
    display
  );
}


// ============================================================
// HELPER: LABEL
// ============================================================

lv_obj_t* createLabel(
  lv_obj_t* parent,
  const char* text,
  lv_color_t color,
  const lv_font_t* font
) {

  lv_obj_t* label =
    lv_label_create(
      parent
    );


  lv_label_set_text(
    label,
    text
  );


  lv_obj_set_style_text_color(
    label,
    color,
    0
  );


  lv_obj_set_style_text_font(
    label,
    font,
    0
  );


  return label;
}


// ============================================================
// NAV LINES
// ============================================================

void configureNavigationLine(
  lv_obj_t* line
) {

  lv_obj_set_size(
    line,
    110,
    110
  );


  lv_obj_set_pos(
    line,
    0,
    0
  );


  lv_obj_set_style_line_color(
    line,
    lv_color_hex(
      COLOR_NAV
    ),
    0
  );


  lv_obj_set_style_line_width(
    line,
    8,
    0
  );


  lv_obj_set_style_line_rounded(
    line,
    true,
    0
  );
}


// ============================================================
// SHOW/HIDE LINES
// ============================================================

void showNavigationLines() {

  lv_obj_clear_flag(
    navLine1,
    LV_OBJ_FLAG_HIDDEN
  );

  lv_obj_clear_flag(
    navLine2,
    LV_OBJ_FLAG_HIDDEN
  );

  lv_obj_clear_flag(
    navLine3,
    LV_OBJ_FLAG_HIDDEN
  );

  lv_obj_clear_flag(
    navLine4,
    LV_OBJ_FLAG_HIDDEN
  );


  lv_obj_add_flag(
    roundaboutLabel,
    LV_OBJ_FLAG_HIDDEN
  );
}


// ============================================================
// MANEUVER: STRAIGHT
// ============================================================

void drawStraightManeuver() {

  showNavigationLines();


  line1Points[0] = {
    55,
    95
  };

  line1Points[1] = {
    55,
    20
  };


  line2Points[0] = {
    55,
    20
  };

  line2Points[1] = {
    38,
    42
  };


  line3Points[0] = {
    55,
    20
  };

  line3Points[1] = {
    72,
    42
  };


  // unsichtbarer Zusatz

  line4Points[0] = {
    55,
    20
  };

  line4Points[1] = {
    55,
    20
  };


  lv_line_set_points(
    navLine1,
    line1Points,
    2
  );

  lv_line_set_points(
    navLine2,
    line2Points,
    2
  );

  lv_line_set_points(
    navLine3,
    line3Points,
    2
  );

  lv_line_set_points(
    navLine4,
    line4Points,
    2
  );
}


// ============================================================
// MANEUVER: RIGHT
// ============================================================

void drawRightManeuver() {

  showNavigationLines();


  // vertikal hoch

  line1Points[0] = {
    40,
    95
  };

  line1Points[1] = {
    40,
    48
  };


  // rechts

  line2Points[0] = {
    40,
    48
  };

  line2Points[1] = {
    88,
    48
  };


  // Pfeilkopf oben

  line3Points[0] = {
    88,
    48
  };

  line3Points[1] = {
    69,
    31
  };


  // Pfeilkopf unten

  line4Points[0] = {
    88,
    48
  };

  line4Points[1] = {
    69,
    65
  };


  lv_line_set_points(
    navLine1,
    line1Points,
    2
  );

  lv_line_set_points(
    navLine2,
    line2Points,
    2
  );

  lv_line_set_points(
    navLine3,
    line3Points,
    2
  );

  lv_line_set_points(
    navLine4,
    line4Points,
    2
  );
}


// ============================================================
// MANEUVER: LEFT
// ============================================================

void drawLeftManeuver() {

  showNavigationLines();


  line1Points[0] = {
    70,
    95
  };

  line1Points[1] = {
    70,
    48
  };


  line2Points[0] = {
    70,
    48
  };

  line2Points[1] = {
    22,
    48
  };


  line3Points[0] = {
    22,
    48
  };

  line3Points[1] = {
    41,
    31
  };


  line4Points[0] = {
    22,
    48
  };

  line4Points[1] = {
    41,
    65
  };


  lv_line_set_points(
    navLine1,
    line1Points,
    2
  );

  lv_line_set_points(
    navLine2,
    line2Points,
    2
  );

  lv_line_set_points(
    navLine3,
    line3Points,
    2
  );

  lv_line_set_points(
    navLine4,
    line4Points,
    2
  );
}


// ============================================================
// KEEP RIGHT
// ============================================================

void drawKeepRightManeuver() {

  showNavigationLines();


  line1Points[0] = {
    45,
    95
  };

  line1Points[1] = {
    45,
    63
  };


  line2Points[0] = {
    45,
    63
  };

  line2Points[1] = {
    78,
    28
  };


  line3Points[0] = {
    78,
    28
  };

  line3Points[1] = {
    57,
    34
  };


  line4Points[0] = {
    78,
    28
  };

  line4Points[1] = {
    73,
    50
  };


  lv_line_set_points(
    navLine1,
    line1Points,
    2
  );

  lv_line_set_points(
    navLine2,
    line2Points,
    2
  );

  lv_line_set_points(
    navLine3,
    line3Points,
    2
  );

  lv_line_set_points(
    navLine4,
    line4Points,
    2
  );
}


// ============================================================
// KEEP LEFT
// ============================================================

void drawKeepLeftManeuver() {

  showNavigationLines();


  line1Points[0] = {
    65,
    95
  };

  line1Points[1] = {
    65,
    63
  };


  line2Points[0] = {
    65,
    63
  };

  line2Points[1] = {
    32,
    28
  };


  line3Points[0] = {
    32,
    28
  };

  line3Points[1] = {
    53,
    34
  };


  line4Points[0] = {
    32,
    28
  };

  line4Points[1] = {
    37,
    50
  };


  lv_line_set_points(
    navLine1,
    line1Points,
    2
  );

  lv_line_set_points(
    navLine2,
    line2Points,
    2
  );

  lv_line_set_points(
    navLine3,
    line3Points,
    2
  );

  lv_line_set_points(
    navLine4,
    line4Points,
    2
  );
}


// ============================================================
// ROUNDABOUT
//
// Erstmal bewusst LVGL-Symbol.
//
// Das kannst du später gegen ein eigenes PNG/SVG/C-Image
// ersetzen.
// ============================================================

void drawRoundaboutManeuver() {

  lv_obj_add_flag(
    navLine1,
    LV_OBJ_FLAG_HIDDEN
  );

  lv_obj_add_flag(
    navLine2,
    LV_OBJ_FLAG_HIDDEN
  );

  lv_obj_add_flag(
    navLine3,
    LV_OBJ_FLAG_HIDDEN
  );

  lv_obj_add_flag(
    navLine4,
    LV_OBJ_FLAG_HIDDEN
  );


  lv_obj_clear_flag(
    roundaboutLabel,
    LV_OBJ_FLAG_HIDDEN
  );
}


// ============================================================
// TARGET DIRECTION
//
// DAS ist dein "Kompass"-Ersatz.
//
// Es gibt keine Himmelsrichtungen.
//
// 0°   = geradeaus
// 90°  = rechts
// -90° = links
//
// Wir zeichnen den Pfeil mathematisch.
// ============================================================

void drawTargetDirection(
  float angleDegrees
) {

  showNavigationLines();


  const float centerX =
    55.0f;


  const float centerY =
    60.0f;


  const float angle =
    (
      angleDegrees
      -
      90.0f
    )
    *
    DEG_TO_RAD;


  const float tailLength =
    34.0f;


  const float tipLength =
    45.0f;


  float tailX =
    centerX
    -
    cosf(angle)
    *
    tailLength;


  float tailY =
    centerY
    -
    sinf(angle)
    *
    tailLength;


  float tipX =
    centerX
    +
    cosf(angle)
    *
    tipLength;


  float tipY =
    centerY
    +
    sinf(angle)
    *
    tipLength;


  // Hauptpfeil

  line1Points[0] = {
    (lv_value_precise_t)tailX,
    (lv_value_precise_t)tailY
  };

  line1Points[1] = {
    (lv_value_precise_t)tipX,
    (lv_value_precise_t)tipY
  };


  // Pfeilkopf

  const float headLength =
    20.0f;


  const float headAngle1 =
    angle
    +
    radians(
      145
    );


  const float headAngle2 =
    angle
    -
    radians(
      145
    );


  line2Points[0] = {
    (lv_value_precise_t)tipX,
    (lv_value_precise_t)tipY
  };


  line2Points[1] = {

    (lv_value_precise_t)(
      tipX
      +
      cosf(headAngle1)
      *
      headLength
    ),

    (lv_value_precise_t)(
      tipY
      +
      sinf(headAngle1)
      *
      headLength
    )
  };


  line3Points[0] = {
    (lv_value_precise_t)tipX,
    (lv_value_precise_t)tipY
  };


  line3Points[1] = {

    (lv_value_precise_t)(
      tipX
      +
      cosf(headAngle2)
      *
      headLength
    ),

    (lv_value_precise_t)(
      tipY
      +
      sinf(headAngle2)
      *
      headLength
    )
  };


  line4Points[0] = {
    (lv_value_precise_t)tipX,
    (lv_value_precise_t)tipY
  };

  line4Points[1] = {
    (lv_value_precise_t)tipX,
    (lv_value_precise_t)tipY
  };


  lv_line_set_points(
    navLine1,
    line1Points,
    2
  );

  lv_line_set_points(
    navLine2,
    line2Points,
    2
  );

  lv_line_set_points(
    navLine3,
    line3Points,
    2
  );

  lv_line_set_points(
    navLine4,
    line4Points,
    2
  );
}


// ============================================================
// UPDATE NAVIGATION
// ============================================================

void updateNavigationGraphic() {

  if (
    navigationMode
    ==
    NAV_TARGET
  ) {

    drawTargetDirection(
      targetDirection
    );

    return;
  }


  switch (
    currentManeuver
  ) {

    case MANEUVER_LEFT:

      drawLeftManeuver();

      break;


    case MANEUVER_RIGHT:

      drawRightManeuver();

      break;


    case MANEUVER_KEEP_LEFT:

      drawKeepLeftManeuver();

      break;


    case MANEUVER_KEEP_RIGHT:

      drawKeepRightManeuver();

      break;


    case MANEUVER_ROUNDABOUT:

      drawRoundaboutManeuver();

      break;


    case MANEUVER_STRAIGHT:

    default:

      drawStraightManeuver();

      break;
  }
}


// ============================================================
// UPDATE BATTERY
// ============================================================

void updateBattery() {

  const char* icon;


  if (
    batteryPercent >= 80
  ) {

    icon =
      LV_SYMBOL_BATTERY_FULL;

  }

  else if (
    batteryPercent >= 60
  ) {

    icon =
      LV_SYMBOL_BATTERY_3;

  }

  else if (
    batteryPercent >= 40
  ) {

    icon =
      LV_SYMBOL_BATTERY_2;

  }

  else if (
    batteryPercent >= 20
  ) {

    icon =
      LV_SYMBOL_BATTERY_1;

  }

  else {

    icon =
      LV_SYMBOL_BATTERY_EMPTY;
  }


  lv_label_set_text(
    batteryLabel,
    icon
  );


  if (
    batteryPercent <= 15
  ) {

    lv_obj_set_style_text_color(
      batteryLabel,
      lv_color_hex(
        COLOR_PHONE_RED
      ),
      0
    );

  }

  else {

    lv_obj_set_style_text_color(
      batteryLabel,
      lv_color_hex(
        COLOR_MUTED
      ),
      0
    );
  }
}


// ============================================================
// UPDATE PHONE
// ============================================================

void updatePhone() {

  lv_label_set_text(
    phoneLabel,
    LV_SYMBOL_CALL
  );


  switch (
    callState
  ) {

    case CALL_INCOMING:

    case CALL_ACTIVE:

      lv_obj_set_style_text_color(
        phoneLabel,
        lv_color_hex(
          COLOR_PHONE_GREEN
        ),
        0
      );

      break;


    case CALL_REJECTED:

      lv_obj_set_style_text_color(
        phoneLabel,
        lv_color_hex(
          COLOR_PHONE_RED
        ),
        0
      );

      break;


    case CALL_IDLE:

    default:

      lv_obj_set_style_text_color(
        phoneLabel,
        lv_color_hex(
          COLOR_PHONE_GREY
        ),
        0
      );

      break;
  }
}


// ============================================================
// UPDATE MUSIC
// ============================================================

void updateMusic() {

  if (
    musicPlaying
  ) {

    lv_label_set_text(
      musicLabel,
      LV_SYMBOL_PAUSE
    );

  }

  else {

    lv_label_set_text(
      musicLabel,
      LV_SYMBOL_PLAY
    );
  }
}


// ============================================================
// UPDATE DISTANCE
// ============================================================

void updateDistance() {

  lv_label_set_text(
    distanceLabel,
    currentDistance.c_str()
  );
}


// ============================================================
// UPDATE STREET
// ============================================================

void updateStreet() {

  lv_label_set_text(
    streetLabel,
    currentStreet.c_str()
  );
}


// ============================================================
// CREATE BOOT SCREEN
// ============================================================

void createBootScreen() {

  bootScreen =
    lv_obj_create(
      nullptr
    );


  lv_obj_remove_style_all(
    bootScreen
  );


  lv_obj_set_style_bg_color(
    bootScreen,
    lv_color_hex(
      COLOR_BACKGROUND
    ),
    0
  );


  lv_obj_set_style_bg_opa(
    bootScreen,
    LV_OPA_COVER,
    0
  );


  lv_obj_t* logo =
    createLabel(
      bootScreen,
      "NavGet",
      lv_color_hex(
        COLOR_TEXT
      ),
      fontHuge()
    );


  lv_obj_align(
    logo,
    LV_ALIGN_CENTER,
    0,
    -10
  );


  lv_obj_t* subtitle =
    createLabel(
      bootScreen,
      "RIDE FOCUSED",
      lv_color_hex(
        COLOR_NAV
      ),
      LV_FONT_DEFAULT
    );


  lv_obj_align(
    subtitle,
    LV_ALIGN_CENTER,
    0,
    28
  );
}


// ============================================================
// CREATE MAIN UI
//
// HIER kannst du dein Layout basteln.
//
// Das sind die Stellen, die du wahrscheinlich am meisten
// verändern wirst.
// ============================================================

void createMainScreen() {

  mainScreen =
    lv_obj_create(
      nullptr
    );


  lv_obj_remove_style_all(
    mainScreen
  );


  lv_obj_set_style_bg_color(
    mainScreen,
    lv_color_hex(
      COLOR_BACKGROUND
    ),
    0
  );


  lv_obj_set_style_bg_opa(
    mainScreen,
    LV_OPA_COVER,
    0
  );


  // ==========================================================
  // AKKU
  // ==========================================================

  batteryLabel =
    createLabel(
      mainScreen,
      LV_SYMBOL_BATTERY_3,
      lv_color_hex(
        COLOR_MUTED
      ),
      fontMedium()
    );


  lv_obj_align(
    batteryLabel,
    LV_ALIGN_TOP_MID,

    0,     // X

    5      // Y
  );


  // ==========================================================
  // DISTANZ
  // ==========================================================

  distanceLabel =
    createLabel(
      mainScreen,
      "350 m",
      lv_color_hex(
        COLOR_NAV
      ),
      fontLarge()
    );


  lv_obj_align(
    distanceLabel,
    LV_ALIGN_TOP_MID,

    0,

    29
  );


  // ==========================================================
  // NAVIGATION AREA
  //
  // Größe und Position kannst du frei ändern.
  // ==========================================================

  navigationArea =
    lv_obj_create(
      mainScreen
    );


  lv_obj_remove_style_all(
    navigationArea
  );


  lv_obj_set_size(
    navigationArea,

    110,

    110
  );


  lv_obj_align(
    navigationArea,

    LV_ALIGN_CENTER,

    0,

    4
  );


  // ----------------------------------------------------------
  // Navigation lines
  // ----------------------------------------------------------

  navLine1 =
    lv_line_create(
      navigationArea
    );


  navLine2 =
    lv_line_create(
      navigationArea
    );


  navLine3 =
    lv_line_create(
      navigationArea
    );


  navLine4 =
    lv_line_create(
      navigationArea
    );


  configureNavigationLine(
    navLine1
  );

  configureNavigationLine(
    navLine2
  );

  configureNavigationLine(
    navLine3
  );

  configureNavigationLine(
    navLine4
  );


  // ----------------------------------------------------------
  // Roundabout placeholder
  // ----------------------------------------------------------

  roundaboutLabel =
    createLabel(
      navigationArea,

      LV_SYMBOL_REFRESH,

      lv_color_hex(
        COLOR_NAV
      ),

      fontHuge()
    );


  lv_obj_center(
    roundaboutLabel
  );


  lv_obj_add_flag(
    roundaboutLabel,
    LV_OBJ_FLAG_HIDDEN
  );


  // ==========================================================
  // TELEFON
  //
  // links der Mitte
  // ==========================================================

  phoneLabel =
    createLabel(
      mainScreen,

      LV_SYMBOL_CALL,

      lv_color_hex(
        COLOR_PHONE_GREY
      ),

      fontLarge()
    );


  lv_obj_align(
    phoneLabel,

    LV_ALIGN_LEFT_MID,

    18,

    2
  );


  // ==========================================================
  // MUSIK
  //
  // rechts der Mitte
  // ==========================================================

  musicLabel =
    createLabel(
      mainScreen,

      LV_SYMBOL_PLAY,

      lv_color_hex(
        COLOR_NAV
      ),

      fontLarge()
    );


  lv_obj_align(
    musicLabel,

    LV_ALIGN_RIGHT_MID,

    -18,

    2
  );


  // ==========================================================
  // STRASSE
  //
  // weit unten, mittig
  // ==========================================================

  streetLabel =
    createLabel(
      mainScreen,

      "Musterstrasse",

      lv_color_hex(
        COLOR_NAV
      ),

      fontMedium()
    );


  lv_obj_set_width(
    streetLabel,
    180
  );


  lv_obj_set_style_text_align(
    streetLabel,
    LV_TEXT_ALIGN_CENTER,
    0
  );


  lv_obj_align(
    streetLabel,

    LV_ALIGN_BOTTOM_MID,

    0,

    -20
  );


  // ==========================================================
  // initial values
  // ==========================================================

  updateBattery();

  updateDistance();

  updatePhone();

  updateMusic();

  updateStreet();

  updateNavigationGraphic();
}


// ============================================================
// LOAD DEMO
// ============================================================

void loadDemoStep(
  size_t index
) {

  if (
    index >=
    DEMO_COUNT
  ) {

    index =
      0;
  }


  demoIndex =
    index;


  currentManeuver =
    demoRoute[
      index
    ].maneuver;


  currentDistance =
    demoRoute[
      index
    ].distance;


  currentStreet =
    demoRoute[
      index
    ].street;


  targetDirection =
    demoRoute[
      index
    ].targetAngle;


  updateDistance();

  updateStreet();

  updateNavigationGraphic();
}


// ============================================================
// BUTTON 1
// ============================================================

void button1() {

  if (
    navigationMode
    ==
    NAV_TURN
  ) {

    navigationMode =
      NAV_TARGET;

  }

  else {

    navigationMode =
      NAV_TURN;
  }


  updateNavigationGraphic();
}


// ============================================================
// BUTTON 2
// ============================================================

void button2() {

  if (
    callState
    ==
    CALL_IDLE
  ) {

    callState =
      CALL_INCOMING;

  }

  else if (
    callState
    ==
    CALL_INCOMING
  ) {

    callState =
      CALL_ACTIVE;

  }

  else {

    callState =
      CALL_IDLE;
  }


  updatePhone();
}


// ============================================================
// BUTTON 3
// ============================================================

void button3() {

  musicPlaying =
    !musicPlaying;


  updateMusic();
}


// ============================================================
// SERIAL
// ============================================================

void handleSerial() {

  while (
    Serial.available()
  ) {

    char command =
      Serial.read();


    switch (
      command
    ) {

      case '1':

        button1();

        break;


      case '2':

        button2();

        break;


      case '3':

        button3();

        break;


      case 'n':

      case 'N':

        demoIndex++;


        if (
          demoIndex >=
          DEMO_COUNT
        ) {

          demoIndex =
            0;
        }


        loadDemoStep(
          demoIndex
        );

        break;


      case 'a':

      case 'A':

        targetDirection -=
          10;


        updateNavigationGraphic();

        break;


      case 'd':

      case 'D':

        targetDirection +=
          10;


        updateNavigationGraphic();

        break;


      case 'i':

      case 'I':

        callState =
          CALL_INCOMING;


        updatePhone();

        break;


      case 'c':

      case 'C':

        callState =
          CALL_ACTIVE;


        updatePhone();

        break;


      case 'x':

      case 'X':

        callState =
          CALL_IDLE;


        updatePhone();

        break;


      case '+':

        batteryPercent +=
          10;


        if (
          batteryPercent > 100
        ) {

          batteryPercent =
            100;
        }


        updateBattery();

        break;


      case '-':

        batteryPercent -=
          10;


        if (
          batteryPercent < 0
        ) {

          batteryPercent =
            0;
        }


        updateBattery();

        break;
    }
  }
}


// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(
    115200
  );


  delay(
    300
  );


  // ==========================================================
  // TFT
  // ==========================================================

  pinMode(
    TFT_BL,
    OUTPUT
  );


  digitalWrite(
    TFT_BL,
    HIGH
  );


  tft.begin(
    40000000
  );


  tft.setRotation(
    0
  );


  tft.fillScreen(
    GC9A01A_BLACK
  );


  // ==========================================================
  // LVGL
  // ==========================================================

  lv_init();


  lvDisplay =
    lv_display_create(
      SCREEN_WIDTH,
      SCREEN_HEIGHT
    );


  lv_display_set_color_format(
    lvDisplay,
    LV_COLOR_FORMAT_RGB565
  );


  lv_display_set_flush_cb(
    lvDisplay,
    displayFlush
  );


  lv_display_set_buffers(
    lvDisplay,

    lvBuffer,

    nullptr,

    sizeof(
      lvBuffer
    ),

    LV_DISPLAY_RENDER_MODE_PARTIAL
  );


  // ==========================================================
  // SCREENS
  // ==========================================================

  createBootScreen();

  createMainScreen();


  // ==========================================================
  // BOOTSCREEN EINMAL
  // ==========================================================

  lv_screen_load(
    bootScreen
  );


  // einmal rendern

  lv_timer_handler();


  delay(
    1500
  );


  // ==========================================================
  // Danach Hauptscreen.
  //
  // Bootscreen wird ab hier nie wieder geladen.
  // ==========================================================

  lv_screen_load(
    mainScreen
  );


  // Bootscreen kann sogar gelöscht werden.

  lv_obj_delete(
    bootScreen
  );


  bootScreen =
    nullptr;


  Serial.println(
    "NavGet LVGL ready"
  );
}


// ============================================================
// LOOP
// ============================================================

void loop() {

  // ==========================================================
  // LVGL braucht einen Zeit-Tick.
  // ==========================================================

  static uint32_t previousMillis =
    millis();


  uint32_t currentMillis =
    millis();


  uint32_t elapsed =
    currentMillis
    -
    previousMillis;


  previousMillis =
    currentMillis;


  lv_tick_inc(
    elapsed
  );


  // ==========================================================
  // Testbefehle
  // ==========================================================

  handleSerial();


  // ==========================================================
  // LVGL Rendering / Events
  // ==========================================================

  lv_timer_handler();


  delay(
    5
  );
}