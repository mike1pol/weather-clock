#include <Adafruit_BME280.h>
#include <LiquidCrystal.h>

class Sensor
{
    public:
        void setup();
        void tick();
        void draw();
        void saveHour();
        void saveDay();
        void predict();
    private:
        int dispTemp;
        int dispHum;
        int dispPressure;
        int dispRain;
        int delta;
        byte time_array[6];
        uint32_t pressure_array[6];
        int tempHour[15], tempDay[15];
        int humHour[15], humDay[15];
        int pressHour[15], pressDay[15];
};