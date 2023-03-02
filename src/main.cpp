#include <Arduino.h>
#include <Serial.h>
#include <Wire.h>

#define I2C0_SLAVE_ADDR 0x56
#define I2C1_SLAVE_ADDR 0x57
#define INT_PIN A2

char header;
float x;
float y;

void xyMotionCommandHandler(int howMany)
{
  // function that executes whenever data is received from master
  // this function is registered as an event, see setup()
  Serial.println("Transmission from master device detected!");
  Serial.print("Reading message...\t");

  char buf[howMany];
  Serial.print(howMany);
  Serial.println(" bytes");
  for (int i=0; i<=howMany; i++)              // loop through all bytes received
  {
    buf[i] = Wire.read();                     // append byte to buffer
  }
  Serial.print("Message received:\n");

  header = buf[0];
  Serial.print("Header:\t\t\t");
  Serial.println(header);

  memcpy(&x, &buf[1], sizeof(x));             // decode buffer to variable
  Serial.print("X value:\t\t");
  Serial.println(x);                          // print the message

  memcpy(&y, &buf[5], sizeof(y));
  Serial.print("Y value:\t\t");
  Serial.println(y);

  /*
  actuate stepper motors here
  */

  Serial.println("Firing interrupt...");
  digitalWrite(INT_PIN, HIGH);                // toggle the interrupt pin
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(INT_PIN, LOW);                 // restore the interrupt pin
  digitalWrite(LED_BUILTIN, LOW);
}

void xyCoordinatesRequestHandler()
{
  Serial.println("I2C request received!");
  digitalWrite(LED_BUILTIN, HIGH);
  
  char buf_x[sizeof(x)+1];                    // temp buffer to encode x to
  memcpy(buf_x, &x, sizeof(x));               // encode x value to temp buffer
  Serial.print("Encoded X value of\t");
  Serial.println(x);

  char buf_y[sizeof(y)+1];                    // temp buffer to encode y to
  memcpy(buf_y, &y, sizeof(y));               // encode y value to temp buffer
  Serial.print("Encoded Y value of\t");
  Serial.println(y);

  Serial.println("Writing message:\t");
  int w_x = Wire1.write(buf_x, sizeof(x));    // write encoded x buffer to Wire buffer
  int w_y = Wire1.write(buf_y, sizeof(y));    // write encoded y buffer to Wire buffer

  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("wrote ");
  Serial.print(w_x+w_y);
  Serial.println(" bytes");
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(INT_PIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(9600);
  //while (!Serial);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("T5 XY CONTROLLER");

  Serial.println("initialising I2C0 (Wire)...");
  Serial.println("mode:\tslave");
  Serial.print("addr:\t");
  Serial.print(I2C0_SLAVE_ADDR);
  Wire.begin(I2C0_SLAVE_ADDR);
  Serial.print("I2C0 initialised");
  Wire.onReceive(xyMotionCommandHandler);
  Serial.println(" and I2C0 callback registered");

  Serial.println("initialising I2C1 (Wire1)...");
  Serial.println("mode:\tslave");
  Serial.print("addr:\t");
  Serial.println(I2C1_SLAVE_ADDR);
  Wire1.begin(I2C1_SLAVE_ADDR);
  Serial.print("I2C1 initialised");
  Wire1.onRequest(xyCoordinatesRequestHandler);
  Serial.println(" and I2C1 callback registered");
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1);
  digitalWrite(LED_BUILTIN, LOW);
  delay(24);
}
