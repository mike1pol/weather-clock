#include <LiquidCrystal.h>
#include <RTClib.h>

class Clock
{
public:
    void setup();
    void tick();

private:
    DateTime now;
    int8_t hours, minutes, secs;
    boolean dotFlag;
    void draw(byte x, byte y);
    void dig(byte dig, byte x, byte y);
    void dots(byte x, byte y);
};