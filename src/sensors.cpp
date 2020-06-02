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
  saveHour();
}

void Sensor::tick()
{
  bme.takeForcedMeasurement();
  dispTemp = (float)bme.readTemperature();
  dispHum = (int)bme.readHumidity();
  dispPressure = (float)bme.readPressure() * 0.00750062;
};

void Sensor::saveHour()
{
  for (byte i = 0; i < 14; i++)
  {
    int next = i + 1;
    tempDay[i] = tempDay[next];
    humDay[i] = humDay[next];
    pressHour[i] = pressHour[next];
  }
  tempDay[14] = dispTemp;
  humDay[14] = dispHum;
  pressHour[14] = dispPressure;
}

void Sensor::saveDay()
{
  long averTemp = 0, averHum = 0, averPress = 0;
  for (byte i = 0; i < 15; i++)
  {
    averTemp += tempDay[i];
    averHum += humDay[i];
    averPress += pressHour[i];
  }
  averTemp /= 15;
  averHum /= 15;
  averPress /= 15;
  for (byte i = 0; i < 14; i++)
  {
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
  for (int i = 0; i < 6; i++)
  {
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
  int key = 13;
  int down = 2;
  int up = 3;
  char text[14];
  // Temp
  lcd.setCursor(0, 0);
  if (tempDay[key] > 0)
  {
    if (dispHum > tempDay[key])
      lcd.write(up);
    else if (dispHum < tempDay[key])
      lcd.write(down);

    if (dispHum != tempDay[key])
      lcd.setCursor(1, 0);
  }
  sprintf(text, "%d%cC", dispTemp, 223);
  lcd.print(text);

  // Hum
  if (humDay[key] > 0)
  {
    lcd.setCursor(dispHum >= 100 ? 11 : 12, 0);
    if (dispHum > humDay[key])
      lcd.write(up);
    else if (dispHum < humDay[key])
      lcd.write(down);
  }
  lcd.setCursor(dispHum >= 100 ? 12 : 13, 0);
  sprintf(text, "%d%%", dispHum);
  lcd.print(text);

  // Press
  lcd.setCursor(0, 1);
  if (pressDay[key] > 0)
  {
    if (dispPressure > pressDay[key])
      lcd.write(up);
    else if (dispPressure < pressDay[key])
      lcd.write(down);

    if (dispPressure != pressDay[key])
      lcd.setCursor(1, 1);
  }
  sprintf(text, "%dmm", dispPressure);
  lcd.print(text);

  // rain
  sprintf(text, "%d%%", dispRain);
  lcd.setCursor(16 - strlen(text) - 1, 1);
  lcd.print(text);
};

void Sensor::drawPlot(plotType plot)
{
  int maxValue = -32000;
  int minValue = 32000;
  char textMin[14];
  char textMax[14];
  int *plotVal;
  switch (plot)
  {
  case TEMP_HOUR_PLOT:
    plotVal = tempDay;
    break;
  case TEMP_DAY_PLOT:
    plotVal = tempDay;
    break;
  case HUM_HOUR_PLOT:
    plotVal = humDay;
    break;
  case HUM_DAY_PLOT:
    plotVal = humDay;
    break;
  case PRESS_HOUR_PLOT:
    plotVal = pressHour;
    break;
  case PRESS_DAY_PLOT:
    plotVal = pressDay;
    break;
  }

  for (byte i = 0; i <= 14; i++)
  {
    if (plotVal[i] > maxValue)
      maxValue = plotVal[i];
    if (plotVal[i] < minValue)
      minValue = plotVal[i];
  }
  switch (plot)
  {
  case TEMP_HOUR_PLOT:
  case TEMP_DAY_PLOT:
    sprintf(textMin, "%d%cC", minValue, 223);
    sprintf(textMax, "%d%cC", maxValue, 223);
    break;
  case HUM_HOUR_PLOT:
  case HUM_DAY_PLOT:
    sprintf(textMin, "%d%%", minValue);
    sprintf(textMax, "%d%%", maxValue);
    break;
  case PRESS_HOUR_PLOT:
  case PRESS_DAY_PLOT:
    sprintf(textMin, "%dmm", minValue);
    sprintf(textMax, "%dmm", maxValue);
    break;
  }
  int row = 0;
  int width = strlen(textMin) > strlen(textMax) ? strlen(textMin) : strlen(textMax);
  width = width + 1;
  int height = 2;
  lcd.setCursor(16 - strlen(textMin), 0);
  lcd.print(textMin);
  lcd.setCursor(16 - strlen(textMax), 1);
  lcd.print(textMax);
  for (byte i = width; i > 0; i--)
  {
    byte infill, fract;
    if (plotVal[i] > minValue)
      infill = floor((float)(plotVal[i] - minValue) / (maxValue - minValue) * height * 10);
    else
      infill = 0;
    fract = (float)(infill % 10) * 8 / 10;
    infill = infill / 10;
    Serial.print("fract: ");
    Serial.print(fract);
    Serial.print(" infill: ");
    Serial.println(infill);

    for (byte n = 0; n < height; n++)
    {
      if (n < infill && infill > 0)
      {
        lcd.setCursor(i, (row - n));
        if (fract > 0)
          lcd.write(fract);
        else
          lcd.write(16);
        for (byte k = n + 1; k < height; k++)
        {
          lcd.setCursor(i, (row - k));
          lcd.write(16);
        }
      }
    }
  }
}

// h - max = 8 / min = 1