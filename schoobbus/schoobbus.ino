/*
  RFID    ESP8266
  VCC====> 3V
  GND=====>GND
  RST====>D3
  SDA=====>D4
  SCL=====>D5
  MOSI=====>D7
  MISO=====>D6
  IRQ ===> Nta pin uyicomekaho uyirekeraho ubusa
*/
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SPI.h>
#include <RFID.h>
int busID = 1;// this is replace by the bus id 

const char* ssid = "admin";                                        // network name
const char* password = "admin123";                                 // network password
const char* host = "http://sbm.enlabs.rw/api/";  // chnage the ip to your own ip address =>ipconfig

#define RST_PIN 0  // Configurable, see typical pin layout above
#define SS_PIN 2   // Configurable, see typical pin layout above
HTTPClient http;
WiFiClient WiFiclient;
LiquidCrystal_I2C lcd(0x27, 16, 2);
int ledGreen = D1;
int ledLed = D2;

RFID rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance
unsigned int lasttime = 0;
int serNum0;
int serNum1;
int serNum2;
int serNum3;
int serNum4;
String card_num = "";
unsigned int last_millis = 0;
int buzzer_pin = 16;
int buzzer_times = 0;
void setup() {
  Serial.begin(9600);  // Initialize serial communications with the PC
  SPI.begin();
  rfid.init();
pinMode(ledLed,OUTPUT);
pinMode(ledGreen,OUTPUT);
  Serial.print("Connecting to ");
  Serial.println(ssid);


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

  card_num = "";
  unsigned int currenttime = millis();
  if (currenttime - lasttime >= 10000) {
    /* to initialise the variable back to zero after 2 seconds or you can
      change it as deisired, this prevents from submitting the card twice since it may temper with results */
    serNum0 = serNum1 = serNum2 = serNum3 = serNum4 = 0;
  }

  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      if (rfid.serNum[0] != serNum0
          && rfid.serNum[1] != serNum1
          && rfid.serNum[2] != serNum2
          && rfid.serNum[3] != serNum3
          && rfid.serNum[4] != serNum4) {
        /* With a new cardnumber, show it. */
        Serial.println(" ");
        Serial.println("Card found");
        serNum0 = rfid.serNum[0];
        serNum1 = rfid.serNum[1];
        serNum2 = rfid.serNum[2];
        serNum3 = rfid.serNum[3];
        serNum4 = rfid.serNum[4];

        card_num += String(rfid.serNum[0], HEX);
        card_num += String(rfid.serNum[1], HEX);
        card_num += String(rfid.serNum[2], HEX);
        card_num += String(rfid.serNum[3], HEX);
        card_num += String(rfid.serNum[4], HEX);
        Serial.println(card_num);
        lasttime = millis();

        send_card_data();  // function to send data to the webserver


      } else {
        /* If we have the same ID, just write a dot. */
      }
    }
  }

  rfid.halt();
}

void send_card_data() {
  String url = host;
  url += "?card=";
  url += card_num;
  url += "&busID=";
  url += busID;
  http.begin(WiFiclient, url);
  http.addHeader("Content-Type", "text/plain");
  int httpCode = http.GET();
  delay(3000);
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
    digitalWrite(ledLed, HIGH);
    delay(1000);
    digitalWrite(ledLed, LOW);
    delay(1000);  
  }
  if(status==0){
    digitalWrite(ledGreen, HIGH);
    delay(1000);
    digitalWrite(ledGreen, LOW);
    delay(1000);  
  }



  http.end();
}
