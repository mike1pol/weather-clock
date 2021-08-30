#define DEBUG true
#define RESET_CLOCK false
#define LED_PIN PD7

// Button
#define BUTTON_PIN PD4
#define BUTTON_INTERRUPT false

// Battery checker
#define BATTERY false
#define BATTERY_PIN A3
#define BATTERY_LOW_LED 30
#define BATTERY_TIMER 3000
#define BATTERY_MIN (long)3000
#define BATTERY_MAX (long)4250

// Sensor & Clock Timers
#define SENSOR_TIMER 28000
#define CLOCK_TIMER 500
#define CLOCK_REDRAW_SCREEN_ONE_TIMER (long) 60 * 1000
#define PHOTO_TIMER 2500
#define HOUR_TIMER (long)4 * 60 * 1000
#define DAY_TIMER (long)1.6 * 60 * 60 * 1000
#define PREDICT_TIMER (long)10 * 60 * 1000

// Photoresistor
#define PHOTO false
#define PHOTO_PIN A1
#define PHOTO_MIN 30
#define PHOTO_MAX 800
#define LCD_BRI_MIN 10
#define LCD_BRI_MAX 255

// LCD
#define LCD_ADDR 0x27 // адрес платы дисплея: 0x27 или 0x3f. Если дисплей не работает - смени адрес! На самом дисплее адрес не указан
#define LCD_BRI_PIN 10
#define LCD_WIDTH 20
#define LCD_HEIGHT 4

// BME
#define BME_ADDRESS 0x76
#define SEALEVELPRESSURE_HPA (1013.25)


// CO2
#define CO2 true
#define CO2_LED_TIMER 2000
#define CO2_MIDDLE 800
#define CO2_MAX 1200
#define CO2_RX 2
#define CO2_TX 3
