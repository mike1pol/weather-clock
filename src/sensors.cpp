#include <sensors.h>

extern Adafruit_BME280 bme;
#if CO2
extern MHZ19_uart mhz19;
#endif
extern LiquidCrystal lcd;
extern int mode;
extern boolean sMode;

void Sensor::setup() {
  bme.takeForcedMeasurement();
  uint32_t Pressure = bme.readPressure();
  for (byte i = 0; i < 6; i++) {
    pressure_array[i] = Pressure;
    time_array[i] = i;
  }
  tick();
  predict();
  saveHour();
  saveDay();
}

void Sensor::tick() {
  bme.takeForcedMeasurement();
  dispTemp = (int) bme.readTemperature();
  dispHum = (int) bme.readHumidity();
  dispPressure = (int) (bme.readPressure() * 0.00750062);
#if CO2
  dispCO2 = (int) mhz19.getPPM();
#endif
}

void Sensor::saveHour() {
  for (byte i = 0; i < 14; i++) {
    int next = i + 1;
    tempHour[i] = tempHour[next];
    humHour[i] = humHour[next];
    pressHour[i] = pressHour[next];
    co2Hour[i] = co2Hour[next];
  }
  tempHour[14] = dispTemp;
  humHour[14] = dispHum;
  pressHour[14] = dispPressure;
  co2Hour[14] = dispCO2;
}

void Sensor::saveDay() {
  long averTemp = 0, averHum = 0, averPress = 0, averCO = 0;
  for (byte i = 0; i < 15; i++) {
    averTemp += tempHour[i];
    averHum += humHour[i];
    averPress += pressHour[i];
    averCO += pressHour[i];
  }
  averTemp /= 15;
  averHum /= 15;
  averPress /= 15;
  averCO /= 15;
  for (byte i = 0; i < 14; i++) {
    tempDay[i] = tempDay[i + 1];
    humDay[i] = humDay[i + 1];
    pressDay[i] = pressDay[i + 1];
    co2Day[i] = co2Day[i + 1];
  }
  tempDay[14] = (int) averTemp;
  humDay[14] = (int) averHum;
  pressDay[14] = (int) averPress;
  co2Day[14] = (int) averCO;
}

void Sensor::predict() {
  long averPress = 0;
  for (byte i = 0; i < 10; i++) {
    bme.takeForcedMeasurement();
    averPress += (long) bme.readPressure();
    delay(1);
  }
  averPress /= 10;
  for (byte i = 0; i < 5; i++) {
    pressure_array[i] = pressure_array[i + 1];
  }
  pressure_array[5] = averPress;
  int sumX = 0;
  int sumY = 0;
  int sumX2 = 0;
  int sumXY = 0;
  for (int i = 0; i < 6; i++) {
    sumX += time_array[i];
    sumY += pressure_array[i];
    sumX2 += time_array[i] * time_array[i];
    sumXY += time_array[i] * pressure_array[i];
  }
  int a;
  a = 6 * sumXY;
  a = a - sumX * sumY;
  a = a / (6 * sumX2 - sumX * sumX);
  delta = a * 6;
  dispRain = (int) map(delta, -250, 250, 100, -100);
}

void Sensor::draw() const {
  int key = 13;
  int down = 2;
  int up = 3;
  char text[14];
  // Temp
  lcd.setCursor(0, 0);
  if (tempDay[key] > 0) {
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
  if (humDay[key] > 0) {
    lcd.setCursor(dispHum >= 100 ? 11 : 12, 0);
    if (dispHum > humDay[key])
      lcd.write(up);
    else if (dispHum < humDay[key])
      lcd.write(down);
  }
  lcd.setCursor(dispHum >= 100 ? 12 : 13, 0);
  sprintf(text, "%d%%", dispHum);
  lcd.print(text);
  // Clear second line
  int clearTo = 16;
#if BATTERY
  clearTo -= 1;
#endif
  for (int i = 0; i < clearTo + 1; i++) {
    lcd.setCursor(i, 1);
    lcd.write(16);
  }
  // Press
  lcd.setCursor(0, 1);
  if (pressDay[key] > 0) {
    if (dispPressure > pressDay[key])
      lcd.write(up);
    else if (dispPressure < pressDay[key])
      lcd.write(down);

    if (dispPressure != pressDay[key])
      lcd.setCursor(1, 1);
  }
  sprintf(text, "%d", dispPressure);
  lcd.print(text);
  lcd.setCursor(strlen(text) + 1, 1);
  lcd.write(4);
#if CO2
  int left = strlen(text) + 2;
  int right = dispRain > 10 ? 3 : 2;
#if BATTERY
  right += 1;
#endif
  sprintf(text, "%d", dispCO2);
  int tW = (strlen(text) + 1) / 2;
  int start = floor(((LCD_WIDTH - left - right) / 2) + tW);
  lcd.setCursor(start, 1);
  lcd.print(text);
  int width = strlen(text);
  lcd.setCursor(start + width, 1);
  lcd.write(5);
#endif
}

void Sensor::drawPredict() const {
  char text[14];
  // rain
  sprintf(text, "%d%%", dispRain);
  int rainCol = LCD_WIDTH - strlen(text);
#ifdef BATTERY
  rainCol = rainCol - 1;
#endif
  lcd.setCursor(rainCol, 1);
  lcd.print(text);
}

void Sensor::drawPlot() {
  switch (mode) {
    case 3:
      if (sMode)
            drawPlot(TEMP_DAY_PLOT);
      else
            drawPlot(TEMP_HOUR_PLOT);
      break;
    case 4:
      if (sMode)
        drawPlot(HUM_DAY_PLOT);
      else
        drawPlot(HUM_HOUR_PLOT);
      break;
    case 5:
      if (sMode)
        drawPlot(PRESS_DAY_PLOT);
      else
        drawPlot(PRESS_HOUR_PLOT);
      break;
    case 6:
      if (sMode)
        drawPlot(CO2_DAY_PLOT);
      else
        drawPlot(CO2_HOUR_PLOT);
      break;
  }
}

void Sensor::drawPlot(plotType plot) {
  int maxValue = -32000;
  int minValue = 32000;
  char textMin[14];
  char textMax[14];
  int *plotVal;
  switch (plot) {
    case CO2_HOUR_PLOT:
      plotVal = co2Hour;
      break;
    case CO2_DAY_PLOT:
      plotVal = co2Day;
      break;
    case TEMP_DAY_PLOT:
      plotVal = tempDay;
      break;
    case HUM_HOUR_PLOT:
      plotVal = humHour;
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
    case TEMP_HOUR_PLOT:
      plotVal = tempHour;
      break;
  }

  for (byte i = 0; i <= 14; i++) {
    maxValue = max(maxValue, plotVal[i]);
    minValue = min(minValue, plotVal[i]);
  }
  switch (plot) {
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
    case CO2_HOUR_PLOT:
    case CO2_DAY_PLOT:
      sprintf(textMin, "%d%c", minValue, 8);
      sprintf(textMax, "%d%c", maxValue, 8);
      break;
  }
  lcd.setCursor(16 - strlen(textMin), 0);
  lcd.print(textMin);
  lcd.setCursor(16 - strlen(textMax), 1);
  lcd.print(textMax);
  int row = 1;
  int height = LCD_HEIGHT;
  int strip = max(strlen(textMin), strlen(textMax));
  int width = LCD_WIDTH - strip - 2;
  int start = 14 - width;
  if (maxValue == minValue) maxValue += 1;
  int col = 0;
  for (int i = start; i < 15; i++) {
    byte infill, fract;
    if ((plotVal[i]) > minValue)
      infill = floor((float) (plotVal[i] - minValue) / (maxValue - minValue) * height * 10);
    else infill = 0;
    fract = (float) (infill % 10) * 8 / 10;
    infill = infill / 10;
    for (byte n = 0; n < height; n++) {
      if (n < infill && infill > 0) {
        lcd.setCursor(col, (row - n));
        lcd.write(255);
      }
      if (n >= infill) {
        lcd.setCursor(col, (row - n));
        if (n == 0 && fract == 0)
          fract++;
        if (fract > 0) lcd.write(fract);
        else lcd.write(16);
        for (byte k = n + 1; k < height; k++) {
          lcd.setCursor(col, (row - k));
          lcd.write(16);
        }
        break;
      }
    }
    col += 1;
  }
}

boolean Sensor::co2State(int max) const {
  return dispCO2 >= max;
}
