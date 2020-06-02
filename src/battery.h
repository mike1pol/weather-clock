#include <Arduino.h>
#include <LiquidCrystal.h>
#include <config.h>

class Battery
{
public:
  void setup();
  int tick();
  void draw();

private:
  float current;
  int pr;
};