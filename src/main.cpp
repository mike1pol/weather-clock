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
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;
Sensor sensor;

#include <GyverTimer.h>
GTimer_ms clockTimer(CLOCK_TIMER);
GTimer_ms photoTimer(PHOTO_TIMER);
GTimer_ms sensorTimer(SENSOR_TIMER);
GTimer_ms switchStatus(SWITCH_TIMER);
GTimer_ms hourPlotTimer(HOUR_TIMER);
GTimer_ms dayPlotTimer(DAY_TIMER);
GTimer_ms predictTimer(PREDICT_TIMER);

boolean startupError;
byte mode = 0;

void setup()
{
#if DEBUG
  Serial.begin(9600);
  Serial.println(F("Initialization..."));
#endif
  analogWrite(LCD_BRI_PIN, LCD_BRI_MAX);
  lcd.begin(16, 2);
  lcd.backlight();
  delay(200);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Loading..."));
  delay(500);
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
  if (startupError)
  {
    lcd.clear();
    while (1)
    {
      lcd.setCursor(0, 0);
      lcd.print(F("Started with errors"));
      lcd.setCursor(0, 1);
      lcd.print(F("Try to restart"));
      delay(1000);
    }
  }
  else
  {
    lcd.clear();
    clock.setup();
    sensor.setup();
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1, // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1, // humidity
                    Adafruit_BME280::FILTER_OFF);
  }
}
void switchMode();
void loop()
{
  if (clockTimer.isReady())
    clock.tick();
  if (photoTimer.isReady())
  {
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
  if (switchStatus.isReady())
  {
    mode++;
    switchMode();
  }
}

void switchMode()
{
  switch (mode)
  {
  case 0:
    lcd.clear();
    clock.draw();
    break;
  case 1:
    lcd.clear();
    sensor.draw();
    clock.draw();
    break;
  case 3:
    mode = 2;
    switchMode();
    break;
  }
}