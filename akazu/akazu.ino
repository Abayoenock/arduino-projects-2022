#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
// THE LIBRARIES FOR THE LOAD CELL
#include <HX711_ADC.h>
#if defined(ESP8266) || defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

//pins:
const int HX711_dout = D5;  //mcu > HX711 dout pin
const int HX711_sck = D6;   //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;

#define echoPin D1
#define trigPin D2
int distance;  // variable for the distance measurement
int state = 0;
int count = 0;
long duration = 0;
int removed = 0;  // to hold the number of removed eggs
int eggWeight = 50;
float lastWeight = 0;
float Weight = 0;
int i = 0;
// THE network parameters
const char* ssid = "Info";                                         // network name
const char* password = "enlabs123";                                // network password
const char* host = "http://192.168.43.61/akazu_system/data.php";  // chnage the ip to your own ip address =>ipconfig
HTTPClient http;
WiFiClient WiFiclient;
unsigned int lastMillis = 0;
void setup() {

  Serial.begin(57600);
  delay(10);
  Serial.println();
  Serial.println("Starting...");

  LoadCell.begin();
  //LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive
  float calibrationValue;    // calibration value (see example file "Calibration.ino")
  calibrationValue = 696.0;  // uncomment this if you want to set the calibration value in the sketch
#if defined(ESP8266) || defined(ESP32)
  EEPROM.begin(512);  // uncomment this if you use ESP8266/ESP32 and want to fetch the calibration value from eeprom
#endif
  EEPROM.get(calVal_eepromAdress, calibrationValue);  // uncomment this if you want to fetch the calibration value from eeprom

  unsigned long stabilizingtime = 2000;  // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true;                  //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1)
      ;
  } else {
    LoadCell.setCalFactor(calibrationValue);  // set calibration value (float)
    Serial.println("Startup is complete");
  }

  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT);   // Sets the echoPin as an INPUT


  //connecting to the network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  CountEggs();
  Measure_Weight();
  send_data(0);
}
void CountEggs() {
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;  // Speed of sound wave divided by 2 (go and back)
  // Displays the distance_left_left on the Serial Monitor
  Serial.print("distance: ");
  Serial.print(distance);
  if (distance <= 35) {
    if (state == 0) {
      count++;
      state = 1;
      send_data(1);
    }
  } else {
    state = 0;
  }
  Serial.println("count : ");
  Serial.print(count);
  Serial.println("");
}
void Measure_Weight() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0;  //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell.update()) newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float i = LoadCell.getData();
      Serial.print("Load_cell output val: ");
      Serial.println(i);
      Serial.println("");
      Serial.println("count : ");
      Serial.print(count);
      Serial.println("");
      Weight = i;
      newDataReady = 0;
      t = millis();
      lastWeight = Weight;
      if (Weight < 18) {
        lastWeight = 0;
      }
    }
  }
}

void send_data(int action) {
  String url = host;
  if (action == 1) {

    url += "?task=laid";
    url += "&count=";
    url += count;
  } else {

    url += "?task=corrected";
    url += "&count=";
    url += lastWeight;
  }


  http.begin(WiFiclient, url);
  http.addHeader("Content-Type", "text/plain");
  int httpCode = http.GET();
  for (int i = 0; i < 300; i++) {
    Measure_Weight();
    CountEggs();
    delay(10);
  }
  String payload = http.getString();  // get data from webhost continously
  Serial.println(payload);
  String input = payload;
  StaticJsonDocument<96> doc;

  DeserializationError error = deserializeJson(doc, input);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }


  int status = doc["status"];  // 0
  if (status == 1) {
    count = 0;
  } else {
  }
  for (int i = 0; i < 50; i++) {
    Measure_Weight();
    CountEggs();
    delay(10);
  }
  http.end();
}
