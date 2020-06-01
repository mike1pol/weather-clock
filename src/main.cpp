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

#include <GyverTimer.h>
GTimer_ms clockTimer(CLOCK_TIMER);
GTimer_ms sensorTimer(SENSOR_TIMER);
GTimer_ms switchStatus(SWITCH_TIMER);

boolean status;
byte mode = 0;

void setup()
{
#if DEBUG
  Serial.begin(9600);
  Serial.println(F("Initialization..."));
#endif
  lcd.begin(16, 2);
  lcd.backlight();
  delay(200);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Loading..."));
  delay(1000);
  bool rtcStatus = rtc.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("RTC..."));
#if DEBUG
  Serial.print(F("RTC..."));
  if (rtcStatus)
    Serial.println(F("OK"));
  else
    Serial.println(F("Error"));
#endif

  delay(100);
  if (rtcStatus)
    lcd.print(F("OK"));
  else
  {
    status = false;
    lcd.print(F("Error"));
  }
  delay(1000);

  bool bmeStatus = bme.begin(BME_ADDRESS, &Wire);
  lcd.setCursor(0, 1);
  lcd.print(F("BME280..."));
#if DEBUG
  Serial.print(F("BME280..."));
  if (bmeStatus)
    Serial.println(F("OK"));
  else
    Serial.println(F("Error"));
#endif
  if (bmeStatus)
    lcd.print(F("OK"));
  else
  {
    status = false;
    lcd.print(F("Error"));
  }

  delay(1000);
  if (RESET_CLOCK)
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

#if DEBUG
  if (status)
    Serial.println("Startup errros");
  else
    Serial.println("Ready");
#endif
  if (status)
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
    lcd.setCursor(0, 0);
    lcd.print("All sensors OK");
    clock.setup();
    sensor.tick();
    delay(1000);
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1, // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1, // humidity
                    Adafruit_BME280::FILTER_OFF);
  }
}
void loop()
{
  if (clockTimer.isReady())
    clock.tick();
  if (sensorTimer.isReady())
    sensor.tick();
  if (switchStatus.isReady())
  {
    mode++;
    if (mode > 1)
    {
      mode = 0;
    }
    lcd.clear();
    switch (mode)
    {
    case 0:
      clock.draw(0, 0);
      break;
    case 1:
      sensor.draw();
      break;
    }
  }
}