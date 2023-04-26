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

  // Setup Serial Monitor
  Serial.begin(9600);
}

void loop() {
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



  if (redPW < bluePW && redPW < greenPW) {
    Serial.println("1: Red Color");

  }

  else if (bluePW < redPW && bluePW < greenPW) {
    Serial.println("1: Blue Color");

  }

  else if (greenPW < redPW && greenPW < bluePW) {
    Serial.println("1: Green Color");
  }

  if (redPW_2 < bluePW_2 && redPW_2 < greenPW_2 )
    {
     Serial.println("2: Red Color");

    }

    else if (bluePW_2 < redPW_2 && bluePW_2 < greenPW_2)
    {
     Serial.println("2: Blue Color");

    }

  else if (greenPW_2 < redPW_2 && greenPW_2 < bluePW_2)
  {
   Serial.println("2: Green Color");

  }

  // Print output to Serial Monitor
  Serial.print("Red PW = ");
  Serial.print(redPW);
  Serial.print(" - Green PW = ");
  Serial.print(greenPW);
  Serial.print(" - Blue PW = ");
  Serial.println(bluePW);
  //Print output to Serial Monitor
  Serial.print("Red PW2 = ");
  Serial.print(redPW_2);
  Serial.print(" - Green PW2 = ");
  Serial.print(greenPW_2);
  Serial.print(" - Blue PW2 = ");
  Serial.println(bluePW_2);
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