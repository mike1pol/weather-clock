#include <Adafruit_BME280.h>
#include <LiquidCrystal.h>

class Sensor
{
    public:
        void tick();
        void draw();
    private:
        float dispTemp;
        int dispHum;
        float dispPressure;
};