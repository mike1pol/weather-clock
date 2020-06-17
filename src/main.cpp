#include <Arduino.h>
#include <Wire.h>

#include <config.h>

#include <LiquidCrystal.h>

LiquidCrystal lcd(LCD_RS_PIN, LCD_E_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

#include <RTClib.h>
#include <clock.h>

RTC_DS1307 rtc;
Clock clock;

#include <Adafruit_BME280.h>
#include <sensors.h>

Adafruit_BME280 bme;
Sensor sensor;

#if BATTERY

#include <app_battery.h>

AppBattery battery;
#endif

#if CO2

#include <MHZ19_uart.h>

MHZ19_uart mhz19;

#endif

#include <GyverTimer.h>

GTimer_ms clockTimer(CLOCK_TIMER);
GTimer_ms photoTimer(PHOTO_TIMER);
GTimer_ms sensorTimer(SENSOR_TIMER);
GTimer_ms switchStatus(SWITCH_TIMER);
GTimer_ms hourPlotTimer(HOUR_TIMER);
GTimer_ms dayPlotTimer(DAY_TIMER);
GTimer_ms predictTimer(PREDICT_TIMER);
#if BATTERY
GTimer_ms batteryTimer(BATTERY_TIMER);
#endif
#if CO2
GTimer_ms co2LedTimer(CO2_LED_TIMER);
#endif

#include <GyverButton.h>
#include <button.h>

GButton btn(BUTTON_PIN, LOW_PULL, NORM_OPEN);
Button button;

boolean startupError;
int mode = 0;
boolean sMode = false;
int ledState = 0;

void switchMode();

void setupLCDClock();

void setupLCDInfo();

void setupLCDPlot();

void buttonInterrupt() {
  button.tick();
}

void setup() {
#if DEBUG
  Serial.begin(9600);
  Serial.println("Initialization...");
#endif
  analogWrite(LCD_BRI_PIN, LCD_BRI_MAX);
  lcd.begin(16, 2);
  lcd.backlight();
  setupLCDClock();
  delay(200);
  digitalWrite(LED_PIN, 1);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Loading...");
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
  lcd.print("RTC..");
  Serial.print("RTC..");
  if (rtcStatus)
    Serial.println("OK");
  else
    Serial.println("E");

  delay(100);
  if (rtcStatus)
    lcd.print("OK");
  else {
    startupError = true;
    lcd.print("E");
  }
  delay(500);
#else
  if (!rtcStatus)
    startupError = true;
#endif

  bool bmeStatus = bme.begin(BME_ADDRESS, &Wire);
#if DEBUG
#if CO2
  lcd.setCursor(9, 0);
#else
  lcd.setCursor(0, 1);
#endif
  lcd.print("BME..");
  Serial.print("BME..");
  if (bmeStatus) {
    Serial.println("OK");
    lcd.print("OK");
  } else {
    startupError = true;
    Serial.println("E");
    lcd.print("E");
  }
  delay(500);
#else
  if (!bmeStatus)
    startupError = true;
#endif

#if CO2
  mhz19.begin(CO2_TX, CO2_RX);
  mhz19.setAutoCalibration(false);
  mhz19.getStatus();
#if DEBUG
  lcd.setCursor(0, 1);
  lcd.print("CO2..");
  Serial.print("CO2..");
  delay(500);
  int co2Status = mhz19.getStatus();
  if (co2Status == 0) {
    lcd.print("OK");
    Serial.println("OK");
  } else {
    startupError = true;
    Serial.println("E");
    lcd.print("E");
  }
  delay(1000);
#else
  delay(500);
  int co2Status = mhz19.getStatus();
  if (co2Status != 0)
    startupError = true;
#endif
#endif

#if DEBUG
  if (startupError)
    Serial.println("Startup errors");
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
#ifdef BATTERY
    battery.setup();
#endif
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1, // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1, // humidity
                    Adafruit_BME280::FILTER_OFF);
  }
}

void loop() {
  button.tick();
#if CO2
  if (co2LedTimer.isReady() && !battery.isLow()) {
    if (sensor.co2State(CO2_MAX))
      ledState = 1;
    else if (sensor.co2State(CO2_MIDDLE))
      ledState = ledState == 1 ? 0 : 1;
    else
      ledState = 0;
  }
#endif
#if BATTERY
  if (batteryTimer.isReady()) {
    battery.tick();
    if (battery.isLow())
      ledState = 1;
    if (mode == 1)
      battery.draw();
  }
#endif
  digitalWrite(LED_PIN, ledState);
  if (clockTimer.isReady())
    clock.tick();
  if (photoTimer.isReady()) {
    int bri = constrain(analogRead(PHOTO_PIN), PHOTO_MIN, PHOTO_MAX);
    analogWrite(LCD_BRI_PIN, (int) map(bri, PHOTO_MIN, PHOTO_MAX, LCD_BRI_MIN, LCD_BRI_MAX));
  }
  if (sensorTimer.isReady()) {
    sensor.tick();
    if (mode == 1)
      sensor.draw();
  }
  if (predictTimer.isReady()) {
    sensor.predict();
    if (mode == 1)
      sensor.drawPredict();
  }
  if (hourPlotTimer.isReady()) {
    sensor.saveHour();
    if (mode == 3 || mode == 4 || mode == 5) {
      sensor.drawPlot();
    }
  }
  if (dayPlotTimer.isReady()) {
    sensor.saveDay();
    if (mode == 31 || mode == 41 || mode == 51) {
      sensor.drawPlot();
    }
  }
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
      clock.draw();
      sensor.draw();
      sensor.drawPredict();
#ifdef BATTERY
      battery.draw();
#endif
      break;
    case 2:
      mode = 0;
      switchMode();
      break;
    case 3:
    case 4:
    case 5:
    default:
      setupLCDPlot();
      lcd.clear();
      sensor.drawPlot();
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
uint8_t PR[8] = {0b00100, 0b00100, 0b11111, 0b01110, 0b00100, 0b00000, 0b11011, 0b00100};
uint8_t CO[8] = {0b00001, 0b11111, 0b10001, 0b11111, 0b00000, 0b10001, 0b10001, 0b11111};

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
  lcd.createChar(4, PR);
  lcd.createChar(5, CO);
}

void setupLCDPlot() {
  lcd.createChar(1, row1);
  lcd.createChar(2, row2);
  lcd.createChar(3, row3);
  lcd.createChar(4, row4);
  lcd.createChar(5, row5);
  lcd.createChar(6, row6);
  lcd.createChar(7, row7);
  lcd.createChar(8, CO);
}