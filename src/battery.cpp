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
int vc5addr = 8;

void Battery::setup() {
    vccConst1 = EEPROM.readFloat(vc1addr);
    vccConst5 = EEPROM.readFloat(vc5addr);
    Serial.print(vccConst1, 3);
    Serial.print(" / ");
    Serial.println(vccConst5, 3);
    tick();
}

void Battery::calibration() {
    vccConst1 = 1.1;
    vccConst5 = 5.0;
    Serial.print("Real VCC is: ");
    Serial.print(readVcc1());
    Serial.print(" / ");
    Serial.println(readVcc1());
    Serial.println("Write your VCC (in millivolts)");
    while (Serial.available() == 0);
    int inputVcc = Serial.parseInt();
    float real_const = (float) 1.1 * inputVcc / readVcc1();
    float real5_const = (float) 5.0 * inputVcc / readVcc5();
    Serial.print("New voltage constant: ");
    Serial.print(real_const, 3);
    Serial.print(" / ");
    Serial.println(real5_const, 3);
    boolean w1 = EEPROM.writeFloat(vc1addr, real_const);
    boolean w2 = EEPROM.writeFloat(vc5addr, real5_const);
    if (!w1 || !w2)
        Serial.println("Write EEPROM error");
    Serial.print(EEPROM.readFloat(vc1addr), 3);
    Serial.print(" / ");
    Serial.println(EEPROM.readFloat(vc5addr), 3);
    while (true);
}

void Battery::tick() {
    long nr = readVcc5();
    Serial.print("VCC5: ");
    Serial.print(nr);
    int nPr = (((nr - BATTERY_MIN) / (BATTERY_MAX - BATTERY_MIN)) * 100);
    Serial.print(" / ");
    Serial.println(nPr);

    current = readVcc1();
    pr = ((current - BATTERY_MIN) / (BATTERY_MAX - BATTERY_MIN)) * 100;
    Serial.print("VCC1: ");
    Serial.print(current);
    Serial.print(" / ");
    Serial.println(pr);
    Serial.println(" ");
}

long Battery::readVcc5() {
    int value = analogRead(BATTERY_PIN);
    long nr = vccConst5 * 1023 * 1000 / value;
    return nr;
}

long Battery::readVcc1() {
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

void Battery::draw() {
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
