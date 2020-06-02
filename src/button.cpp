#include <button.h>

extern GButton btn;
extern byte mode;
extern GTimer_ms switchStatus;
extern void switchMode();

void Button::tick()
{
    btn.tick();
    if (btn.isSingle())
    {
        switchStatus.stop();
        if (mode == 1)
            mode = 3;
        else if (mode == 5)
        {
            switchStatus.reset();
            switchStatus.start();
            mode = 0;
        }
        else
            mode += 1;
        switchMode();
    }
    else if (btn.isDouble())
    {
        switchStatus.reset();
        switchStatus.start();
        mode = 0;
        switchMode();
    }
}

void Button::interrupt()
{
    btn.tick();
}