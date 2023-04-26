#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <Temperature_LM75_Derived.h>
#define REPORTING_PERIOD_MS 1000
#define HelpBtn 7
Generic_LM75 temperature;
PulseOximeter pox;
uint32_t tsLastReport = 0;
void onBeatDetected() {
  Serial.println("Beat!");
}
String serialNumber = "RDF-175A";
String API = " http://soldierhealth.enlabs.rw/api/";

static const int RXPin = 11, TXPin = 12;
static const int RXPin_gsm = 3, TXPin_gsm = 2;  //GSM
static const uint32_t GPSBaud = 9600;
String longi;  // to hold the longitude
String lati;   // to hold the latitude
// The TinyGPSPlus object
TinyGPSPlus gps;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);
SoftwareSerial gsm(RXPin_gsm, TXPin_gsm);
int d = 0;
int steps = 0;                           // to continusly check the next step to send the data via the gsm
int s = 0;                               // to initialize the send of at commands
unsigned long delayer_millis = 0;        // the amount of delay before next command
unsigned long currentMillis_d = 0;       // to holld the current time
unsigned long lastExecutedMillis_d = 0;  // to hold the last miilisenconds that the data was sent


// the variables to hold the sensor values
int oxygen = 0;
float bodyTemp = 0;
int heartBeat = 0;
int helpVal = 0;

void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);
  gsm.begin(115200);
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;)
      ;
  } else {
    Serial.println("SUCCESS");
  }
  pox.setOnBeatDetectedCallback(onBeatDetected);
  pinMode(HelpBtn, OUTPUT);
}

void loop() {
  SensorData();  //the function to read the sensor values
  ss.listen();
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      displayInfo();
      helpFunc();
      gsm.listen();
      sendLocationData();
    }
  }
}
void helpFunc() {
  if (digitalRead(HelpBtn) == 1) {
    steps = 0;
    s = 0;
    helpVal = 1;
  }
}
void SensorData() {
  // Make sure to call update as fast as possible
  pox.update();
  // Asynchronously dump heart rate and oxidation levels to the serial
  // For both, a value of 0 means "invalid"
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    Serial.print("Heart rate:");
    Serial.print(pox.getHeartRate());
    heartBeat = pox.getHeartRate();
    Serial.print("bpm / SpO2:");
    Serial.print(pox.getSpO2());
    oxygen = pox.getSpO2();
    Serial.println("%");
    tsLastReport = millis();
    // Serial.print("Temperature = ");
    // Serial.print(temperature.readTemperatureC());
    // Serial.println(" C");
    bodyTemp = temperature.readTemperatureC();
  }
}

void displayInfo() {
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    longi = String(gps.location.lng(), 6);
    lati = String(gps.location.lat(), 6);
    Serial.print(lati);
    Serial.print("/");
    Serial.print(longi);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.print(F("INVALID"));
  }

  //  Serial.println("");
}
void sendLocationData() {
  gsm.println("AT+CSQ");  // Signal quality check
  d = 0;
  while (d < 20) {
    helpFunc();
    SensorData();
    d++;
    delay(5);
  }
  ShowData();
  gsm.println("AT+CGATT?");  //Attach or Detach from GPRS Support
  d = 0;
  while (d < 20) {
    helpFunc();
    SensorData();
    d++;
    delay(5);
  }
  ShowData();
  gsm.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");  //setting the SAPBR, the connection type is using gprs
  d = 0;
  while (d < 200) {
    helpFunc();
    SensorData();
    d++;
    delay(5);
  }
  ShowData();
  gsm.println("AT+SAPBR=3,1,\"APN\",\"INTERNET\"");  //setting the APN, Access point name string
  d = 0;
  while (d < 800) {
    helpFunc();
    SensorData();
    d++;
    delay(5);
  }
  ShowData();
  gsm.println("AT+SAPBR=1,1");  //setting the SAPBR
  d = 0;
  while (d < 400) {
    helpFunc();
    SensorData();
    d++;
    delay(5);
  }
  ShowData();
  gsm.println("AT+HTTPINIT");  //init the HTTP request
  d = 0;
  while (d < 400) {
    helpFunc();
    SensorData();
    d++;
    delay(5);
  }
  ShowData();
  String url = API;
  url += "?serialNumber=";
  url += serialNumber;
  url += "&long=";
  url += longi;
  url += "&lat=";
  url += lati;
  url += "&heart=";
  url += heartBeat;
  url += "&oxygen=";
  url += oxygen;
  url += "&temp=";
  url += bodyTemp;
  url += "&help=";
  url += helpVal;
  Serial.println(url);
  gsm.println("AT+HTTPPARA=\"URL\"," + url);

  d = 0;
  while (d < 200) {
    helpFunc();
    SensorData();
    d++;
    delay(5);
  }
  ShowData();
  gsm.println("AT+HTTPACTION=0");  //submit the request
  d = 0;
  while (d < 2000) {
    helpFunc();
    SensorData();
    d++;
    delay(5);
  }
  ShowData();
  gsm.println("AT+HTTPREAD");  // read the data from the website you access
  d = 0;
  while (d < 20) {
    helpFunc();
    SensorData();
    d++;
    delay(5);
  }
  helpVal = 0;
  ShowData();
}
void ShowData() {
  while (gsm.available() != 0)
    Serial.write(char(gsm.read()));
}