#include <SoftwareSerial.h>  // to use the serial monitor
//  the libries for wifi
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
// arduino json library
#include <ArduinoJson.h>
//esp8266 globals
// The libraies for the lcd
#include <LCD_I2C.h>
//lcd globals
LCD_I2C lcd(0x27, 16, 2);  // Default address of most PCF8574 modules, change according

const char* ssid = "ABAYO";           // change this to your wireless network
const char* password = "enlabs1234";  // change this to your password network
//Your Domain name with URL path or IP address with path
const char* serverName = "http://192.168.1.196/health-new/API/";
const char* serverNameGet = "http://192.168.1.196/health-new/API/?task=get";  // to get the user information
// to difine the pins used in serial communication
SoftwareSerial arduinoData(D7, D6);  // to make the digital pins into TX and RX respectively
// global variables
String cardId = "";
String cardIdRecorded = "";
float bodyTemp = 0;
int oxygen = 0;
int heartBeat = 0;
unsigned int lastDisplay = 0;
int heartBeatRecorded = 0;
int oxygenRecorded = 0;
float bodyTempRecorded = 0;
bool breakLoop = false;

// define the buzzer pin
#define buzzer D5

#define inputPushButton D0  // the pin where the push button is connectd to

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
  Serial.begin(9600);
  arduinoData.begin(115200);  // to initialize the baurd rate for the software serail pins
                              // to setup the lcd display
  lcd.begin();                // If you are using more I2C devices using the Wire library use lcd.begin(false)
  lcd.backlight();
  WiFi_SetUp();
  pinMode(inputPushButton, INPUT);
  pinMode(buzzer, OUTPUT);
}
void loop() {
  datahandler(0);  // call of the fuction to receieve and handle the data from the arduino
  if (millis() - lastDisplay >= 2000) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tap your card");
    lcd.setCursor(0, 1);
    lcd.print("To continue");
    lastDisplay = millis();
  }
  delay(10);
}
void datahandler(int step) {
  String datas = "";
  while (arduinoData.available() != 0) {
    datas = datas + String(char(arduinoData.read()));
  }
  if (datas != "") {
    cardId = "";
    Serial.println(datas);
    String input = datas;
    StaticJsonDocument<200> doc;
    DeserializationError err = deserializeJson(doc, input);
    if (err) {
      Serial.print("ERROR:");
      Serial.print(err.c_str());
      return;
    } else {
      String cardIdData = doc["card"];
      float bodyTemp2 = doc["temp"];
      int oxygen2 = doc["oxygen"];
      int heartBeat2 = doc["heatBeat"];
      //add an offset value to make the measurements right
      bodyTemp = bodyTemp2 + 3.2;
      oxygen = oxygen2;
      heartBeat = heartBeat2;
      cardId = cardIdData;
      Serial.println(bodyTemp);
      Serial.println(oxygen);
      Serial.println(heartBeat);
      if (cardId.length() > 5) {
        buzzerSound(1, 500);

        if (step == 1) {
          if (cardId != cardIdRecorded) {

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("The placed card ");
            lcd.setCursor(0, 1);
            lcd.print("do not match");
            breakLoop = true;
            buzzerSound(3, 500);
            delay(1500);
          } else {
            sendData(2);
          }

        } else {
          cardIdRecorded = cardId;
          sendData(1);
        }
        delay(2000);
      } else {
      }
    }
  }
}
void ReadtempAndHeart() {
  String datas = "";
  while (arduinoData.available() != 0) {
    datas = datas + String(char(arduinoData.read()));
  }
  if (datas != "") {
    Serial.println(datas);
    String input = datas;
    StaticJsonDocument<200> doc;
    DeserializationError err = deserializeJson(doc, input);
    if (err) {
      Serial.print("ERROR:");
      Serial.print(err.c_str());
      return;
    }
    float bodyTemp2 = doc["temp"];
    int oxygen2 = doc["oxygen"];
    int heartBeat2 = doc["heatBeat"];
    bodyTemp = bodyTemp2 + 3.2;
    oxygen = oxygen2;
    heartBeat = heartBeat2;
  }
}
void sendData(int step) {
  //Send an HTTP POST request every define interval

  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    // Your Domain name
    if (step == 1) {
      http.begin(client, serverNameGet);
      //content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "card=" + cardId;
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

        int userExists = doc["userExists"];  // 1
        const char* name = doc["name"];      // "abayo"
        if (userExists == 1) {
          Serial.print("Welcome ");
          Serial.println(name);
          // print on the lcd the welcome message

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Welcome ");
          lcd.setCursor(0, 1);
          lcd.print(name);
          delay(3000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Take your heart ");
          lcd.setCursor(0, 1);
          lcd.print("tests");
          unsigned int dataTimeDelay = millis();
          while (1) {
            ReadtempAndHeart();
            if (digitalRead(inputPushButton) == HIGH) {
              oxygenRecorded = oxygen;
              heartBeatRecorded = heartBeat;
              break;
            }
            if (millis() - dataTimeDelay >= 30000) {
              break;
            }
          }
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Oxygen: ");
          lcd.print(oxygenRecorded);
          lcd.setCursor(0, 1);
          lcd.print("Heart B: ");
          lcd.print(heartBeatRecorded);
          delay(3000);
          Serial.println(oxygenRecorded);
          Serial.println(heartBeatRecorded);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Take your temp ");
          lcd.setCursor(0, 1);
          lcd.print("tests");
          delay(3000);

          dataTimeDelay = millis();
          while (1) {
            ReadtempAndHeart();
            if (digitalRead(inputPushButton) == HIGH) {
              bodyTempRecorded = bodyTemp;
              break;
            }
            if (millis() - dataTimeDelay >= 30000) {
              break;
            }
          }
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Temperature");
          lcd.setCursor(0, 1);
          lcd.print(bodyTempRecorded);
          delay(3000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Tap your card ");
          lcd.setCursor(0, 1);
          lcd.print("again");
          breakLoop = false;
          while (1) {
            datahandler(1);
            if (breakLoop == true) {
              break;
            }
          }




        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Oooops ! ");
          lcd.setCursor(0, 1);
          lcd.print("user not found !");
          buzzerSound(3, 500);
          Serial.print("user not found ");
        }

        delay(2000);
        cardId = "";

      } else {
        // when the request was not received by the server
      }
    } else {
      http.begin(client, serverName);
      //content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "card=" + cardIdRecorded + "&oxygen=" + String(oxygenRecorded) + "&heartBeat=" + String(heartBeatRecorded) + "&bodyTemp=" + String(bodyTempRecorded);
      // Send HTTP POST request
      Serial.println(httpRequestData);
      int httpResponseCode = http.POST(httpRequestData);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      if (httpResponseCode == 200) {
        String payload = http.getString();
        Serial.println(payload);
        StaticJsonDocument<140> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }

        int inserted = doc["i"];
        int emergency = doc["e"];
        if (inserted != 1) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Data insert  ");
          lcd.setCursor(0, 1);
          lcd.print("Failed");
          buzzerSound(3, 500);

        } else {
          if (emergency == 1) {
            // display back to the user the mergency message
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("You have an  ");
            lcd.setCursor(0, 1);
            lcd.print("Emergency case ");
            buzzerSound(3, 500);

          } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Data recorded  ");

            buzzerSound(1, 500);
          }
        }


        breakLoop = true;






      } else {
        // when the device looses connection to the server
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("  Server connection");
        lcd.setCursor(0, 1);
        lcd.print("lost ");
        breakLoop = true;
      }
    }




    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void buzzerSound(int times, int delayMs) {
  int z = 0;
  while (z < times) {
    digitalWrite(buzzer, HIGH);
    delay(delayMs);
    digitalWrite(buzzer, LOW);
    delay(delayMs);
    z++;
  }
}