#include <Arduino.h>
#include <Serial.h>
#include <Wire.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("ARM SEGMENT MOCK-UP");
  Serial.println("initialising I2C (Wire)...");
  Serial.println("mode:\tmaster");
  Wire.begin();
  Serial.println("I2C initialised!");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Initialising transmission with slave device at 0xbf...");
  Wire.beginTransmission(0xbf);

  Serial.println("Transmission initialised!");
  float x = 123.456;                // value to encode
  Serial.print("Writing message:\t");
  Serial.println(x);
  char buf[sizeof(float)];          // buffer to send
  memcpy(buf, &x, sizeof(x));       // encode value to buffer
  Wire.write(buf);                  // send buffer
  Serial.println("Done.");

  Serial.println("Terminating transmission...");
  Wire.endTransmission();
  Serial.println("Transmission terminated.");

  delay(2000);
}
