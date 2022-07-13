/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/home/maxwell/CTD-IoT/capstone/scoreboardandmaze/scoreboard/src/scoreboard.ino"
// Demo the quad alphanumeric display LED backpack kit
// scrolls through every character, then scrolls Serial
// input onto the display


void write8s();
int getNthDigit(long long number, int digit);
void writeNum(long long num);
void setup();
void loop();
#line 6 "/home/maxwell/CTD-IoT/capstone/scoreboardandmaze/scoreboard/src/scoreboard.ino"
SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(MANUAL);
#include "Wire.h"
long long counter = 0;

#include "adafruit-led-backpack.h"
// 0x70 0x73 0x71
// last mid first
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 last = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 mid = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 first = Adafruit_AlphaNum4();

void write8s() {
for (int i = 0; i < 4; i++) {
    alpha4.writeDigitRaw(i, 0xFFFF);
  }
  alpha4.writeDisplay();
}
int getNthDigit(long long number, int digit) {
  for (int place = 0; place < digit; place++) number = number / 10;
  return number % 10;
}
void writeNum(long long num) {
  first.writeDigitAscii(3, getNthDigit(num, 0) + '0');
  first.writeDigitAscii(2, getNthDigit(num, 1) + '0');
  first.writeDigitAscii(1, getNthDigit(num, 2) + '0');
  first.writeDigitAscii(0, getNthDigit(num, 3) + '0');
  mid.writeDigitAscii(3, getNthDigit(num, 4) + '0');
  mid.writeDigitAscii(2, getNthDigit(num, 5) + '0');
  mid.writeDigitAscii(1, getNthDigit(num, 6) + '0');
  mid.writeDigitAscii(0, getNthDigit(num, 7) + '0');
  last.writeDigitAscii(3, getNthDigit(num, 8) + '0');
  last.writeDigitAscii(2, getNthDigit(num, 9) + '0');
  last.writeDigitAscii(1, getNthDigit(num, 10) + '0');
  last.writeDigitAscii(0, getNthDigit(num, 11) + '0');
  first.writeDisplay();
  mid.writeDisplay();
  last.writeDisplay();
  
}
void setup() {
  Serial.begin(9600);
  Wire.setSpeed(CLOCK_SPEED_400KHZ);
  Wire.begin();
  

  last.begin(0x70);  // pass in the address
  mid.begin(0x73);
  first.begin(0x71);

  
}



void loop() {
//Serial.println(getNthDigit(12345, 0));
writeNum(counter);
counter++;
}