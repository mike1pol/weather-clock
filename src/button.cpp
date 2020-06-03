#include <button.h>

extern GButton btn;
extern int mode;
extern GTimer_ms switchStatus;

extern void switchMode();

void Button::tick() {
  btn.tick();
  if (btn.isSingle()) {
    if (!isClicked) {
      isClicked = true;
      switchStatus.stop();
      mode++;
    } else if (mode == 1)
      mode = 3;
    else if (mode == 5 || mode == 51)
      mode = 0;
    else if (mode == 31)
      mode = 4;
    else if (mode == 41)
      mode = 5;
    else
      mode += 1;
    switchMode();
  } else if (btn.isDouble()) {
    if (mode == 3 || mode == 31)
      mode = 1;
    else if (mode == 0)
      mode = 5;
    else if (mode == 51)
      mode = 4;
    else if (mode == 41)
      mode = 3;
    else
      mode -= 1;
    switchMode();
  } else if (btn.isHold()) {
    if (mode > 30)
      return;
    if (mode == 3)
      mode = 31;
    else if (mode == 4)
      mode = 41;
    else if (mode == 5)
      mode = 51;
    if (mode > 30)
      switchMode();
  } else if (btn.isTriple()) {
    switchStatus.reset();
    switchStatus.start();
    mode = 0;
    isClicked = false;
    switchMode();
  }
}

