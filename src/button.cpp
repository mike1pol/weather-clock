#include <button.h>

extern GButton btn;
extern int mode;
extern boolean sMode;
//extern GTimer_ms switchStatus;

extern void switchMode();

void Button::tick() {
  btn.tick();
  if (btn.isSingle() && !btn.isHolded() && !btn.isHold()) {
    if (!isClicked) {
      isClicked = true;
//      switchStatus.stop();
    }
    if (mode == 1)
      mode = 3;
    else if (mode == end)
      mode = 0;
    else
      mode += 1;
    if (sMode)
      sMode = !sMode;
    switchMode();
  } else if (btn.isDouble()) {
    if (!isClicked) {
      isClicked = true;
//      switchStatus.stop();
    }
    if (mode == 3)
      mode = 1;
    else if (mode == 0)
      mode = end;
    else
      mode -= 1;
    if (sMode)
      sMode = !sMode;
    switchMode();
  } else if (btn.isHolded()) {
    if (btn.getHoldClicks() == 1 && mode > 2) {
      sMode = !sMode;
      switchMode();
      return;
    }
//    switchStatus.reset();
//    switchStatus.start();
    mode = 0;
    if (isClicked)
      switchMode();
    isClicked = false;
  }
}

