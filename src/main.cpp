#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <config.h>

#include <LiquidCrystal.h>

LiquidCrystal lcd(LCD_RS_PIN, LCD_E_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

#include <RTClib.h>
#include <clock.h>

RTC_DS1307 rtc;
Clock clock;

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <sensors.h>

Adafruit_BME280 bme;
Sensor sensor;

#include <battery.h>

Battery battery;

#include <GyverTimer.h>

GTimer_ms clockTimer(CLOCK_TIMER);
GTimer_ms photoTimer(PHOTO_TIMER);
GTimer_ms sensorTimer(SENSOR_TIMER);
GTimer_ms switchStatus(SWITCH_TIMER);
GTimer_ms hourPlotTimer(HOUR_TIMER);
GTimer_ms dayPlotTimer(DAY_TIMER);
GTimer_ms predictTimer(PREDICT_TIMER);
GTimer_ms batteryTimer(BATTERY_TIMER);

#include <GyverButton.h>
#include <button.h>

GButton btn(BUTTON_PIN, LOW_PULL, NORM_OPEN);
Button button;

boolean startupError;
byte mode = 0;

void switchMode();

void setupLCDClock();

void setupLCDInfo();

void setupLCDPlot();

void buttonInterrupt() {
    button.interrupt();
}

void setup() {
// #if DEBUG || BATTERY_CALIBRATION
    Serial.begin(9600);
    Serial.println(F("Initialization..."));
// #endif
#if BATTERY_CALIBRATION
    battery.callibration();
#endif
    pinMode(BATTERY_PIN, INPUT);
    analogWrite(LCD_BRI_PIN, LCD_BRI_MAX);
    lcd.begin(16, 2);
    lcd.backlight();
    setupLCDClock();
    delay(200);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Loading..."));
    delay(500);
#if BUTTON_INTERRUPT
    attachInterrupt(BUTTON_PIN - 1, buttonInterrupt, CHANGE);
#endif
    bool rtcStatus = rtc.begin();
    if (RESET_CLOCK)
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
#if DEBUG
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("RTC..."));
    Serial.print(F("RTC..."));
    if (rtcStatus)
      Serial.println(F("OK"));
    else
      Serial.println(F("Error"));

    delay(100);
    if (rtcStatus)
      lcd.print(F("OK"));
    else
    {
      startupError = false;
      lcd.print(F("Error"));
    }
    delay(500);
#else
    if (!rtcStatus)
        startupError = false;
#endif

    bool bmeStatus = bme.begin(BME_ADDRESS, &Wire);
#if DEBUG
    lcd.setCursor(0, 1);
    lcd.print(F("BME280..."));
    Serial.print(F("BME280..."));
    if (bmeStatus)
      Serial.println(F("OK"));
    else
      Serial.println(F("Error"));
    if (bmeStatus)
      lcd.print(F("OK"));
    else
    {
      startupError = false;
      lcd.print(F("Error"));
    }
    delay(1000);
#else
    if (!bmeStatus)
        startupError = false;
#endif

#if DEBUG
    if (startupError)
      Serial.println("Startup errros");
    else
      Serial.println("Ready");
#endif
    if (startupError) {
        lcd.clear();
        while (true) {
            lcd.setCursor(0, 0);
            lcd.print("Started with errors");
            lcd.setCursor(0, 1);
            lcd.print("Try to restart");
            delay(1000);
        }
    } else {
        lcd.clear();
        clock.setup();
        sensor.setup();
        battery.setup();
        bme.setSampling(Adafruit_BME280::MODE_FORCED,
                        Adafruit_BME280::SAMPLING_X1, // temperature
                        Adafruit_BME280::SAMPLING_X1, // pressure
                        Adafruit_BME280::SAMPLING_X1, // humidity
                        Adafruit_BME280::FILTER_OFF);
    }
}

void loop() {
    button.tick();
    if (batteryTimer.isReady())
        battery.tick();
    if (clockTimer.isReady())
        clock.tick();
    if (photoTimer.isReady()) {
        int bri = constrain(analogRead(PHOTO_PIN), PHOTO_MIN, PHOTO_MAX);
#if DEBUG
        Serial.print(bri);
        Serial.print(F(" - "));
        Serial.println(map(bri, PHOTO_MIN, PHOTO_MAX, LCD_BRI_MIN, LCD_BRI_MAX));
#endif
        analogWrite(LCD_BRI_PIN, map(bri, PHOTO_MIN, PHOTO_MAX, LCD_BRI_MIN, LCD_BRI_MAX));
    }
    if (sensorTimer.isReady())
        sensor.tick();
    if (hourPlotTimer.isReady())
        sensor.saveHour();
    if (dayPlotTimer.isReady())
        sensor.saveDay();
    if (predictTimer.isReady())
        sensor.predict();
    if (switchStatus.isReady()) {
        mode++;
        switchMode();
    }
}

void switchMode() {
    switch (mode) {
        case 0:
            setupLCDClock();
            lcd.clear();
            clock.draw();
            break;
        case 1:
            setupLCDInfo();
            lcd.clear();
            sensor.draw();
            clock.draw();
            battery.draw();
            break;
        case 2:
            mode = 0;
            switchMode();
            break;
        case 3:
            setupLCDPlot();
            lcd.clear();
            sensor.drawPlot(TEMP_HOUR_PLOT);
            break;
        case 4:
            setupLCDPlot();
            lcd.clear();
            sensor.drawPlot(HUM_HOUR_PLOT);
            break;
        case 5:
            setupLCDPlot();
            lcd.clear();
            sensor.drawPlot(PRESS_HOUR_PLOT);
            break;
    }
}

// 4 display
byte rowD[8] = {0b00000, 0b00000, 0b00000, 0b10001, 0b11011, 0b01110, 0b00100, 0b00000};
byte rowU[8] = {0b00000, 0b00100, 0b01110, 0b11011, 0b10001, 0b00000, 0b00000, 0b00000};
byte row7[8] = {0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111};
byte row6[8] = {0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111};
byte row5[8] = {0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111};
byte row4[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111};
byte row3[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111};
byte row2[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111};
byte row1[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111};
uint8_t UB[8] = {0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000};
uint8_t UMB[8] = {0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111};
uint8_t LMB[8] = {0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111};
uint8_t LM2[8] = {0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000};

// Power icons
// AC
uint8_t AC[8] = {0b01010, 0b01010, 0b11111, 0b11111, 0b01110, 0b00100, 0b00100, 0b00011};

void setupLCDClock() {
    lcd.createChar(0, row2);
    lcd.createChar(1, UB);
    lcd.createChar(2, row3);
    lcd.createChar(3, UMB);
    lcd.createChar(4, LMB);
    lcd.createChar(5, LM2);
    lcd.createChar(6, rowD);
    lcd.createChar(7, rowU);
}

void setupLCDInfo() {
    lcd.createChar(0, AC);
    lcd.createChar(2, rowD);
    lcd.createChar(3, rowU);
}

void setupLCDPlot() {
    lcd.createChar(1, row1);
    lcd.createChar(2, row2);
    lcd.createChar(3, row3);
    lcd.createChar(4, row4);
    lcd.createChar(5, row5);
    lcd.createChar(6, row6);
    lcd.createChar(7, row7);
}