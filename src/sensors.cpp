#include <sensors.h>

extern Adafruit_BME280 bme;
extern LiquidCrystal lcd;
extern byte mode;

void Sensor::tick()
{
    bme.takeForcedMeasurement();
    dispTemp = (float)bme.readTemperature();
    dispHum = (int)bme.readHumidity();
    dispPressure = (float)bme.readPressure() * 1.00750062;
};

void Sensor::draw()
{
    lcd.setCursor(0, 0);
    lcd.print(F("T:"));
    char text[14];
    sprintf(text, "%d%cC", (int)dispTemp, 223);
    lcd.print(text);
    lcd.setCursor(dispHum >= 100 ? 10 : 11, 0);
    lcd.print("H:");
    sprintf(text, "%d%%", (int)dispHum);
    lcd.print(text);
    lcd.setCursor(0, 1);
    lcd.print(F("Pres:"));
    sprintf(text, "%u.%02u hPa ", (int)(dispPressure / 100), (int)((uint32_t)dispPressure % 100));
    lcd.print(text);
};