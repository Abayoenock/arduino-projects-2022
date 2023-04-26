
#define echoPin_left_left 12
#define trigPin_left_left 11

#define echoPin_left_right 8
#define trigPin_left_right 5

#define echoPin_right_left 10
#define trigPin_right_left 9

#define echoPin_right_right 4
#define trigPin_right_right 3

// define the LED pins

#define led_right A1
#define led_left A2
#define green_right A3
#define green_left A4

// define the buzzer pin
#define buzzer 2
int buzzerStatus = 0;
//DEFINE THE DELAY BETTWEEN THE BUZZER SOUNDS
#define Bdelay 250
// to check the last update of the buzzer status
unsigned int lastBuzzerUpdate = 0;

// defines variables
long duration;  // variable for the duration of sound wave travel
//the variables for the left left
int distance_left_left;  // variable for the distance measurement
int state_left_left = 0;
int count_left_left = 0;
//the variables for the left right
int distance_left_right;  // variable for the distance measurement
int state_left_right = 0;
int count_left_right = 0;
//the variables for the left left
int distance_right_left;  // variable for the distance measurement
int state_right_left = 0;
int count_right_left = 0;
//the variables for the left left
int distance_right_right;  // variable for the distance measurement
int state_right_right = 0;
int count_right_right = 0;
unsigned int lastTimeLeft = 0;
unsigned int lastTimeRight = 0;
void setup() {
  pinMode(trigPin_left_left, OUTPUT);  // Sets the trigPin as an OUTPUT
  pinMode(echoPin_left_left, INPUT);   // Sets the echoPin as an INPUT

  pinMode(trigPin_left_right, OUTPUT);  // Sets the trigPin as an OUTPUT
  pinMode(echoPin_left_right, INPUT);   // Sets the echoPin as an INPUT

  pinMode(trigPin_right_right, OUTPUT);  // Sets the trigPin as an OUTPUT
  pinMode(echoPin_right_right, INPUT);   // Sets the echoPin as an INPUT

  pinMode(trigPin_right_left, OUTPUT);  // Sets the trigPin as an OUTPUT
  pinMode(echoPin_right_left, INPUT);   // Sets the echoPin as an INPUT
  pinMode(buzzer, OUTPUT);
  pinMode(led_right, OUTPUT);
  pinMode(led_left, OUTPUT);
  pinMode(green_left, OUTPUT);
  pinMode(green_right, OUTPUT);

  Serial.begin(9600);  // // Serial Communication is starting with 9600 of baudrate speed
}
void loop() {
  left_left();
  left_right();
  right_right();
  right_left();
  compute();
  delay(100);
}
void compute() {
  //to compute  if the time elapse has excedded the time set
  if ((count_left_left - count_right_left > 0) && (millis() - lastTimeLeft >= 20000)) {
    count_left_left = 0;
    count_right_left = 0;
  }
  if ((count_right_right - count_left_right > 0) && (millis() - lastTimeRight > 20000)) {
    count_right_right = 0;
    count_left_right = 0;
  }

  if (count_left_left - count_right_left > 0) {

    rightDisplay(0, 1);

  } else if (count_left_left - count_right_left == 0) {
    rightDisplay(1, 0);
  } else {
    count_left_left = 0;
    count_right_left = 0;
  }
  if (count_right_right - count_left_right > 0) {


    leftDisplay(0, 1);

  } else if (count_right_right - count_left_right == 0) {
    leftDisplay(1, 0);
  } else {
    count_right_right = 0;
    count_left_right = 0;
  }

  if ((count_left_left - count_right_left > 0) || (count_right_right - count_left_right > 0)) {
    buzzerFunc(1);
  } else {
    buzzerFunc(0);
  }
}
void rightDisplay(int green, int led) {
  digitalWrite(green_right, green);
  digitalWrite(led_right, led);
}
void leftDisplay(int green, int led) {
  digitalWrite(green_left, green);
  digitalWrite(led_left, led);
}

void buzzerFunc(int i) {
  if (i == 1) {
    if (millis() - lastBuzzerUpdate >= Bdelay) {
      buzzerStatus == 0 ? buzzerStatus = 1 : buzzerStatus = 0;
      digitalWrite(buzzer, buzzerStatus);
      lastBuzzerUpdate = millis();
    }
  } else {
    digitalWrite(buzzer, LOW);
  }
}

void left_left() {
  // Clears the trigPin condition
  digitalWrite(trigPin_left_left, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin_left_left, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_left_left, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin_left_left, HIGH);
  // Calculating the distance
  distance_left_left = duration * 0.034 / 2;  // Speed of sound wave divided by 2 (go and back)
  // Displays the distance_left_left on the Serial Monitor
  Serial.print("distance_left_left: ");
  Serial.print(distance_left_left);
  if (distance_left_left <= 10) {
    if (state_left_left == 0) {
      count_left_left++;
      state_left_left = 1;
      lastTimeLeft = millis();
    }
  } else {
    state_left_left = 0;
  }
  Serial.println("count : ");
  Serial.print(count_left_left);
  Serial.println("");
}


void left_right() {
  // Clears the trigPin condition
  digitalWrite(trigPin_left_right, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin_left_right, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_left_right, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin_left_right, HIGH);
  // Calculating the distance
  distance_left_right = duration * 0.034 / 2;  // Speed of sound wave divided by 2 (go and back)
  // Displays the distance_left_right on the Serial Monitor
  Serial.print("distance_left_right: ");
  Serial.print(distance_left_right);
  if (distance_left_right <= 10) {
    if (state_left_right == 0) {
      count_left_right++;
      state_left_right = 1;
    }
  } else {
    state_left_right = 0;
  }
  Serial.println("count : ");
  Serial.print(count_left_right);
  Serial.println("");
}

void right_left() {
  // Clears the trigPin condition
  digitalWrite(trigPin_right_left, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin_right_left, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_right_left, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin_right_left, HIGH);
  // Calculating the distance
  distance_right_left = duration * 0.034 / 2;  // Speed of sound wave divided by 2 (go and back)
  // Displays the distance_right_left on the Serial Monitor
  Serial.print("distance_right_left: ");
  Serial.print(distance_right_left);
  if (distance_right_left <= 10) {
    if (state_right_left == 0) {
      count_right_left++;


      state_right_left = 1;
    }
  } else {
    state_right_left = 0;
  }
  Serial.println("count : ");
  Serial.print(count_right_left);
  Serial.println("");
}
void right_right() {
  // Clears the trigPin condition
  digitalWrite(trigPin_right_right, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin_right_right, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_right_right, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin_right_right, HIGH);
  // Calculating the distance
  distance_right_right = duration * 0.034 / 2;  // Speed of sound wave divided by 2 (go and back)
  // Displays the distance_right_right on the Serial Monitor
  Serial.print("distance_right_right: ");
  Serial.print(distance_right_right);
  if (distance_right_right <= 10) {
    if (state_right_right == 0) {
      count_right_right++;
      state_right_right = 1;
      lastTimeRight = millis();
    }
  } else {
    state_right_right = 0;
  }
  Serial.println("count : ");
  Serial.print(count_right_right);
  Serial.println("");
}
