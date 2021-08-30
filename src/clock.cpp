#include <clock.h>

extern LiquidCrystal_I2C lcd;
extern RTC_DS1307 rtc;
extern int mode;

void Clock::setup() {
  this->now = rtc.now();
  this->secs = now.second();
  this->minutes = now.minute();
  this->hours = now.hour();
  this->drawScreenOne();
}

void Clock::tick() {
  this->dotFlag = !this->dotFlag;
  if (this->dotFlag) {
    this->secs++;
    if (this->secs > 59) {
      this->secs = 0;
      this->minutes++;
    }
    if (this->minutes == 60) {
      this->now = rtc.now();
      this->secs = this->now.second();
      this->minutes = this->now.minute();
      this->hours = this->now.hour();
      if (this->hours > 23)
        this->hours = 0;
    }
  }
  if (mode == 0) {
    this->dots();
  } else if (mode == 1) {
    lcd.setCursor(10, 0);
    lcd.print(this->dotFlag ? ":" : " ");
  }
}

void Clock::drawScreenOne() const {
  byte x = 0;
  byte y = 0;
  lcd.setCursor(x, y);
  lcd.print("               ");
  lcd.setCursor(x, y + 1);
  lcd.print("               ");
  if (this->hours / 10 == 0)
    WriteDig(10, x, y);
  else
    WriteDig(this->hours / 10, x, y);
  WriteDig(this->hours % 10, x + 4, y);
  WriteDig(this->minutes / 10, x + 8, y);
  WriteDig(this->minutes % 10, x + 12, y);
}

void Clock::drawScreenTwo() const {
    lcd.setCursor(8, 0);
    if (this->hours < 10)
      lcd.print(" ");
    lcd.print(String(this->hours));
    lcd.print(this->dotFlag ? ":" : " ");
    if (this->minutes < 10)
      lcd.print("0");
    lcd.print(String(this->minutes));
}

void Clock::dots() const {
  int x = 7;
  int y = 0;
  byte code;
  if (this->dotFlag)
    code = 165;
  else
    code = 32;
  lcd.setCursor(x, y);
  lcd.write(code);
  lcd.setCursor(x, y + 1);
  lcd.write(code);
}