#ifndef XY_COORD_SENDER
#define XY_COORD_SENDER
#include <Arduino.h>
#include <Serial.h>
#include <Wire.h>
#endif

#define XY_ADDR 55

void xyCoordSender(int x_coord, int y_coord)
{
  Serial.println("Encoding message...");

  char header = 0x61;                         // header ('a')                         

  Serial.print("Message header:\t");
  Serial.println(header);

  char buf_x[sizeof(x_coord)+1];                      // temp buffer to encode x to
  memcpy(buf_x, &x_coord, sizeof(x_coord));                 // encode x value to temp buffer
  Serial.print("Encoded X value of\t");
  Serial.println(x_coord);

  char buf_y[sizeof(y_coord)+1];                      // temp buffer to encode y to
  memcpy(buf_y, &y_coord, sizeof(y_coord));                 // encode y value to temp buffer
  Serial.print("Encoded Y value of\t");
  Serial.println(y_coord);

  Serial.print("Initialising transaction with slave device at ");
  Serial.print(XY_ADDR);
  Serial.println("...");
  Wire.beginTransmission(XY_ADDR);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.println("I2C transaction initialised!");
  Serial.print("Buffering message:\t");
  int w_h = Wire.write(header);                 // write header to Wire buffer
  int w_x = Wire.write(buf_x, sizeof(x_coord));       // write encoded x buffer to Wire buffer
  int w_y = Wire.write(buf_y, sizeof(y_coord));       // write encoded y buffer to Wire buffer

  Serial.print("buffered ");
  Serial.print(w_h+w_x+w_y);
  Serial.println(" bytes");

  Serial.println("Flushing buffer...");
  Wire.endTransmission();
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("I2C transaction terminated.");

  Serial.println("Requesting xy go-ahead...");
  Wire.requestFrom(XY_ADDR, 1);
  int xy_ready = Wire.read();
  Serial.print("xy_ready:\t");
  Serial.println(xy_ready);

  Serial.println("Loop completed!");
  Serial.println();
}
