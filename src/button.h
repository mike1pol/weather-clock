#include <GyverButton.h>
#include <GyverTimer.h>

class Button {
public:
  void tick();

private:
  boolean isClicked;
};