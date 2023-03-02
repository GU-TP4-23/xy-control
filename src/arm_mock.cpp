#include <Arduino.h>
#include <Serial.h>
#include <Wire.h>

#define XY_ADDR 0x56

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(9600);
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
  Serial.println("Encoding message...");

  char header = 'a';                           // header
  float x = 123.456;                           // x value
  float y = 789;                               // y value

  Serial.print("Message header:\t");
  Serial.println(header);

  char buf_x[sizeof(x)+1];                      // temp buffer to encode x to
  memcpy(buf_x, &x, sizeof(x));                 // encode x value to temp buffer
  Serial.print("Encoded X value of\t");
  Serial.println(x);

  char buf_y[sizeof(y)+1];                      // temp buffer to encode y to
  memcpy(buf_y, &y, sizeof(y));                 // encode y value to temp buffer
  Serial.print("Encoded Y value of\t");
  Serial.println(y);

  Serial.print("Initialising transaction with slave device at ");
  Serial.print(XY_ADDR);
  Serial.println("...");
  Wire.beginTransmission(XY_ADDR);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.println("I2C transaction initialised!");
  Serial.print("Buffering message:\t");
  int w_h = Wire.write(header);                 // write header to Wire buffer
  int w_x = Wire.write(buf_x, sizeof(x));       // write encoded x buffer to Wire buffer
  int w_y = Wire.write(buf_y, sizeof(y));       // write encoded y buffer to Wire buffer
  
  Serial.print("buffered ");
  Serial.print(w_h+w_x+w_y);
  Serial.println(" bytes");

  Serial.println("Flushing buffer...");
  Wire.endTransmission();
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("I2C transaction terminated.");
  Serial.println();

  delay(10000);
}
