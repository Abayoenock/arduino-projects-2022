//DH11  Libraries
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
// arduino json library
#include <ArduinoJson.h>

// wifi libraies
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
// LCD libraris
#include <LCD_I2C.h>
//lcd globals
LCD_I2C lcd(0x27, 16, 2);
unsigned int lastLCD_update = 0;
#define displayInterval 2000
//esp8266 globals
const char* ssid = "Info";         // change this to your wireless network
const char* password = "enlabs1245";  // change this to your password network
//Your Domain name with URL path or IP address with path
const char* serverName = "http://192.168.43.196/coldRoom/api/";

//DTH11 Globals
#define DHTPIN D5  // Digital pin connected to the DHT sensor
//the pins to connect the buzzer
#define buzzer D6

#define DHTTYPE DHT11  // DHT 11
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
unsigned int lastDHT_Update = 0;
// send data to cloud glabals
unsigned int lastTime = 0;
#define SendDataInterval 2000
// the pin where the magnetic reed is connected
#define DoorSensor D0
// global data variables
float tempData = 0;
int humidityData = 0;
int voltageData = 0;
int doorData = 0;



void LCD_SetUp() {
  lcd.begin();
  lcd.backlight();
}



void DTH11_SetUp() {
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
}

void WiFi_SetUp() {


  lcd.print("Connecting to ");  // You can make spaces using well... spaces
  lcd.setCursor(0, 1);          // Or setting the cursor in the desired position.
  lcd.print("Network .....");
  delay(2000);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi connected ");
  lcd.setCursor(0, 1);
  lcd.print("Successfuly  ");
}

void setup() {
  Serial.begin(9600);
  pinMode(DoorSensor, INPUT);  // define the rain sensor as an input
  pinMode(A0,INPUT);
  pinMode(buzzer,OUTPUT);

  LCD_SetUp();    // call of function to setup the lcd
  DTH11_SetUp();  // call to defiend function to setup the DTH11 sensor
  WiFi_SetUp();   // call of define function to set up the wifi of the esp8266

  delay(50);
}


void loop() {


  DTH11_SensorValues();  // get the temperature and humidity
  doorValues();
  lcdDisplay();
  currentSensor();
  sendData();  //send data to the cloud

  delay(10);
}
void lcdDisplay() {  // the function to display the data to the lcd
  if (millis() - lastLCD_update >= displayInterval) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T: ");
    lcd.print(tempData);
    lcd.print("C  H:");
    lcd.print(humidityData);
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print("D: ");
    if (doorData == 1) {
      lcd.print("Open  P:");
      if(voltageData==1){
         lcd.print(" ON");
      }
      else{
         lcd.print("OFF");
      }
    } else {
      lcd.print("Closed  P:");
       if(voltageData==1){
         lcd.print(" ON");
      }
      else{
         lcd.print("OFF");
      }
    }

    lastLCD_update = millis();
  }
}
void doorValues() {
  if (digitalRead(DoorSensor) == HIGH) {
    Serial.println("door open");
    doorData = 0;
  } else {
    doorData = 1;
  }
}

void DTH11_SensorValues() {
  // Delay between measurements.
  delay(delayMS);
  if (millis() - lastDHT_Update >= delayMS) {
    // Get temperature event and print its value.
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println(F("Error reading temperature!"));
    } else {
      Serial.print(F("Temperature: "));
      tempData = event.temperature;
      Serial.print(event.temperature);
      Serial.println(F("°C"));
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println(F("Error reading humidity!"));
    } else {
      Serial.print(F("Humidity: "));
      humidityData = event.relative_humidity;
      Serial.print(event.relative_humidity);
      Serial.println(F("%"));
    }
    lastDHT_Update = millis();
  }
}










void sendData() {
  //Send an HTTP POST request every define interval
  if ((millis() - lastTime) > SendDataInterval) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      // Your Domain name
      http.begin(client, serverName);
      //content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "tempData=" + String(tempData) + "&humidityData=" + String(humidityData) + "&doorData=" + String(doorData) + "&voltageData=" + String(voltageData);
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
      if (httpResponseCode == 200) {
        // Stream& input;

        StaticJsonDocument<120> doc;

        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }

        int overTemp = doc["overTemp"];          // 0
        int lessTemp = doc["lessTemp"];          // 0
        int overHimidity = doc["overHimidity"];  // 0
        int lessHumidity = doc["lessHumidity"];  // 0
        if (overTemp == 1 || lessTemp == 1 || overHimidity == 1 || lessHumidity == 1) {
          tone(buzzer, 450, 500);
          if (overTemp == 1){
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Over");
            lcd.setCursor(0, 1);
            lcd.print("Temperature ");
            delay(1000);
          }
          if (overHimidity == 1) {
             tone(buzzer, 450, 500);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Over");
            lcd.setCursor(0, 1);
            lcd.print("Humidity ");
            delay(1000);
          }
          if (lessHumidity == 1) {
            digitalWrite(buzzer,HIGH);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Less");
            lcd.setCursor(0, 1);
            lcd.print("Humidity ");
            delay(1000);
            digitalWrite(buzzer,LOW);
          }
          if (lessTemp == 1) {
             tone(buzzer, 450, 500);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Less");
            lcd.setCursor(0, 1);
            lcd.print("Temperature ");
            delay(1000);
          }
        } else {
          tone(buzzer, 0, 0);
        }

      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Lost Server....");
        delay(1000);
      }


      http.end();
    } else {
      Serial.println("WiFi Disconnected");

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Wifi disconnected");
      delay(1000);
    }
    lastTime = millis();
  }
}
void currentSensor() {
  unsigned int x = 0;
  float AcsValue = 0.0, Samples = 0.0, AvgAcs = 0.0, AcsValueF = 0.0;
  for (int x = 0; x < 150; x++) {  //Get 150 samples
    AcsValue = analogRead(A0);     //Read current sensor values
    Samples = Samples + AcsValue;  //Add samples together
    delay(3);                      // let ADC settle before next sample 3ms
  }
  AvgAcs = Samples / 150.0;  //Taking Average of Samples

  //((AvgAcs * (5.0 / 1024.0)) is converitng the read voltage in 0-5 volts
  //2.5 is offset(I assumed that arduino is working on 5v so the viout at no current comes
  //out to be 2.5 which is out offset. If your arduino is working on different voltage than
  //you must change the offset according to the input voltage)
  //0.185v(185mV) is rise in output voltage when 1A current flows at input
  AcsValueF = (3.69 - (AvgAcs * (5.12 / 1024.0))) / 0.185;
  Serial.println(AcsValueF);  //Print the read current on Serial monitor
  delay(50);
  if(AcsValueF<=-7){
    voltageData=0;
  }
  else{
    voltageData=1;
  }
}