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
Adafruit_BME280 bme;

#include <GyverTimer.h>
GTimer_ms clockTimer(CLOCK_TIMER);
GTimer_ms sensorTimer(SENSOR_TIMER);

boolean status;

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
    lcd.setCursor(0, 0);
    lcd.print("Starting clock...");
    clock.setup();
    delay(1000);
    clock.tick();
    lcd.clear();
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
  {
    bme.takeForcedMeasurement();
    Serial.print(F("Temp: "));
    Serial.println(bme.readTemperature());
    Serial.print(F("Humidity: "));
    Serial.println(bme.readHumidity());
    Serial.print(F("Pressure: "));
    Serial.println((float)bme.readPressure() * 0.00750062);
  }
  // int phVal = analogRead(PHOTORESISTOR_PIN);
  // Serial.println(phVal);
  bme.takeForcedMeasurement();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(bme.readTemperature());
  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(bme.readHumidity());
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("hPa: ");
  lcd.print((float)bme.readPressure() * 0.00750062);
  lcd.setCursor(0, 1);
  lcd.print("Alt: ");
  lcd.print(bme.readAltitude(BME_SEALEVELPRESSURE_HPA));
  delay(2000);
}