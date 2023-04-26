

// the pins for the motor driver
int motor_right_a = 2;
int motor_right_b = 3;
int motor_left_a = 4;
int motor_left_b = 5;
// the pins for the infrared sensors
int L_S = A3;
int S_S = A2;
int R_S = A1;
// the pins to which the enable pins for the motor driver are connected to
int speed_motor_a = 9;
int speed_motor_b = 10;

// the ultrasonic sensor
int trigPin = 30;  // Trigger
int echoPin = 31;  // Echo
// the variables to hold the lenght measured by the ultrasonic sensor
long duration, cm, inches;

// the pins for the limit switches
#define stop_down 32
#define stop_up 33
// the varibale to check the status of the lift
int lifted = 0;
// defining the GPIO to which the stepper motor is connected to
#define STEPPER_PIN_1 6
#define STEPPER_PIN_2 7
#define STEPPER_PIN_3 8
#define STEPPER_PIN_4 11
// the step check for the stepper motor
int step_number = 0;

int colorLifted = 0;
int colorGround = 0;

///################# color sensors ############################3
// Define color sensor pins
#define S0 22
#define S1 23
#define S2 24
#define S3 25
#define sensorOut 12


// Define color sensor pins
#define S0_2 26
#define S1_2 27
#define S2_2 28
#define S3_2 18
#define sensorOut_2 13
// Variables for Color Pulse Width Measurements
int redPW = 0;
int greenPW = 0;
int bluePW = 0;


int redPW_2 = 0;
int greenPW_2 = 0;
int bluePW_2 = 0;


void setup() {
  Serial.begin(9600);
  pinMode(motor_right_a, OUTPUT);
  pinMode(motor_left_a, OUTPUT);
  pinMode(motor_right_b, OUTPUT);
  pinMode(motor_left_b, OUTPUT);

  pinMode(stop_down, INPUT);
  pinMode(stop_up, INPUT);
  pinMode(speed_motor_a, OUTPUT);
  pinMode(speed_motor_b, OUTPUT);

  pinMode(L_S, INPUT);
  pinMode(S_S, INPUT);
  pinMode(R_S, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(STEPPER_PIN_1, OUTPUT);
  pinMode(STEPPER_PIN_2, OUTPUT);
  pinMode(STEPPER_PIN_3, OUTPUT);
  pinMode(STEPPER_PIN_4, OUTPUT);


  /// color sensors
  // Set S0 - S3 as outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  // Set S0 - S3 as outputs
  pinMode(S0_2, OUTPUT);
  pinMode(S1_2, OUTPUT);
  pinMode(S2_2, OUTPUT);
  pinMode(S3_2, OUTPUT);

  // Set Pulse Width scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  // Set Pulse Width scaling to 20%
  digitalWrite(S0_2, HIGH);
  digitalWrite(S1_2, LOW);
  // Set Sensor output as input
  pinMode(sensorOut, INPUT);
  pinMode(sensorOut_2, INPUT);



  // up_lift();
}
void loop() {

  // OneStep(false);

  analogWrite(speed_motor_a, 130);
  analogWrite(speed_motor_b, 130);

  obstacle();
  if (cm < 10) {
    Stop();
  }

  Serial.println("----------------------------------------------");
  Serial.println(digitalRead(L_S));
  Serial.println(digitalRead(S_S));
  Serial.println(digitalRead(R_S));

  if ((digitalRead(L_S) == 0) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 0)) {
    forword();
  }
  if ((digitalRead(L_S) == 1) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 0) && lifted == 1) {
    Stop();
    colorsGround();
    delay(1000);
    if (colorLifted == colorGround) {
      Serial.println("the colors match");
      up_in_place();
    } else {
      Serial.println("the colors do not match");
      forword();
      delay(1000);
    }
  }
  // if ((digitalRead(L_S) == 1) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 1) && lifted == 1) {
  //   // Stop();
  //   // delay(1000);
  //   //up_in_place();
  // }

  if ((digitalRead(L_S) == 1) && (digitalRead(S_S) == 0) && (digitalRead(R_S) == 0)) {
    turnLeft();
  }

  if ((digitalRead(L_S) == 1) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 0)) {
    turnLeft();
  }

  if ((digitalRead(L_S) == 0) && (digitalRead(S_S) == 0) && (digitalRead(R_S) == 1)) {
    turnRight();
  }
  if ((digitalRead(L_S) == 0) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 1)) {
    turnRight();
  }


  if ((digitalRead(L_S) == 1) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 1)) {  // when the car reaches the stop place
    Stop();
    delay(2000);
    if (cm < 40) {
      down_lift();
      while (1) {
        analogWrite(speed_motor_a, 70);
        analogWrite(speed_motor_b, 70);
        obstacle();
        if (cm < 6) {
          Stop();
          delay(1000);
          up_lift();
          colorsLift();
          delay(1000);
          lifted = 1;
          forword();
          delay(500);
          break;
        } else {
          forword();
        }
      }
    }
  }
}
void forword() {
  digitalWrite(motor_left_a, HIGH);
  digitalWrite(motor_left_b, LOW);
  digitalWrite(motor_right_a, HIGH);
  digitalWrite(motor_right_b, LOW);
}
void backward() {
  digitalWrite(motor_left_a, LOW);
  digitalWrite(motor_left_b, HIGH);
  digitalWrite(motor_right_a, LOW);
  digitalWrite(motor_right_b, HIGH);
}

void turnRight() {

  digitalWrite(motor_left_a, HIGH);
  digitalWrite(motor_left_b, LOW);
  digitalWrite(motor_right_a, LOW);
  digitalWrite(motor_right_b, LOW);
}
void turnLeft() {
  digitalWrite(motor_left_a, LOW);
  digitalWrite(motor_left_b, LOW);
  digitalWrite(motor_right_a, HIGH);
  digitalWrite(motor_right_b, LOW);
}

void Stop() {
  digitalWrite(motor_left_a, LOW);
  digitalWrite(motor_left_b, LOW);
  digitalWrite(motor_right_a, LOW);
  digitalWrite(motor_right_b, LOW);
}
void obstacle() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  // Convert the time into a distance
  cm = (duration / 2) / 29.1;  // Divide by 29.1 or multiply by 0.0343
   Serial.print("cm");
  Serial.print(cm);
   Serial.println("");
}
void down_lift() {

  while (1) {
    if (digitalRead(stop_down) == HIGH) {

      break;
    } else {

      OneStep(false);
       delay(2);
    }
  }
}
void up_lift() {

  while (1) {
    if (digitalRead(stop_up) == HIGH) {

      break;
    } else {
      OneStep(true);
       delay(2);
    }
  }
}
void up_in_place() {
  while (1) {
    analogWrite(speed_motor_a, 80);
    analogWrite(speed_motor_b, 80);

    if ((digitalRead(L_S) == 1) && (digitalRead(S_S) == 1) && (digitalRead(R_S) == 1)) {
      Stop();
      delay(2000);
      down_lift();
      delay(2000);
      back_line();
      break;
    } else {
      forword();
    }
  }
}
void back_line() {
  int x = 0;
  while (1) {
    analogWrite(speed_motor_a, 90);
    analogWrite(speed_motor_b, 90);
    if (x == 0) {
      backward();
      delay(1000);
      x++;
    }
    if ((digitalRead(L_S) == 1) || (digitalRead(S_S) == 1) || (digitalRead(R_S) == 1)) {
      Stop();
      delay(2000);
      up_lift();
      lifted = 0;
      delay(2000);
      break;
    } else {
      backward();
    }
  }
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

void colorsGround() {
  int greens = 0, reds = 0,blues=0;
  for (int i = 0; i < 10; i++) {
    // Read Red Pulse Width
    redPW = getRedPW();
    // Delay to stabilize sensor
    delay(100);

    // Read Green Pulse Width
    greenPW = getGreenPW();
    // Delay to stabilize sensor
    delay(100);

    // Read Blue Pulse Width
    bluePW = getBluePW();
    // Delay to stabilize sensor
    delay(100);
    // Read Red Pulse Width

    if (redPW < bluePW && redPW < greenPW) {
      Serial.println("1: Red Color");
      reds += 1;
    }

    else if (bluePW < redPW && bluePW < greenPW) {
      Serial.println("1: Blue Color");
      blues += 1;
    }

    else if (greenPW < redPW && greenPW < bluePW) {
      Serial.println("1: Green Color");
      // int colorLifted=0;
      greens += 1;
    }

//

    // Print output to Serial Monitor
    Serial.print("Red PW = ");
    Serial.print(redPW);
    Serial.print(" - Green PW = ");
    Serial.print(greenPW);
    Serial.print(" - Blue PW = ");
    Serial.println(bluePW);
  }
  if (greens > reds && greens>blues) {
    colorGround = 1;
    Serial.println("green");
  } else if(reds > greens && reds>blues){
    colorGround= 2;
     Serial.println("reds");
  }
  else{
    colorGround=3;
     Serial.println("blue");
  }
}
void colorsLift() {
  int greens = 0, reds = 0,blues=0;
  for (int i = 0; i < 5; i++) {
    // Read Red Pulse Width
    redPW_2 = getRedPW2();
    // Delay to stabilize sensor
    delay(100);

    // Read Green Pulse Width
    greenPW_2 = getGreenPW2();
    //Delay to stabilize sensor
    delay(100);

    // Read Blue Pulse Width
    bluePW_2 = getBluePW2();
    // Delay to stabilize sensor
    delay(100);

    if (redPW_2 < bluePW_2 && redPW_2 < greenPW_2) {
      Serial.println("2: Red Color");

      reds += 1;
    }

    else if (bluePW_2 < redPW_2 && bluePW_2 < greenPW_2) {
      Serial.println("2: Blue Color");
      blues += 1;
    }

    else if (greenPW_2 < redPW_2 && greenPW_2 < bluePW_2) {
      Serial.println("2: Green Color");

      greens += 1;      
    }


    //Print output to Serial Monitor
    Serial.print("Red PW2 = ");
    Serial.print(redPW_2);
    Serial.print(" - Green PW2 = ");
    Serial.print(greenPW_2);
    Serial.print(" - Blue PW2 = ");
    Serial.println(bluePW_2);
  }
  if (greens > reds && greens>blues) {
    colorLifted = 1;
    Serial.println("green");
  } else if(reds > greens && reds>blues){
    colorLifted = 2;
     Serial.println("red");
  }
  else{
    colorLifted=3;
     Serial.println("blue");
  }
}

// Function to read Red Pulse Widths
int getRedPW() {
  // Set sensor to read Red only
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}
// Function to read Red Pulse Widths
int getRedPW2() {
  // Set sensor to read Red only
  digitalWrite(S2_2, LOW);
  digitalWrite(S3_2, LOW);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut_2, LOW);
  // Return the value
  return PW;
}

// Function to read Green Pulse Widths
int getGreenPW() {
  // Set sensor to read Green only
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}
// Function to read Green Pulse Widths
int getGreenPW2() {
  // Set sensor to read Green only
  digitalWrite(S2_2, HIGH);
  digitalWrite(S3_2, HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut_2, LOW);
  // Return the value
  return PW;
}

// Function to read Blue Pulse Widths
int getBluePW() {
  // Set sensor to read Blue only
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}

// Function to read Blue Pulse Widths
int getBluePW2() {
  // Set sensor to read Blue only
  digitalWrite(S2_2, LOW);
  digitalWrite(S3_2, HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut_2, LOW);
  // Return the value
  return PW;
}
