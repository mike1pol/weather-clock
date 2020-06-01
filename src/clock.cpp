#include <clock.h>

extern LiquidCrystal lcd;
extern RTC_DS1307 rtc;

void Clock::setup()
{
    now = rtc.now();
    secs = now.second();
    minutes = now.minute();
    hours = now.hour();
}

void Clock::tick()
{
    dotFlag = !dotFlag;
    if (dotFlag)
    {
        secs++;
        if (secs > 59)
        {
            secs = 0;
            minutes++;
            if (minutes < 60)
                draw(0, 0);
        }
        if (minutes == 60)
        {
            now = rtc.now();
            secs = now.second();
            minutes = now.minute();
            hours = now.hour();
            draw(0, 0);
            if (hours > 23)
                hours = 0;
        }
    }
    dots(7, 0);
}

void Clock::draw(byte x, byte y)
{
    lcd.setCursor(x, y);
    lcd.print("               ");
    lcd.setCursor(x, y + 1);
    lcd.print("               ");
    if (hours / 10 == 0)
        dig(10, x, y);
    else
        dig(hours / 10, x, y);
    dig(hours % 10, x + 4, y);
    dig(minutes / 10, x + 8, y);
    dig(minutes % 10, x + 12, y);
};

void Clock::dots(byte x, byte y)
{
    byte code;
    if (dotFlag)
        code = 165;
    else
        code = 32;
    lcd.setCursor(x, y);
    lcd.write(code);
    lcd.setCursor(x, y + 1);
    lcd.write(code);
}

void Clock::dig(byte dig, byte x, byte y)
{
    uint8_t zero = 0;
    switch (dig)
    {
    case 0:
        lcd.setCursor(x, y);
        lcd.write(zero);
        lcd.write(1);
        lcd.write(2);
        lcd.setCursor(x, y + 1);
        lcd.write(3);
        lcd.write(4);
        lcd.write(5);
        break;
    case 1:
        lcd.setCursor(x + 1, y);
        lcd.write(1);
        lcd.write(2);
        lcd.setCursor(x + 2, y + 1);
        lcd.write(5);
        break;
    case 2:
        lcd.setCursor(x, y);
        lcd.write(6);
        lcd.write(6);
        lcd.write(2);
        lcd.setCursor(x, y + 1);
        lcd.write(3);
        lcd.write(7);
        lcd.write(7);
        break;
    case 3:
        lcd.setCursor(x, y);
        lcd.write(6);
        lcd.write(6);
        lcd.write(2);
        lcd.setCursor(x, y + 1);
        lcd.write(7);
        lcd.write(7);
        lcd.write(5);
        break;
    case 4:
        lcd.setCursor(x, y);
        lcd.write(3);
        lcd.write(4);
        lcd.write(2);
        lcd.setCursor(x + 2, y + 1);
        lcd.write(5);
        break;
    case 5:
        lcd.setCursor(x, y);
        lcd.write(zero);
        lcd.write(6);
        lcd.write(6);
        lcd.setCursor(x, y + 1);
        lcd.write(7);
        lcd.write(7);
        lcd.write(5);
        break;
    case 6:
        lcd.setCursor(x, y);
        lcd.write(zero);
        lcd.write(6);
        lcd.write(6);
        lcd.setCursor(x, y + 1);
        lcd.write(3);
        lcd.write(7);
        lcd.write(5);
        break;
    case 7:
        lcd.setCursor(x, y);
        lcd.write(1);
        lcd.write(1);
        lcd.write(2);
        lcd.setCursor(x + 1, y + 1);
        lcd.write(zero);
        break;
    case 8:
        lcd.setCursor(x, y);
        lcd.write(zero);
        lcd.write(6);
        lcd.write(2);
        lcd.setCursor(x, y + 1);
        lcd.write(3);
        lcd.write(7);
        lcd.write(5);
        break;
    case 9:
        lcd.setCursor(x, y);
        lcd.write(zero);
        lcd.write(6);
        lcd.write(2);
        lcd.setCursor(x + 1, y + 1);
        lcd.write(4);
        lcd.write(5);
        break;
    case 10:
        lcd.setCursor(x, y);
        lcd.write(32);
        lcd.write(32);
        lcd.write(32);
        lcd.setCursor(x, y + 1);
        lcd.write(32);
        lcd.write(32);
        lcd.write(32);
        break;
    }
}