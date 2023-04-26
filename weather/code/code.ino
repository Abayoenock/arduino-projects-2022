//BMP280 libraries
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
//DH11  Libraries
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// wifi libraies
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
//esp8266 globals
const char* ssid = "method";// change this to your wireless network
const char* password = "kigali50";// change this to your password network
//Your Domain name with URL path or IP address with path
const char* serverName = "http://weatherapp.enlabs.rw/api/";

//DTH11 Globals
#define DHTPIN D5  // Digital pin connected to the DHT sensor
//the pins to connect the leds
#define ledSuccess D6
#define ledError D7
#define DHTTYPE DHT11  // DHT 11
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
unsigned int lastDHT_Update = 0;
// send data to cloud glabals
unsigned int lastTime = 0;
#define SendDataInterval 2000

//BMP280 globals
Adafruit_BMP280 bmp;  // I2C

//rain sensor globals
#define rainfall A0  // the pin where the rain sensor is connected
int rainSensorValue;
int set = 200;  //set the threeshold rainSensorValue
// global data variables
float tempData = 0;
int humidityData = 0;
int rainData = 0;
float pressureData = 0;
float altitudeData = 0;



void BMP280_SetUp() {
  while (!Serial) delay(100);  // wait for native usb
  Serial.println(F("BMP280 test"));
  unsigned status;
  status = bmp.begin();
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                     "try a different address!"));
    Serial.print("SensorID was: 0x");
    Serial.println(bmp.sensorID(), 16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
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
  leds(0, 1);
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
  leds(1, 0);
}

void setup() {
  Serial.begin(9600);
  pinMode(rainfall, INPUT);  // define the rain sensor as an input
  pinMode(ledError, OUTPUT);
  pinMode(ledSuccess, OUTPUT);
  //BMP280 setup
  BMP280_SetUp();  // call to defined function to setup the BMP280 sensor
  DTH11_SetUp();   // call to defiend function to setup the DTH11 sensor
  WiFi_SetUp();    // call of define function to set up the wifi of the esp8266
  delay(50);
}


void loop() {
  rainSensorFunction();   //detect the presence or absence of rain
  BMP280_SensorValues();  // get the pressure,aprroximate latitude
  DTH11_SensorValues();   // get the temperature and humidity
  sendData();             //send data to the cloud
  delay(10);
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


void leds(int success, int errorLd) {
  digitalWrite(ledSuccess, success);
  digitalWrite(ledError, errorLd);
}

void BMP280_SensorValues() {
  Serial.print(F("Temperature = "));
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  Serial.print(F("Pressure = "));
  Serial.print(bmp.readPressure());
  pressureData = bmp.readPressure();
  Serial.println(" Pa");

  Serial.print(F("Approx altitude = "));
  Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
  altitudeData = bmp.readAltitude(1013.25);
  Serial.println(" m");
  Serial.println();
}



void rainSensorFunction() {
  rainSensorValue = analogRead(rainfall);
  Serial.println(rainSensorValue);

  if (rainSensorValue < 1000) {  // compare the thresold value to the sensor value

    rainData = 1;
    Serial.println("rain detected");

  }

  else {
    rainData = 0;
    Serial.println("No rain detected ");
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
  firname="mamany";
  lastname="jose";
  fullname=firtanama+" "+lastanme
      //content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "tempData=" + String(tempData) + "&humidityData=" + String(humidityData) + "&pressureData=" + String(pressureData) + "&altitudeData=" + String(altitudeData) + "&rainData=" + String(rainData);
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      if(httpResponseCode==200){
        leds(1, 0);
      }
      else{
        leds(0, 1);
      }
      

      http.end();
    } else {
      Serial.println("WiFi Disconnected");
      leds(0, 1);
    }
    lastTime = millis();
  }
}