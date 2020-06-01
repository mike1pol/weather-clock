#include <clock.h>

extern LiquidCrystal lcd;
extern RTC_DS1307 rtc;
extern byte mode;

byte row2[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111};   // в т.ч. для двустрочной цифры 4 (с)НР
byte row3[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};   // в т.ч. для двустрочной цифры 0, для четырехстрочных цифр 2, 3, 4, 5, 6, 8, 9 (с)НР
uint8_t UB[8] = {0b11111,  0b11111,  0b11111,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000};   // для двустрочных 7, 0   // для четырехстрочных 2, 3, 4, 5, 6, 8, 9
uint8_t UMB[8] = {0b11111,  0b11111,  0b11111,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111};  // для двустрочных 2, 3, 5, 6, 8, 9
uint8_t LMB[8] = {0b11111,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};  // для двустрочных 2, 3, 5, 6, 8, 9
uint8_t LM2[8] = {0b11111,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000};  // для двустрочной 4

void Clock::setup()
{
    now = rtc.now();
    secs = now.second();
    minutes = now.minute();
    hours = now.hour();
    lcd.createChar(0, row2);
    lcd.createChar(1, UB);
    lcd.createChar(2, row3);
    lcd.createChar(3, UMB);
    lcd.createChar(4, LMB);
    lcd.createChar(5, LM2);
    draw();
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
                draw();
        }
        if (minutes == 60)
        {
            now = rtc.now();
            secs = now.second();
            minutes = now.minute();
            hours = now.hour();
            draw();
            if (hours > 23)
                hours = 0;
        }
    }
    if (mode == 0) {
        dots(7, 0);
    } else if (mode == 1) {
        lcd.setCursor(8, 0);
        lcd.print(dotFlag ? ":" : " ");
    }
}

void Clock::draw()
{
    if (mode == 0)
    {
        byte x = 0;
        byte y = 0;
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
    }
    else if (mode == 1)
    {
        lcd.setCursor(6, 0);
        if (hours < 10)
            lcd.print(" ");
        lcd.print(String(hours));
        lcd.print(dotFlag ? ":" : " ");
        if (minutes < 10)
            lcd.print("0");
        lcd.print(String(minutes));
    }
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

void Clock::digSeg(byte x, byte y, byte z1, byte z2, byte z3, byte z4, byte z5, byte z6)
{
    lcd.setCursor(x, y);
    lcd.write(z1);
    lcd.write(z2);
    lcd.write(z3);
    if (x <= 11)
        lcd.print(" ");
    lcd.setCursor(x, y + 1);
    lcd.write(z4);
    lcd.write(z5);
    lcd.write(z6);
    if (x <= 11)
        lcd.print(" ");
}

void Clock::dig(byte dig, byte x, byte y)
{
    switch (dig)
    {
    case 0:
        digSeg(x, y, 255, 1, 255, 255, 2, 255);
        break;
    case 1:
        digSeg(x, y, 32, 255, 32, 32, 255, 32);
        break;
    case 2:
        digSeg(x, y, 3, 3, 255, 255, 4, 4);
        break;
    case 3:
        digSeg(x, y, 3, 3, 255, 4, 4, 255);
        break;
    case 4:
        digSeg(x, y, 255, 0, 255, 5, 5, 255);
        break;
    case 5:
        digSeg(x, y, 255, 3, 3, 4, 4, 255);
        break;
    case 6:
        digSeg(x, y, 255, 3, 3, 255, 4, 255);
        break;
    case 7:
        digSeg(x, y, 1, 1, 255, 32, 255, 32);
        break;
    case 8:
        digSeg(x, y, 255, 3, 255, 255, 4, 255);
        break;
    case 9:
        digSeg(x, y, 255, 3, 255, 4, 4, 255);
        break;
    case 10:
        digSeg(x, y, 32, 32, 32, 32, 32, 32);
        break;
    }
}