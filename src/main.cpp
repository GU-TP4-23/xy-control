#include <Arduino.h>
#include <Serial.h>
#include <Wire.h>

#define I2C0_SLAVE_ADDR 0x56
#define I2C1_SLAVE_ADDR 0x57
#define INT_PIN A2

const char header_mv = 0x61;                    // 'a'
const char header_ok = 0x76;                    // 'v'

const int dirPin_x = 15;
const int stepPin_x = 14;
const int dirPin_y = 17;
const int stepPin_y = 16;

char header;
float x;
float y;

int Displacement_Step_Converter(int displacement_um) //function returns the step value after converting um input displacement
{
    float pinion_circumference = 25133; //this is in um //this is been calculated by hand (C = pi*D)
    int step_resolution = round(pinion_circumference/3200);
    int StepValue = round(displacement_um/step_resolution);
    return StepValue;
  }

void xyCommandHandlerGeneric(MbedI2C *bus, int howMany)
{
  Serial.println("Transmission from master device detected!");
  Serial.print("Reading message...\t");

  char buf[howMany];
  Serial.print(howMany);
  Serial.println(" bytes");
  for (int i=0; i<=howMany; i++)                // loop through all bytes received
  {
    buf[i] = (*bus).read();
  }
  Serial.print("Message received:\t");
  Serial.println(buf);

  header = buf[0];
  Serial.print("Header:\t\t\t");
  Serial.print(header);

  if (header == header_mv)
  {
    Serial.println("\t[motion command]");

    memcpy(&x, &buf[1], sizeof(x));             // decode buffer to variable
    Serial.print("X value:\t\t");
    Serial.println(x);                          // print the message

    memcpy(&y, &buf[5], sizeof(y));
    Serial.print("Y value:\t\t");
    Serial.println(y);
    
    x = Displacement_Step_Converter(x);
    y = Displacement_Step_Converter(y);

    //move to x position
    digitalWrite(dirPin_x, HIGH);

    for(int i = 0; i < x; i++)
      {
        digitalWrite(stepPin_x, HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin_x, LOW);
        delayMicroseconds(500);
    }
    digitalWrite(stepPin_x, LOW);

    //move to y position
    digitalWrite(dirPin_y, HIGH);
      for(int i = 0; i < y; i++)
      {
        digitalWrite(stepPin_y, HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin_y, LOW);
        delayMicroseconds(500);
    }
    digitalWrite(stepPin_y, LOW);

    Serial.println("Firing interrupt...");
    digitalWrite(INT_PIN, HIGH);                // toggle the interrupt pin
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(INT_PIN, LOW);                 // restore the interrupt pin
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if (header == header_ok)
  {
    Serial.println("\t[coordinates valid]");

    int done = 1;
    char buf_done[sizeof(done)+1];
    memcpy(buf_done, &done, sizeof(done));
    Wire.write(buf_done);

  }
  else
  {
    Serial.println("\t[ERROR: INVALID HEADER]");
  }
}

void xyCommandHandler0(int howMany)
{
  // executes whenever data is received from arm segment master
  // see setup() for registration
  xyCommandHandlerGeneric(&Wire, howMany);
}

void xyCommandHandler1(int howMany)
{
  // executes whenever data is received from coordinate validator master
  // see setup() for registration
  xyCommandHandlerGeneric(&Wire1, howMany);
}

void xyCoordinatesRequestHandler()
{
  Serial.println("I2C request received!");
  digitalWrite(LED_BUILTIN, HIGH);
  
  char buf_x[sizeof(x)+1];                      // temp buffer to encode x to
  memcpy(buf_x, &x, sizeof(x));                 // encode x value to temp buffer
  Serial.print("Encoded X value of\t");
  Serial.println(x);

  char buf_y[sizeof(y)+1];                      // temp buffer to encode y to
  memcpy(buf_y, &y, sizeof(y));                 // encode y value to temp buffer
  Serial.print("Encoded Y value of\t");
  Serial.println(y);

  Serial.println("Writing message:\t");
  int w_x = Wire1.write(buf_x, sizeof(x));      // write encoded x buffer to Wire buffer
  int w_y = Wire1.write(buf_y, sizeof(y));      // write encoded y buffer to Wire buffer

  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("wrote ");
  Serial.print(w_x+w_y);
  Serial.println(" bytes");
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(INT_PIN, OUTPUT);
  pinMode(stepPin_x, OUTPUT);
	pinMode(dirPin_x, OUTPUT);
  pinMode(stepPin_y, OUTPUT);
	pinMode(dirPin_y, OUTPUT);

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
  Wire.onReceive(xyCommandHandler0);
  Serial.println(" and I2C0 callback registered");

  Serial.println("initialising I2C1 (Wire1)...");
  Serial.println("mode:\tslave");
  Serial.print("addr:\t");
  Serial.println(I2C1_SLAVE_ADDR);
  Wire1.begin(I2C1_SLAVE_ADDR);
  Serial.print("I2C1 initialised");
  Wire1.onRequest(xyCoordinatesRequestHandler);
  Wire1.onReceive(xyCommandHandler1);
  Serial.println(" and I2C1 callbacks registered");
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1);
  digitalWrite(LED_BUILTIN, LOW);
  delay(24);
}
