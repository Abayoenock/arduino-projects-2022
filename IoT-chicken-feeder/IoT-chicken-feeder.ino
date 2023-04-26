#include <Servo.h>
Servo myservo;
#define SENSOR D2
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
// arduino json library
#include <ArduinoJson.h>

const char* ssid = "block1";         // change this to your wireless network
const char* password = "Admin@123";  // change this to your password network
//Your Domain name with URL path or IP address with path
const char* serverName = "http://172.16.0.122/chicken-fidder/API/";

int pump = D5;
long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
//boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
void IRAM_ATTR pulseCounter() {
  pulseCount++;
}
void WiFi_SetUp() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}
void sendData() {
  //Send an HTTP POST request every define interval

  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    // Your Domain name

    http.begin(client, serverName);
    //content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // Data to send with HTTP POST
    String httpRequestData = "t=get";
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    if (httpResponseCode == 200) {
      String payload = http.getString();
      Serial.println(payload);
      // Stream& input;

      StaticJsonDocument<140> doc;

      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      int f = doc["f"];    // 2000
      float w = doc["w"];  // 0.7
      if(f!=0){
        foodOpen(f);
      }
      if(w!=0){
        WaterFlowPass(w*1000);
        
      }

    } else {
      // when the request was not received by the server
    }





    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}
void setup() {
  Serial.begin(9600);
  myservo.attach(D1);  // attaches the servo on GIO2 to the servo object
  pinMode(SENSOR, INPUT_PULLUP);
  pinMode(pump, OUTPUT);
  digitalWrite(pump, HIGH);
  WiFi_SetUp();
  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;
  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);
}
void loop() {
  sendData();
  delay(100);
}

void foodOpen(int delayer) {

  myservo.write(180);
  delay(delayer);
  myservo.write(0);
}
void WaterFlowPass(float water) {
  digitalWrite(pump, LOW);
  totalMilliLitres = 0;
  while (1) {
    currentMillis = millis();
    if (currentMillis - previousMillis > interval) {
      pulse1Sec = pulseCount;
      pulseCount = 0;
      flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
      previousMillis = millis();
      flowMilliLitres = (flowRate / 60) * 1000;
      totalMilliLitres += flowMilliLitres;
      // Print the flow rate for this second in litres / minute
      Serial.print("Flow rate: ");
      Serial.print(int(flowRate));  // Print the integer part of the variable
      Serial.print("L/min");
      Serial.print("\t");  // Print tab space
      // Print the cumulative total of litres flowed since starting
      Serial.print("Output Liquid Quantity: ");
      Serial.print(totalMilliLitres);
      Serial.print("mL / ");
      Serial.print(totalMilliLitres / 1000);
      Serial.println("L");
    }
    if (totalMilliLitres >= water) {
      digitalWrite(pump, HIGH);
      break;
    }
    delay(10);
  }
}