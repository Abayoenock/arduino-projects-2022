#include <Wire.h>
//  the libries for wifi
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
// arduino json library
#include <ArduinoJson.h>
const char* ssid = "admin";         // change this to your wireless network
const char* password = "admin123";  // change this to your password network
//Your Domain name with URL path or IP address with path
const char* serverName = "http://192.168.43.196/powerMonitor/API/?t=postData";
const char* serverName2 = "http://192.168.43.196/powerMonitor/API/?t=getVal";
SoftwareSerial ss(D6, D4);  // to make the digital pins into TX and RX respectively

#include <Nokia_LCD.h>
Nokia_LCD lcd(D5 /* CLK */, D7 /* DIN */, D1 /* DC */, D8 /* CE */, D2 /* RST */);
int voltageData = 0;
//the global variable for the data recieved form the esp8266
double currentA_Data = 0, currentB_Data = 0, energyA_Data = 0, energyB_Data = 0, powerA_Data = 0, powerB_Data = 0;
// the varilable fot he display
unsigned int lastDisplayUpdate = 0;
int displayStep = 0;

// the varilables for the status of the relays
int relayBlockA = 0;
int relayBlockB = 0;

// the viable to store the data of the energy form the server
double BootEnergyA = 0;
double BootEnergyB = 0;

// to hold the total energyof the whole system
double totalEnergyA = 0;
double totalEnergyB = 0;
//the relay pins

#define relayA D0
#define relayB D3
// the variables to match the status with the system
int rs1 = 0;
int rs2 = 0;
int a = 0;
void WiFi_SetUp() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(" Network successfuly connected ");
  delay(2000);
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}


void setup() {
  Serial.begin(9600);  //open serial port, set the baud rate to 9600 bps
  ss.begin(230400);    // to initialize the baurd rate for the software serail pins
  pinMode(relayA, OUTPUT);
  pinMode(relayB, OUTPUT);
  digitalWrite(relayA, HIGH);
  digitalWrite(relayB, HIGH);
  lcd.begin();
  // Set the contrast
  lcd.setContrast(60);  // Good values are usualy between 40 and 60
  // Clear the screen by filling it with black pixels
  lcd.clear();
  lcd.setCursor(10, 3);
  lcd.setInverted(false);
  lcd.print("Welcome  ");
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(" Connecting to the network");
  delay(2000);
  WiFi_SetUp();

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(" Fetching data from  server");
  getData();
}
void loop() {
  //sendData();
  datahandler();
  displayz();
  sendData();
  delay(50);
}
void displayz() {

  if (millis() - lastDisplayUpdate >= 4000) {
    if (displayStep == 0) {
      lcd.clear();
      // Set the cursor on the beginning of the 6th row (the last one)
      lcd.setCursor(0, 0);
      lcd.setInverted(true);
      lcd.print("   Volgate    ");
      lcd.setCursor(0, 2);
      lcd.setInverted(false);
      lcd.print(voltageData);
      lcd.print(" V");
      lcd.setCursor(0, 3);
      lcd.setInverted(true);
      lcd.print("   Current    ");
      lcd.setInverted(false);
      lcd.setCursor(0, 5);
      lcd.print(currentA_Data);
      lcd.print("A ");
      lcd.setCursor(40, 5);
      lcd.print(currentB_Data);
      lcd.print("A");
      displayStep = 1;
    } else if (displayStep == 1) {
      lcd.clear();
      // Set the cursor on the beginning of the 6th row (the last one)
      lcd.setCursor(0, 0);
      lcd.setInverted(true);
      lcd.print("   Energy A   ");
      lcd.setCursor(0, 2);
      lcd.setInverted(false);
      if (totalEnergyA > 1000) {
        lcd.print(totalEnergyA / 1000);
        lcd.print(" KwH");
      } else {
        lcd.print(totalEnergyA);
        lcd.print(" wH");
      }

      lcd.setCursor(0, 3);
      lcd.setInverted(true);
      lcd.print("   Energy B   ");
      lcd.setInverted(false);
      lcd.setCursor(0, 5);
      if (totalEnergyB > 1000) {
        lcd.print(totalEnergyB);
        lcd.print(" wH");
      } else {
        lcd.print(totalEnergyB / 1000);
        lcd.print(" KwH");
      }

      displayStep = 0;
    }
    lastDisplayUpdate = millis();
  }
}
void datahandler() {
  String datas = "";
  while (ss.available() != 0) {
    datas = datas + String(char(ss.read()));
  }
  if (datas != "") {
    Serial.println(datas);
    String input = datas;
    StaticJsonDocument<356> doc;

    DeserializationError error = deserializeJson(doc, input);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }
    String step = doc["S"];      // "0.00"
    String energyA = doc["E"];   // "0.00"
    String voltage = doc["v"];   // "0.00"
    String currentA = doc["A"];  // "9.03"
    String currentB = doc["B"];  // "9.08"
    String energyB = doc["F"];   // "0.00"
    if (step.toInt() == 0) {
      voltageData = voltage.toInt();
      currentA_Data = currentA.toDouble();
      currentB_Data = currentB.toDouble();
    } else if (step.toInt() == 1) {
      energyA_Data = energyA.toDouble();
      totalEnergyA = BootEnergyA + energyA_Data;
      Serial.println("totalEnergyA");
      Serial.println(totalEnergyA);

    } else if (step.toInt() == 2) {
      energyB_Data = energyB.toDouble();
      totalEnergyB = BootEnergyB + energyB_Data;
      Serial.println("totalEnergyB");
      Serial.println(totalEnergyB);
    }
  }
}

void getData() {
  //Send an HTTP POST request every define interval


  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    // Your Domain name

    http.begin(client, serverName2);
    //content-type header
    delay(1000);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.GET();
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    if (httpResponseCode == 200) {
      a = 1;
      String payload = http.getString();
      Serial.println(payload);
      StaticJsonDocument<308> doc;

      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      String eA = doc["eA"];  // "7.9"
      String eB = doc["eB"];  // "9.7"
      String sA = doc["sA"];  // "1"
      String sB = doc["sB"];  // "1"

      relayBlockA = sA.toInt();
      relayBlockB = sB.toInt();
      BootEnergyA = eA.toDouble();
      BootEnergyB = eB.toDouble();

      rs1 = relayBlockA;
      rs2 = relayBlockB;
      relayBlockA == 0 ? relayBlockA = 1 : relayBlockA = 0;
      relayBlockB == 0 ? relayBlockB = 1 : relayBlockB = 0;
      Serial.println(BootEnergyA);
      Serial.println(BootEnergyB);
      digitalWrite(relayA, relayBlockA);
      digitalWrite(relayB, relayBlockB);

    } else {
      // when the request was not received by the server
      delay(500);
      http.end();
      getData();
    }

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
    String httpRequestData = "voltage=" + String(voltageData) + "&currentA=" + String(currentA_Data) + "&currentB=" + String(currentB_Data) + "&energyA=" + String(totalEnergyA) + "&energyB=" + totalEnergyB + "&RstatusA=" + rs1 + "&RstatusB=" + rs2;
    // Send HTTP POST request
    Serial.println(httpRequestData);
    int httpResponseCode = http.POST(httpRequestData);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    if (httpResponseCode == 200) {
      String payload = http.getString();
      Serial.println(payload);

      StaticJsonDocument<98> doc;

      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      String sA = doc["sA"];  // "1"
      String sB = doc["sB"];  // "1"
      relayBlockA = sA.toInt();
      relayBlockB = sB.toInt();
      rs1 = relayBlockA;
      rs2 = relayBlockB;
      relayBlockA == 0 ? relayBlockA = 1 : relayBlockA = 0;
      relayBlockB == 0 ? relayBlockB = 1 : relayBlockB = 0;
      Serial.println(relayBlockA);
      Serial.println(relayBlockB);
      digitalWrite(relayA, relayBlockA);
      digitalWrite(relayB, relayBlockB);

    } else {
      // when the request was not received by the server
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}