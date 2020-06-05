#include <GyverButton.h>
#include <GyverTimer.h>
#include <config.h>

class Button {
public:
  void tick();

private:
  boolean isClicked = false;
#if CO2
  int end = 6;
#else
  int end = 5;
#endif
};