#include <Arduino.h>
#include <Serial.h>
#include <Wire.h>
#include <xyCoordSender.h>

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(9600);
  // wake-on-serial for debugging reproducibility
  while (!Serial);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.println("ARM SEGMENT MOCK-UP");

  Serial.println("initialising I2C (Wire)...");
  Serial.println("mode:\tmaster");
  Wire.begin();
  Serial.println("I2C initialised!");
}

void loop() {
  // put your main code here, to run repeatedly:
  xyCoordSender(150000, 150000);
  delay(1000);
}
