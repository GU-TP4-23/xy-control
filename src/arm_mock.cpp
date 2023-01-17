#include <Arduino.h>
#include <i2c.h>

i2c_inst *i2c;

void setup() {
  // put your setup code here, to run once:
  _i2c_init(i2c, 48000);
}

void loop() {
  // put your main code here, to run repeatedly:
}
