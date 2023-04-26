#include <Wire.h>
#define STEPPER_PIN_1 9
#define STEPPER_PIN_2 10
#define STEPPER_PIN_3 11
#define STEPPER_PIN_4 12
#define OpenLimit 7
#define CloseLimit 8
#define Direction 2
int step_number = 0;
String RoofStatus = "0";
int command = 1;
void setup() {
  Serial.begin(9600);
  Wire.begin(8);                /* join i2c bus with address 8 */
  Wire.onReceive(receiveEvent); /* register receive event */
  Wire.onRequest(requestEvent); /* register request event */
  pinMode(STEPPER_PIN_1, OUTPUT);
  pinMode(STEPPER_PIN_2, OUTPUT);
  pinMode(STEPPER_PIN_3, OUTPUT);
  pinMode(STEPPER_PIN_4, OUTPUT);
  pinMode(OpenLimit, INPUT);
  pinMode(CloseLimit, INPUT);
  pinMode(Direction, INPUT);
}

void loop() {


  if (digitalRead(OpenLimit) == HIGH) {
    RoofStatus = "1";
  } else if (digitalRead(CloseLimit) == HIGH) {
    RoofStatus = "0";
  } else {
    RoofStatus = "2";
  }
  if (command == 1) {
    if (digitalRead(OpenLimit) == LOW) {
      OneStep(false);
    } 
  } else {
    if (digitalRead(CloseLimit) == LOW) {
      OneStep(true);
    } 
  }

  delay(2);
}

void receiveEvent() {
  char data = Wire.read(); /* receive byte as a character */
  Serial.print(data);      /* print the character */
  String cmd = String(data);
  command = cmd.toInt();
  Serial.println(); /* to newline */
}
// function that executes whenever data is requested from master in our case the nodeMCU
void requestEvent() {

  Wire.write(RoofStatus[0]); /*send string on request */
}

void OneStep(bool dir) {

  if (dir) {
    switch (step_number) {
      case 0:
        digitalWrite(STEPPER_PIN_1, HIGH);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 1:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, HIGH);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 2:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, HIGH);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 3:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, HIGH);
        break;
    }
  } else {
    switch (step_number) {
      case 0:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, HIGH);
        break;
      case 1:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, HIGH);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 2:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, HIGH);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 3:
        digitalWrite(STEPPER_PIN_1, HIGH);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
    }
  }
  step_number++;
  if (step_number > 3) {
    step_number = 0;
  }
}