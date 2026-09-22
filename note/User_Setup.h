//                            USER DEFINED SETTINGS
//   Set driver type, fonts to be loaded, pins used and SPI control method etc.
//
//   See the User_Setup_Select.h file if you wish to be able to define multiple
//   setups and then easily select which setup file is used by the compiler.
//
//   If this file is edited correctly then all the library example sketches should
//   run without the need to make any more changes for a particular hardware setup!

#define USER_SETUP_INFO "User_Setup"

// ##################################################################################
//
// Section 1. Call up the right driver file and any options for it
//
// ##################################################################################

// Only define one driver, the other ones must be commented out
//#define ILI9341_DRIVER
//#define ST7735_DRIVER
//#define ILI9163_DRIVER
//#define S6D02A1_DRIVER
//#define RPI_ILI9486_DRIVER
//#define HX8357D_DRIVER
//#define ILI9481_DRIVER
//#define ILI9486_DRIVER
//#define ILI9488_DRIVER
//#define ST7789_DRIVER
//#define R61581_DRIVER
//#define RM68140_DRIVER
//#define ST7796_DRIVER
//#define SSD1351_DRIVER
//#define SSD1963_480_DRIVER
//#define SSD1963_800_DRIVER
//#define ILI9225_DRIVER

// >>> HIER DEIN TREIBER: GC9A01 (Rundes Display) <<<
#define GC9A01_DRIVER

// Auflösung des runden Displays
#define TFT_WIDTH  240
#define TFT_HEIGHT 240

// ##################################################################################
//
// Section 2. Define the pins that are used to interface with the display here
//
// ##################################################################################

// >>> HIER DEINE PIN-BELEGUNG (ESP32-S3) <<<
#define TFT_MOSI 11
#define TFT_SCLK 10
#define TFT_CS    9  // Chip select control pin
#define TFT_DC    8  // Data Command control pin
#define TFT_RST  12  // Reset pin
#define TFT_MISO -1  // MISO nicht verbunden
#define TFT_BL   40  // Backlight Pin

// ##################################################################################
//
// Section 3. Define the fonts that are to be used here
//
// ##################################################################################

// Comment out the #defines below with // to stop that font being loaded
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts
#define SMOOTH_FONT

// ##################################################################################
//
// Section 4. Other options
//
// ##################################################################################

// SPI Frequenz: 40MHz ist stabil und schnell für GC9A01 am ESP32-S3
#define SPI_FREQUENCY  40000000

// Optional reduced SPI frequency for reading TFT
#define SPI_READ_FREQUENCY  20000000

// The XPT2046 requires a lower SPI clock rate of 2.5MHz so we define that here:
#define SPI_TOUCH_FREQUENCY  2500000