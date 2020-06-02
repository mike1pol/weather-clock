#include <battery.h>

extern byte mode;

// DC
uint8_t DC100[8] = {0b01110,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
uint8_t DC80[8] = {0b01110,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b10001};
uint8_t DC70[8] = {0b01110,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b10001,  0b10001};
uint8_t DC60[8] = {0b01110,  0b11111,  0b11111,  0b11111,  0b11111,  0b10001,  0b10001,  0b10001};
uint8_t DC50[8] = {0b01110,  0b11111,  0b11111,  0b11111,  0b10001,  0b10001,  0b10001,  0b10001};
uint8_t DC40[8] = {0b01110,  0b11111,  0b11111,  0b10001,  0b10001,  0b10001,  0b10001,  0b10001};
uint8_t DC30[8] = {0b01110,  0b11111,  0b10001,  0b10001,  0b10001,  0b10001,  0b10001,  0b10001};
uint8_t DC20[8] = {0b01110,  0b10001,  0b10001,  0b10001,  0b10001,  0b10001,  0b10001,  0b10001};

extern LiquidCrystal lcd;

void Battery::setup()
{
  tick();
}

int Battery::tick()
{
  int value = analogRead(BATTERY_PIN);
  current = value * 5.0 / 1023.0;
  pr = ((current - BATTERY_MIN) / (BATTERY_MAX - BATTERY_MIN)) * 100;
  Serial.print("Battery: ");
  Serial.print(current);
  Serial.print(" / ");
  Serial.println(pr);
  return current;
}

void Battery::draw()
{
  int zero = 0;
  if (pr > 100)
  {
    lcd.setCursor(15, 1);
    lcd.write(zero);
  }
  else
  {
    if (pr > 80)
      lcd.createChar(1, DC100);
    else if (pr <= 80 && pr > 70)
      lcd.createChar(1, DC80);
    else if (pr <= 70 && pr > 60)
      lcd.createChar(1, DC70);
    else if (pr <= 60 && pr > 50)
      lcd.createChar(1, DC60);
    else if (pr <= 50 && pr > 40)
      lcd.createChar(1, DC50);
    else if (pr <= 40 && pr > 30)
      lcd.createChar(1, DC40);
    else if (pr <= 30 && pr > 20)
      lcd.createChar(1, DC30);
    else if (pr <= 20)
      lcd.createChar(1, DC20);
    lcd.setCursor(15, 1);
    lcd.write(1);
  }
}