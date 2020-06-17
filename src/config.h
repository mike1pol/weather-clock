#define DEBUG true
#define RESET_CLOCK false
#define LED_PIN 7

// Button
#define BUTTON_PIN 2
#define BUTTON_INTERRUPT true

// Battery checker
#define BATTERY true
#define BATTERY_PIN A3
#define BATTERY_LOW_LED 30
#define BATTERY_TIMER 3000
#define BATTERY_MIN (long)3000
#define BATTERY_MAX (long)4250

// Sensor & Clock Timers
#define SENSOR_TIMER 28000
#define CLOCK_TIMER 500
#define PHOTO_TIMER 2500
#define SWITCH_TIMER 15000
#define HOUR_TIMER (long)4 * 60 * 1000
#define DAY_TIMER (long)1.6 * 60 * 60 * 1000
#define PREDICT_TIMER (long)10 * 60 * 1000

// Photoresistor
#define PHOTO_PIN A1
#define PHOTO_MIN 30
#define PHOTO_MAX 800
#define LCD_BRI_MIN 10
#define LCD_BRI_MAX 255

// LCD
#define LCD_WIDTH 16
#define LCD_HEIGHT 2
#define LCD_RS_PIN 9
#define LCD_E_PIN 8
#define LCD_D4_PIN A0
#define LCD_D5_PIN 13
#define LCD_D6_PIN 12
#define LCD_D7_PIN 11
#define LCD_BRI_PIN 10

// BME
#define BME_ADDRESS 0x76
#define SEALEVELPRESSURE_HPA (1013.25)


// CO2
#define CO2 true
#define CO2_LED_TIMER 2000
#define CO2_MIDDLE 800
#define CO2_MAX 1200
#define CO2_RX 3
#define CO2_TX 4
