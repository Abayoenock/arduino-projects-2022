
//  the libries for wifi
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
// arduino json library
#include <ArduinoJson.h>
// the network name and password
const char* ssid = "BLOCK2";
const char* password = "Admin@123";
//Your Domain name with URL path or IP address with path
const char* serverName = "http://labsec.enlabs.rw/api/";
int addr = 0;
int doorStatus = 1;

#define rxPin D3
#define txPin D4
SoftwareSerial sim800L(rxPin, txPin);

int M1 = D2;  // MOTOR PIN1-ON-D7 of NodeMCU
int M2 = D6;  // MOTOR PIN2-ON-D8 of NodeMCUz
int PIRsensor = D8;
int Buzzer = D1;
char data;
int motion = 0;


void setup() {
  sim800L.begin(9600);
  Serial.begin(9600);
  pinMode(PIRsensor, INPUT);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  Serial.println("Initializing...");
  sim800L.println("AT");
  delay(1000);
  sim800L.println("AT+CMGF=1");
  delay(1000);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");


  Serial.print(WiFi.localIP());
  Serial.println("/");
  fetchData();
}
uint32_t x = 0;

void loop() {

  while (sim800L.available()) {
    Serial.println(sim800L.readString());
  }

  if (Serial.available()) /* If data is available on serial port */
  {

    data = Serial.read(); /* Data received from bluetooth */
    if (data == 'A') {

      // open the door
      Door_open();
      Serial.write("DOOR IS OPEN\n");
    } else if (data == 'B') {
      // close the door
      Door_close();
      Serial.write("DOOR IS CLOSED\n");
    }
  }


  if (digitalRead(PIRsensor) == HIGH) {

    motion = 1;
  } else {
    motion = 0;
  }

  getData();

  delay(100);
  Serial.println("");
}


void getData() {
  //Send an HTTP POST request every define interval

  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    // Your Domain name

    http.begin(client, serverName);
    //content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String httpRequestData = "motion=" + String(motion);
    // Send HTTP POST request
    Serial.println(httpRequestData);
    int httpResponseCode = http.POST(httpRequestData);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    if (httpResponseCode == 200) {
      String payload = http.getString();
      Serial.println(payload);
      StaticJsonDocument<356> doc;

      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      String mode = doc["mode"];      // "1"
      String cmd = doc["cmd"];        // "1"
      String motion = doc["motion"];  // "1"
      int time = doc["time"];         // 0
      String sms = doc["sms"];        // "1"
      String door = doc["door"];      // "1"
      String buzzer = doc["buzzer"];  // "1"

      String phones_0 = doc["phones"][0];  // "0787078404"
      String phones_1 = doc["phones"][1];  // "0786135953"
      int arraySize = doc["phones"].size();

      if (mode.toInt() == 1) {
        if (time == 1) {
         
          if (buzzer.toInt() == 1 && motion.toInt() == 1) {
            digitalWrite(Buzzer, HIGH);
          } else {
            digitalWrite(Buzzer, LOW);
          }
          if (door.toInt() == 1 && motion.toInt() == 1 && doorStatus == 0) {
            Door_close();
          }
          if (door.toInt() == 0) {
            if (cmd.toInt() != doorStatus) {
              doorStatus == 0 ? Door_close() : Door_open();
            }
          }
           if (sms.toInt() == 1 && motion.toInt() == 1) {
            for (int i = 0; i < arraySize; i++) {

              String phoneNumber = doc["phones"][i];
              Serial.println(phoneNumber);
              send_sms("Intruder detected please check ", phoneNumber);
              delay(1000);
            }
          }
        }
      } else {
       
        if (buzzer.toInt() == 1 && motion.toInt() == 1) {
          digitalWrite(Buzzer, HIGH);
        } else {
          digitalWrite(Buzzer, LOW);
        }
        if (door.toInt() == 1 && motion.toInt() == 1 && doorStatus == 0) {
          Door_close();
        }
        if (door.toInt() == 0) {
          if (cmd.toInt() != doorStatus) {
            doorStatus == 0 ? Door_close() : Door_open();
          }
        }
         if (sms.toInt() == 1 && motion.toInt() == 1) {
          for (int i = 0; i < arraySize; i++) {

            String phoneNumber = doc["phones"][i];
            Serial.println(phoneNumber);
            send_sms("Intruder detected please check ", phoneNumber);
            delay(2000);
          }
        }
      }






    } else {
      // when the request was not received by the server
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void fetchData() {
  //Send an HTTP POST request every define interval

  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    // Your Domain name

    http.begin(client, serverName);
    //content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String httpRequestData = "motion=" + String(motion);
    // Send HTTP POST request
    Serial.println(httpRequestData);
    int httpResponseCode = http.POST(httpRequestData);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    if (httpResponseCode == 200) {
      String payload = http.getString();
      Serial.println(payload);
      StaticJsonDocument<356> doc;

      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      String cmd = doc["cmd"];  // "1"
      cmd.toInt() == 0 ? Door_open() : Door_close();
    } else {
      // when the request was not received by the server
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}


void send_sms(String text, String phone) {
  Serial.println("sending sms....");
  delay(50);
  sim800L.print("AT+CMGF=1\r");
  delay(1000);
  sim800L.print("AT+CMGS=\"" + phone + "\"\r");
  delay(1000);
  sim800L.print(text);
  delay(100);
  sim800L.write(0x1A);  //ascii code for ctrl-26 //Serial2.println((char)26); //ascii code for ctrl-26
  delay(5000);          // five seconds to resend
}

void Door_open() {
  doorStatus = 0;
  digitalWrite(M1, HIGH);
  digitalWrite(M2, LOW);
  delay(2000);
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  Serial.write("DOOR IS OPEN\n");
}
void Door_close() {
  doorStatus = 1;
  digitalWrite(M1, LOW);   // set pin 2 on L293D low
  digitalWrite(M2, HIGH);  // set pin 7 on L293D high
  Serial.write("DOOR IS CLOSED\n");
  delay(2000);
  digitalWrite(M1, LOW);  // set pin 2 on L293D low
  digitalWrite(M2, LOW);  // set pin 7 on L293D low
}
