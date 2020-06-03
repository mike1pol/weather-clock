#include <LiquidCrystal.h>
#include <RTClib.h>

class Clock {
public:
    void setup();

    void tick();

    void draw();

private:
    DateTime now;
    int8_t hours, minutes, secs;
    boolean dotFlag;

    void dig(byte dig, byte x, byte y);

    void digSeg(byte x, byte y, byte z1, byte z2, byte z3, byte z4, byte z5, byte z6);

    void dots(byte x, byte y);
};