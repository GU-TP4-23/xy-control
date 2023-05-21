#include <Arduino.h>
#include <Serial.h>
#include <Wire.h>
#include <AccelStepper.h>

#define I2C0_SLAVE_ADDR 55
#define I2C1_SLAVE_ADDR 56
#define INT_PIN A2

#define limitPin_yInput 10
#define limitPin_y3V3 11
#define limitPin_xInput 14
#define limitPin_x3V3 15
#define enablePin_x 18
#define enablePin_y 21
#define dirPin_x 16
#define stepPin_x 17
#define dirPin_y 19
#define stepPin_y 20

const char header_mv = 0x61;                    // 'a'
const char header_ok = 0x76;                    // 'v'

char header;
int x;
int y;
int x_steps;
int y_steps;
int initial_homing = -1;

AccelStepper stepper_x = AccelStepper(1, stepPin_x, dirPin_x);
AccelStepper stepper_y = AccelStepper(1, stepPin_y, dirPin_y);

int coordinatesToSteps(int coordinate_um)
// converts coordinates from um to steps
{
    float pinion_circumference = 37800; //this is in um //this is been calculated by hand (C = pi*D)
    float step_resolution = pinion_circumference/3200;
    int steps = round(coordinate_um/step_resolution);
    return steps;
  }

void homeXAxis()
{
  int input_state = digitalRead(limitPin_xInput);
  Serial.print("[x-axis] Input state:\t");
  Serial.println(input_state);
  digitalWrite(enablePin_x, LOW);
  digitalWrite(enablePin_y, HIGH);
  while(!digitalRead(limitPin_xInput))  //pull-down resistor keeps it low, press makes it high
    {         
      stepper_x.moveTo(initial_homing);
      stepper_x.run();
      initial_homing--;
      delay(5);
    }
  stepper_x.setCurrentPosition(0);
  initial_homing=1;
  while(digitalRead(limitPin_xInput))  
    {     
      stepper_x.moveTo(initial_homing);
      stepper_x.run();
      initial_homing++;
      delay(5);
    }
  Serial.println("[x-axis] We're home");
  stepper_x.setCurrentPosition(0);
  initial_homing = -1;
}

void homeYAxis()
{
  int input_state = digitalRead(limitPin_yInput);
  Serial.print("[y-axis] Input state:\t");
  Serial.println(input_state);
  digitalWrite(enablePin_x, HIGH);
  digitalWrite(enablePin_y, LOW);
  while(!digitalRead(limitPin_yInput))  //pull-down resistor keeps it low, press makes it high
    {         
      stepper_y.moveTo(initial_homing);
      stepper_y.run();
      initial_homing--;
      delay(5);
    }
  stepper_y.setCurrentPosition(0);
  initial_homing=1;
  while(digitalRead(limitPin_yInput))  
    {     
      stepper_y.moveTo(initial_homing);
      stepper_y.run();
      initial_homing++;
      delay(5);
    }
  Serial.println("[y-axis] We're home");
  stepper_y.setCurrentPosition(0);
}


void xyCommandHandlerGeneric(MbedI2C *bus, int howMany)
{
  Serial.print(millis());
  Serial.println("\tTransmission from master device detected!");
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
    
    //actuate stepper motors
    x_steps = coordinatesToSteps(x);
    y_steps = coordinatesToSteps(y);
    Serial.print(millis());
    Serial.println("\tActuating stepper motors by:");
    Serial.print("x:\t\t\t");
    Serial.print(x_steps);
    Serial.println("\tsteps");
    Serial.print("y:\t\t\t");
    Serial.print(y_steps);
    Serial.println("\tsteps");

    //setup stepper parameters
    stepper_x.setMaxSpeed(5000);
	  stepper_y.setMaxSpeed(5000);
	  stepper_x.setAcceleration(200);
	  stepper_y.setAcceleration(200);

    //move to x position
    digitalWrite(enablePin_x, LOW);
    digitalWrite(enablePin_y, HIGH);

    stepper_x.moveTo(x_steps);
	  stepper_x.runToPosition();

    //move to y position
    digitalWrite(enablePin_x, HIGH);
    digitalWrite(enablePin_y, LOW);

	  stepper_y.moveTo(y_steps);
	  stepper_y.runToPosition();

    digitalWrite(enablePin_x,HIGH);
    digitalWrite(enablePin_y,HIGH);
    
    Serial.println(millis());
    Serial.println("\tFiring interrupt...");
    digitalWrite(INT_PIN, HIGH);                // toggle the interrupt pin
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(INT_PIN, LOW);                 // restore the interrupt pin
    digitalWrite(LED_BUILTIN, LOW);
    
  }
  else if (header == header_ok)
  {
    Serial.println("\t[coordinates valid]");
    Serial.println();

    int done = 1;
    char buf_done[sizeof(done)+1];
    memcpy(buf_done, &done, sizeof(done));
    Wire.write(buf_done);
  }
  else
  {
    Serial.println("\t[ERROR: INVALID HEADER]");
    Serial.println();
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
  Serial.print(millis());
  Serial.println("\tI2C request received!");
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
  Serial.begin(9600);
  Serial.println("T5 XY CONTROLLER");

  stepper_x.setCurrentPosition(0);
  stepper_x.setMaxSpeed(5000);              // stepper max speed of steps per second
  stepper_x.setAcceleration(200); 
  stepper_y.setCurrentPosition(0);
  stepper_y.setMaxSpeed(5000);              // stepper max speed of steps per second 
  stepper_y.setAcceleration(200); 

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(INT_PIN, OUTPUT);
  pinMode(stepPin_x, OUTPUT);
	pinMode(dirPin_x, OUTPUT);
  pinMode(stepPin_y, OUTPUT);
	pinMode(dirPin_y, OUTPUT);

  pinMode(limitPin_xInput, INPUT);
  pinMode(limitPin_x3V3, OUTPUT);

  pinMode(limitPin_yInput, INPUT);
  pinMode(limitPin_y3V3, OUTPUT);

  gpio_pull_down(limitPin_xInput);  // pull-down for x-axis limit switch input pin
  gpio_pull_down(limitPin_yInput);  // pull-down for y-axis limit switch input pin
 
  digitalWrite(enablePin_x, HIGH);
  digitalWrite(enablePin_y, HIGH);
  digitalWrite(limitPin_x3V3, HIGH);
  digitalWrite(limitPin_y3V3, HIGH);

  digitalWrite(LED_BUILTIN, HIGH);
  homeXAxis();
  homeYAxis();
  digitalWrite(LED_BUILTIN, LOW);

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
  Wire1.onReceive(xyCommandHandler1);
  Wire1.onRequest(xyCoordinatesRequestHandler);
  Serial.println(" and I2C1 callbacks registered");
}



void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1);
  digitalWrite(LED_BUILTIN, LOW);
  delay(24);
}
