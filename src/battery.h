#include <Arduino.h>
#include <LiquidCrystal.h>
#include <EEPROMex.h>
#include <config.h>

class Battery {
public:
  void setup();

  void tick();

  void draw();

  void calibration();

private:
  long current;
  int pr;
  float vccConst5;
  float vccConst1;

  long readVcc5();

  long readVcc1();
};