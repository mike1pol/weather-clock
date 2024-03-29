#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROMex.h>
#include <Battery.h>
#include <config.h>

class AppBattery {
public:
  void setup();

  void tick();

  void draw() const;

  boolean isLow() const;

private:
  long current;
  int pr;
};