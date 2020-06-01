#include <sensors.h>

extern Adafruit_BME280 bme;
extern LiquidCrystal lcd;
extern byte mode;

void Sensor::setup()
{
    bme.takeForcedMeasurement();
    uint32_t Pressure = bme.readPressure();
    for (byte i = 0; i < 6; i++)
    {
        pressure_array[i] = Pressure;
        time_array[i] = i;
    }
    tick();
    predict();
}

void Sensor::tick()
{
    bme.takeForcedMeasurement();
    dispTemp = (float)bme.readTemperature();
    dispHum = (int)bme.readHumidity();
    dispPressure = (float)bme.readPressure() * 0.00750062;
};

void Sensor::saveHour() {
    for (byte i = 0; i < 14; i++) {
        int next = i + 1;
        tempHour[i] = tempHour[next];
        humHour[i] = humHour[next];
        pressHour[i] = pressHour[next];
    }
    tempHour[14] = dispTemp;
    humHour[14] = dispHum;
    pressHour[14] = dispPressure;
}

void Sensor::saveDay() {
    long averTemp = 0, averHum = 0, averPress = 0;
    for (byte i = 0; i < 14; i++) {
        averTemp += tempHour[i];
        averHum += humHour[i];
        averPress += pressHour[i];
    }
    averTemp /= 15;
    averHum /= 15;
    averPress /= 15;
    for (byte i = 0; i < 14; i++) {
      tempDay[i] = tempDay[i + 1];
      humDay[i] = humDay[i + 1];
      pressDay[i] = pressDay[i + 1];
    }
    tempDay[14] = averTemp;
    humDay[14] = averHum;
    pressDay[14] = averPress;
}

void Sensor::predict()
{
    long averPress = 0;
    for (byte i = 0; i < 10; i++)
    {
        bme.takeForcedMeasurement();
        averPress += bme.readPressure();
        delay(1);
    }
    averPress /= 10;
    for (byte i = 0; i < 5; i++)
    {
        pressure_array[i] = pressure_array[i + 1];
    }
    pressure_array[5] = averPress;
    int sumX = 0;
    int sumY = 0;
    int sumX2 = 0;
    int sumXY = 0;
    for (int i = 0; i < 6; i++) {
      sumX += time_array[i];
      sumY += (long)pressure_array[i];
      sumX2 += time_array[i] * time_array[i];
      sumXY += (long)time_array[i] * pressure_array[i];
    }
    int a = 0;
    a = (long)6 * sumXY;
    a = a - (long)sumX * sumY;
    a = (float)a / (6 * sumX2 - sumX * sumX);
    delta = a * 6;
    dispRain = map(delta, -250, 250, 100, -100);
}

void Sensor::draw()
{
    lcd.setCursor(0, 0);
    char text[14];
    sprintf(text, "%d%cC", dispTemp, 223);
    lcd.print(text);
    lcd.setCursor(dispHum >= 100 ? 12 : 13, 0);
    sprintf(text, "%d%%", dispHum);
    lcd.print(text);
    sprintf(text, "%d mm", dispPressure);
    lcd.setCursor(0, 1);
    lcd.print(text);
    sprintf(text, "%d%%", dispRain);
    lcd.setCursor(16 - strlen(text), 1);
    lcd.print(text);

};