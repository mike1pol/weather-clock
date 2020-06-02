#define DEBUG false
#define RESET_CLOCK false

// Button
#define BUTTON_PIN 2
#define BUTTON_INTERRUPT true

// Battery checker
#define BATTERY true
#define BATTERY_CALIBRATION false
#define BATTERY_PIN A3 // Port name in pinout
#define BATTERY_MIN (float)2.75
#define BATTERY_MAX (float)4.35

// Timers
#define BATTERY_TIMER 1000
#define SENSOR_TIMER 28000
#define CLOCK_TIMER 500
#define PHOTO_TIMER 2000
#define SWITCH_TIMER 15000
#define HOUR_TIMER (long)4 * 60 * 1000
#define DAY_TIMER (long)1.6 * 60 * 60 * 1000
#define PREDICT_TIMER (long)10 * 60 * 1000

// Photoresistor
#define PHOTO_PIN A1
#define PHOTO_MIN 30
#define PHOTO_MAX 800
#define LCD_BRI_MIN 50
#define LCD_BRI_MAX 255

// LCD
#define LCD_RS_PIN 9
#define LCD_E_PIN 8
#define LCD_D4_PIN A0
#define LCD_D5_PIN 13
#define LCD_D6_PIN 12
#define LCD_D7_PIN 11
#define LCD_BRI_PIN 10

// BME
#define BME_ADDRESS 0x76
