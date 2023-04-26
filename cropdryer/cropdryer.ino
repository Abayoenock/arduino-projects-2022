#include <Wire.h>
//  the libries for wifi
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
// arduino json library
#include <ArduinoJson.h>
//DHT libraries
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
// The libraies for the lcd
#include <LCD_I2C.h>
//lcd globals
LCD_I2C lcd(0x27, 16, 2);  // Default address of most PCF8574 modules, change according

// DHT globals
#define DHTPIN D5      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // DHT 11
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
unsigned int lastDHTread = 0;
float temp = 0;
int humidity = 0;
unsigned int lastDisplay=0;
// define the pin where the rain sensor is connected to
#define rainSensor D6
int rain = 0;
// the button for the operator
#define operatorBtn D7

// the variable to hold the status of the mode the device is in
int modes = 1;
// the variable to get the status of the roof from the arduino
String roofStatus = "0";
// the uv sensor globals
int UVsensorIn = A0;  //Output from the sensor
float UV_Value = 0;
unsigned int lastUvRead = 0;

const char* ssid = "Chriss 4G";         // change this to your wireless network
const char* password = "christian123";  // change this to your password network
//Your Domain name with URL path or IP address with path
const char* serverName = "http://192.168.247.196/cropDryer/API/";
const char* serverName2 = "http://192.168.247.196/cropDryer/API/op/";
void DHT_SetUp() {
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
}
void WiFi_SetUp() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
   lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print("Network");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("WiFi connected");
}



void setup() {
  pinMode(UVsensorIn, INPUT);
  pinMode(rainSensor, INPUT);
  pinMode(operatorBtn, INPUT);
  Serial.begin(9600);  //open serial port, set the baud rate to 9600 bps
  Wire.begin(D2, D1);
  lcd.begin();
    lcd.backlight();
  DHT_SetUp();
  WiFi_SetUp();
}
void loop() {
  UvReadFunction();
  DHT_ReadFunction();
  detectRain();
  statusData();
  Operator();
  display();
  sendData();
}
void display(){
if(millis()-lastDisplay>=2000){
lcd.clear();
lcd.setCursor(0,0);
lcd.print("T: ");
lcd.print(temp);
lcd.print("C  H:");
lcd.print(humidity);
lcd.print(" %");
lcd.print(0,1);
lcd.print("UV :");
lcd.print(UV_Value);
lastDisplay=millis();
}

}
void statusData() {
  Wire.requestFrom(8, 1);

  char data = Wire.read();
  Serial.println(data);

  roofStatus = data;
}
void Operator() {
  if (digitalRead(operatorBtn) == HIGH) {
    Serial.println("presssed");
    OperatorControl(1);
  }

  else {
  }
}

void OperatorControl(int m) {
  //Send an HTTP POST request every define interval

  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    // Your Domain name
    http.begin(client, serverName2);
    //content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "op=" + String(2) + "&temp=" + String(temp) + "&humidity=" + String(humidity) + "&uv=" + String(UV_Value) + "&rain=" + String(rain) + "&roofStatus=" + roofStatus;
    // Data to send with HTTP POST
    if (m == 0) {
      String httpRequestData = "op=" + String(1) + "&temp=" + String(temp) + "&humidity=" + String(humidity) + "&uv=" + String(UV_Value) + "&rain=" + String(rain) + "&roofStatus=" + roofStatus;
    } 

    // Send HTTP POST request
    Serial.println(httpRequestData);
    int httpResponseCode = http.POST(httpRequestData);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
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
    String httpRequestData = "temp=" + String(temp) + "&humidity=" + String(humidity) + "&uv=" + String(UV_Value) + "&rain=" + String(rain) + "&roofStatus=" + roofStatus;
    // Send HTTP POST request
    Serial.println(httpRequestData);
    int httpResponseCode = http.POST(httpRequestData);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    if (httpResponseCode == 200) {
      String payload = http.getString();
      Serial.println(payload);

      StaticJsonDocument<48> doc;

      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }

      const char* roof = doc["roof"];  // "1"
      const char* mode = doc["mode"];  // "0"
      String roofData = String(roof);
      String modeData = String(mode);
      modes = modeData.toInt();
      if (roofData.toInt() == 1) {
        Wire.beginTransmission(8);
        Wire.write("1");
        Wire.endTransmission();
      }
      if (roofData.toInt() == 0) {
        Wire.beginTransmission(8);
        Wire.write("0");
        Wire.endTransmission();
      }

    } else {
      // when the request was not received by the server
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void DHT_ReadFunction() {
  // Delay between measurements.
  delay(delayMS);
  if (millis() - lastDHTread >= delayMS) {
    // Get temperature event and print its value.
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println(F("Error reading temperature!"));
    } else {
      Serial.print(F("Temperature: "));
      Serial.print(event.temperature);
      temp = event.temperature;
      Serial.println(F("°C"));
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println(F("Error reading humidity!"));
    } else {
      Serial.print(F("Humidity: "));
      Serial.print(event.relative_humidity);
      humidity = event.relative_humidity;
      Serial.println(F("%"));
    }
    lastDHTread = millis();
  }
}

void detectRain() {
  if (digitalRead(rainSensor) == HIGH) {
    rain = 1;
  } else {
    rain = 0;
  }
}


void UvReadFunction() {
  if (millis() - lastUvRead >= 150) {
    int uvLevel = averageAnalogRead(UVsensorIn);

    float outputVoltage = 3.3 * uvLevel / 1024;
    float uvIntensity = mapfloat(outputVoltage, 0.99, 2.9, 0.0, 15.0);
    UV_Value = uvIntensity;

    Serial.print(" UV Intensity: ");
    Serial.print(uvIntensity);
    Serial.print(" mW/cm^2");
    Serial.println();
    lastUvRead = millis();
  }
}

//Takes an average of readings on a given pin
//Returns the average
int averageAnalogRead(int pinToRead) {
  byte numberOfReadings = 8;
  unsigned int runningValue = 0;

  for (int x = 0; x < numberOfReadings; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return (runningValue);
}


float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}