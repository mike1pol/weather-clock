#include <battery.h>

extern byte mode;

// DC
uint8_t DC100[8] = {0b01110, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111};
uint8_t DC80[8] = {0b01110, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b10001};
uint8_t DC70[8] = {0b01110, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b10001, 0b10001};
uint8_t DC60[8] = {0b01110, 0b11111, 0b11111, 0b11111, 0b11111, 0b10001, 0b10001, 0b10001};
uint8_t DC50[8] = {0b01110, 0b11111, 0b11111, 0b11111, 0b10001, 0b10001, 0b10001, 0b10001};
uint8_t DC40[8] = {0b01110, 0b11111, 0b11111, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001};
uint8_t DC30[8] = {0b01110, 0b11111, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001};
uint8_t DC20[8] = {0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001};

extern LiquidCrystal lcd;

int vc1addr = 0;

void Battery::setup() {
  vccConst1 = EEPROM.readFloat(vc1addr);
  for (int i = 0; i < 11; i++) {
    tick();
  }
}

void Battery::calibration() {
  vccConst1 = 1.1;
  Serial.print("Real VCC is: ");
  Serial.print(readVcc1());
  Serial.println("Write your VCC (in millivolts)");
  while (Serial.available() == 0);
  int inputVcc = Serial.parseInt();
  float real_const = (float) 1.1 * inputVcc / readVcc1();
  Serial.print("New voltage constant: ");
  Serial.print(real_const, 3);
  boolean w1 = EEPROM.writeFloat(vc1addr, real_const);
  if (!w1)
    Serial.println("Write EEPROM error");
  else  {
    Serial.print("New vccConst: ");
    Serial.println(EEPROM.readFloat(vc1addr), 3);
  }
  while (true);
}

long median(long newVal) {
  static long buffer[10];
  static byte count = 0;
  buffer[count] = newVal;

  if ((count < 10 - 1) and (buffer[count] > buffer[count + 1])) {
    for (int i = count; i < 10 - 1; i++) {
      if (buffer[i] > buffer[i + 1]) {
        long buff = buffer[i];
        buffer[i] = buffer[i + 1];
        buffer[i + 1] = buff;
      }
    }
  } else {
    if ((count > 0) and (buffer[count - 1] > buffer[count])) {
      for (int i = count; i > 0; i--) {
        if (buffer[i] < buffer[i - 1]) {
          long buff = buffer[i];
          buffer[i] = buffer[i - 1];
          buffer[i - 1] = buff;
        }
      }
    }
  }
  if (++count >= 10) count = 0;
  return buffer[(int)10 / 2];
}

void Battery::tick() {
  long cr = readVcc1();
  current = (long)median(cr);
  pr = floor(((double)current - BATTERY_MIN) / (BATTERY_MAX - BATTERY_MIN) * 100);
#if DEBUG
  Serial.print("VCC: ");
  Serial.print(current);
  Serial.print(" / ");
  Serial.print(pr);
  Serial.println(" ");
#endif
}

long Battery::readVcc1() const {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif
  ADCSRA |= _BV(ADEN); // Enable the ADC
  delay(20); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC)); // Detect end-of-conversion

  uint8_t low = ADCL;  // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both
  long result = (high << 8) | low;
  result = vccConst1 * 1023 * 1000 / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

void Battery::draw() const {
  int zero = 0;
  if (pr > 100) {
    lcd.setCursor(15, 1);
    lcd.write(zero);
  } else {
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
