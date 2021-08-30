#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include "config.h"
#include "dig.h"

class Clock {
public:
  void setup();

  void tick();

  void drawScreenOne() const;

  void drawScreenTwo() const;

private:
  DateTime now;
  uint8_t hours, minutes, secs;
  boolean dotFlag;

  void dots() const;
};