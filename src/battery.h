#include <Arduino.h>
#include <LiquidCrystal.h>
#include <EEPROMex.h>
#include <config.h>

class Battery {
public:
  void setup();

  void tick();

  void draw() const;

  void calibration();

  boolean isLow() const;

private:
  long current;
  int pr;
  float vccConst1;

  long readVcc1() const;
};