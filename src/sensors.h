#include <Adafruit_BME280.h>
#include <MHZ19_uart.h>
#include <LiquidCrystal.h>
#include <config.h>

enum plotType {
  TEMP_HOUR_PLOT,
  TEMP_DAY_PLOT,
  HUM_HOUR_PLOT,
  HUM_DAY_PLOT,
  PRESS_HOUR_PLOT,
  PRESS_DAY_PLOT,
  CO2_HOUR_PLOT,
  CO2_DAY_PLOT
};

class Sensor {
public:
  void setup();

  void tick();

  void draw() const;

  void drawPlot();

  void saveHour();

  void saveDay();

  void predict();

  void drawPredict() const;

  boolean co2State(int max) const;

private:
  int temp;
  int hum;
  int pressure;
  int rain;
  int co2;
  int delta;
  byte time_array[6];
  uint32_t pressure_array[6];
  int tempHour[15], tempDay[15];
  int humHour[15], humDay[15];
  int pressHour[15], pressDay[15];
  int co2Hour[15], co2Day[15];

  void drawPlot(plotType plot);
};