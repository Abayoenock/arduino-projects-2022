#include <WiFi.h>
#include <HTTPClient.h>
const char* ssid = "Wokwi-GUEST";
const char* password = "";  // define the pump pin
#define relayPin 19

//define the sensor pins

#define moistureSensor_1 34
#define moistureSensor_2 35
  // define the operator btn
#define operatorBtn 36
  // sensor values variables
int sensorA = 0;
int sensorB = 0;
int operatorVal = 0;

// arduino json library
#include <ArduinoJson.h>
//Your Domain name with URL path or IP address with path
const char* serverName = "http://irrigationSystem.enlabs.rw/api/";
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;
int relayStatus = 0;
void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  pinMode(moistureSensor_1, INPUT);
  pinMode(moistureSensor_2, INPUT);
  pinMode(operatorVal, INPUT);
  digitalWrite(relayPin, HIGH);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  delay(1000);
}

void loop() {
  readSensors();
  if ((millis() - lastTime) > timerDelay) {
    sendData();
    lastTime = millis();
  }
  delay(20);
}
void Btn() {
  if (digitalRead(operatorBtn) == HIGH) {
    Serial.println("pressed");
    operatorVal = 1;
    sendData();
    delay(1000);
  } else {
    operatorVal = 0;
  }
}
void readSensors() {
  sensorA = analogRead(moistureSensor_1);
  sensorB = analogRead(moistureSensor_2);
  sensorA =100- map(sensorA, 0, 4095, 0, 100);
  sensorB =100- map(sensorB, 0, 4095, 0, 100);
  Serial.println("sensorA");
  Serial.println(sensorA);
  Serial.println("sensorB");
  Serial.println(sensorB);
  delay(100);
}



void sendData() {
  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;
    http.useHTTP10(true);
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // Data to send with HTTP POST
    String httpRequestData = "api_key=tPmAT5Ab3j7F&sensorA=" + String(sensorA) + "&sensorB=" + String(sensorB) + "&status=" + relayStatus + "&push=" + operatorVal;
    // Send HTTP POST request
    delay(3000);
    int httpResponseCode = http.POST(httpRequestData);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    if (httpResponseCode == 200) {
      operatorVal = 0;
    }
    String payload = http.getString();
    Serial.println(payload);
    StaticJsonDocument<48> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    String status = doc["status"];  // "0"
    status.toInt() == 0 ? relayStatus = 1 : relayStatus = 0;
    Serial.println(relayStatus);

    digitalWrite(relayPin, relayStatus);
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}