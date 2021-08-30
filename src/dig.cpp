#include "dig.h"

extern LiquidCrystal_I2C lcd;

void digSeg(byte x, byte y, byte z1, byte z2, byte z3, byte z4, byte z5, byte z6) {
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

void WriteDig(byte dig, byte x, byte y) {
  if (dig == 0)
    digSeg(x, y, 255, 1, 255, 255, 2, 255);
  else if (dig == 1)
    digSeg(x, y, 32, 255, 32, 32, 255, 32);
  else if (dig == 2)
    digSeg(x, y, 3, 3, 255, 255, 4, 4);
  else if (dig == 3)
    digSeg(x, y, 3, 3, 255, 4, 4, 255);
  else if (dig == 4)
    digSeg(x, y, 255, 0, 255, 5, 5, 255);
  else if (dig == 5)
    digSeg(x, y, 255, 3, 3, 4, 4, 255);
  else if (dig == 6)
    digSeg(x, y, 255, 3, 3, 255, 4, 255);
  else if (dig == 7)
    digSeg(x, y, 1, 1, 255, 32, 255, 32);
  else if (dig == 8)
    digSeg(x, y, 255, 3, 255, 255, 4, 255);
  else if (dig == 9)
    digSeg(x, y, 255, 3, 255, 4, 4, 255);
  else if (dig == 10)
    digSeg(x, y, 32, 32, 32, 32, 32, 32);
}
