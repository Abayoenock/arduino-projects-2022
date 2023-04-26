// rtc libraries
#include "RTClib.h"
// nokia display libraries
#include <Nokia_LCD.h>
#include "Bold_LCD_Fonts.h"
#include "Glyphs_LCD_Fonts.h"
#include "Small_LCD_Fonts.h"

// dht libraries
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <EEPROM.h>
// the address memories on the EEPROME
int yearAddress = 0, monthAddress = 1, dayAddress = 2, hourAddress = 3, minAddress = 4;
#define DHTPIN 2       // Digital pin where the dth11 is connected
#define DHTTYPE DHT11  // DHT 11
DHT_Unified dht(DHTPIN, DHTTYPE);

#define maxHumidity 70       // this value can be changed according to the desired maximum himidity
#define minHumidity 50       //->change this value to the desired minimun humidity
#define maxAmmoniaLevel 120  // change this value according to the value of ammonia
int currentDay = 0;          // hold the days since last reset

uint32_t delayMS;
int humidityValue = 0;       // stores the value of humidity
float temperatureValue = 0;  // stores the value of temperature
//update variable in millis
unsigned int lastUpdateValue = 0;
unsigned int lastUpdateScreen = 0;
unsigned int lastMillisUpdate=0; 

//Rtc gloabal
RTC_DS3231 rtc;
int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
// Nokia diplay globals

Nokia_LCD lcd(13 /* CLK */, 12 /* DIN */, 11 /* DC */, 10 /* CE */, 9 /* RST */,
              9 /* BL */);
const LcdFont BoldFont{
  [](char c) {
    return Bold_LCD_Fonts::kFont_Table[c - 0x20];
  },                                       // method to retrieve the character
  Bold_LCD_Fonts::kColumns_per_character,  // width of each char
  Bold_LCD_Fonts::hSpace,                  // horizontal spacing array
  1                                        // size of horizontal spacing array
};

const unsigned char TOOLBAR[] = {

  0xD0,  // D0 degree symbol
  0x25,  // 25 %
  0x0F,  // 0D hourglass
  0xF6,  //F6 checkbox, not selected
  0xF7,  // F7 checkbox, selected

};

//relay control pins
int coolingFan = 3, exaustFan = 4, humidifier = 5, heater = 6;
// the input push button to set the start date this will help in changing the temperature
int inputPush = 7;
// the variable to hold the values of the ammonia sensor
int sensorValue = 0;
#define MaxAmmoniaLevel 200  // change this level to the desired level 


int maxTemperature = 32;
int minTemperature = 30;
void setup() {
  Serial.begin(9600);
  // Initialize device.
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);

  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
  //-------------------------RTC-----------------------

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");

    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  // Initialize the screen
  lcd.begin();
  // Turn on the backlight
  lcd.setBacklight(true);
  // Set the contrast
  lcd.setContrast(60);  // Good values are usualy between 40 and 60
  lcd.clear();
  lcd.setFont(&Glyphs_LCD_Fonts::GlyphFont);
  lcd.setCursor(0, 1);

  lcd.print(" Smart pautry ");
  lcd.setCursor(0, 3);
  lcd.print(" farming ");
  delay(3000);
  lcd.clear();



  // define what the pins will be used for
  pinMode(coolingFan, OUTPUT);
  pinMode(exaustFan, OUTPUT);
  pinMode(heater, OUTPUT);
  pinMode(humidifier, OUTPUT);
  pinMode(inputPush, INPUT);
  //put the pins HIGH as the relay are low state triggered
  digitalWrite(coolingFan, HIGH);
  digitalWrite(heater, HIGH);
  digitalWrite(humidifier, HIGH);
  digitalWrite(exaustFan, HIGH);
}

void loop() {

  TemeperatureHumidity();
  humidityControl();
  currentTimedayCalculator();
  temperatureControl(maxTemperature, minTemperature);
  checkIputPush();
  getAmmonia();
  displayInformation();
}
//
void displayInformation() {
  if (millis() - lastUpdateScreen >= 2000) {
    lcd.clear();
    lcd.setFont(&Glyphs_LCD_Fonts::GlyphFont);
    lcd.print((char)TOOLBAR[2]);
    lcd.setFont(&Small_LCD_Fonts::SmallFont);
    lcd.print(" ");
    lcd.print(day);
    lcd.print("/");
    lcd.print(month);
    lcd.print("/");
    lcd.print(year);
    lcd.print(" ");
    lcd.print(hour);
    lcd.print(":");
    lcd.print(minute);
    lcd.setCursor(0, 2);
    lcd.setFont(&Small_LCD_Fonts::SmallFont);
    lcd.print("Humidity");
    lcd.setCursor(55, 2);
    lcd.print("Temp ");

    lcd.setCursor(0, 3);

    lcd.setFont(&Glyphs_LCD_Fonts::GlyphFont);
    lcd.print(humidityValue);

    lcd.print((char)TOOLBAR[1]);
    lcd.setCursor(55, 3);
    lcd.print(round(temperatureValue));
    lcd.print((char)TOOLBAR[0]);
    lcd.print("C");
    lcd.setCursor(0, 5);
    lcd.print("AML:");
    lcd.print(sensorValue);
    lcd.setCursor(55, 5);
    lcd.print("D:");
    lcd.print(currentDay + 1);
    lastUpdateScreen = millis();
  }
}

void checkIputPush() {  // the function to reset the day to the start day
  if (digitalRead(inputPush) == HIGH) {

    lcd.clear();
    lcd.setFont(&Glyphs_LCD_Fonts::GlyphFont);
    lcd.setCursor(10, 1);
    lcd.print((char)TOOLBAR[3]);
    lcd.setFont(&Small_LCD_Fonts::SmallFont);
    lcd.print(" Long press for");
    lcd.setCursor(10, 2);
    lcd.print(" 3 seconds to ");
    lcd.setCursor(10, 3);
    lcd.print(" reset the ");
    lcd.setCursor(10, 4);
    lcd.print("  starting day");
    delay(3000);
    if (digitalRead(inputPush) == HIGH) {  // if the user waits  for 3 second to confirm the reset
      lcd.clear();
      lcd.setCursor(10, 1);
      lcd.setFont(&Glyphs_LCD_Fonts::GlyphFont);
      lcd.print((char)TOOLBAR[4]);
      lcd.setFont(&Small_LCD_Fonts::SmallFont);
      lcd.print(" Reset was");
      lcd.setCursor(10, 2);
      lcd.print(" successful");
      setStartDate();  // THE FUNCTION TO STORE THE CURRENT DATE AND TIME IN THE EEPROM
      delay(3000);
    } else {
      lcd.clear();
      lcd.setCursor(10, 1);
      lcd.setFont(&Glyphs_LCD_Fonts::GlyphFont);
      lcd.print((char)TOOLBAR[4]);
      lcd.setFont(&Small_LCD_Fonts::SmallFont);
      lcd.print(" Reset was");
      lcd.setCursor(10, 2);
      lcd.print(" cancelled");
      delay(3000);
    }


    Serial.println("the time has been reset");
  }
}
void setStartDate() {
  /*
  this function is called when the user sets the start day by a long press on the button
  this helps in knowing the number of elapsed dayz , by storing the start date in the EEPROM
  */
  DateTime currentTimeSet = rtc.now();
  int yearSet = (currentTimeSet.year() - 2000);
  int monthSet = (currentTimeSet.month());
  int daySet = (currentTimeSet.day());
  int hourSet = (currentTimeSet.hour());
  int minuteSet = (currentTimeSet.minute());
  int secondSet = (currentTimeSet.second());
  EEPROM.update(yearAddress, yearSet);
  EEPROM.update(monthAddress, monthSet);
  EEPROM.update(dayAddress, daySet);
  EEPROM.update(hourAddress, hourSet);
  EEPROM.update(minAddress, minuteSet);
}

void currentTimedayCalculator() {
  DateTime now = rtc.now();
  year = now.year();
  month = now.month();
  day = now.day();
  String dayOfTheWeek = daysOfTheWeek[now.dayOfTheWeek()];
  hour = now.hour();
  minute = now.minute();
  second = now.second();

  int readYear = EEPROM.read(yearAddress);
  int readMonth = EEPROM.read(monthAddress);
  int readDay = EEPROM.read(dayAddress);
  int readHour = EEPROM.read(hourAddress);
  int readMin = EEPROM.read(minAddress);
  DateTime now2 = rtc.now();
  DateTime dt0(readYear, readMonth, readDay, readHour, readMin, 0);
  TimeSpan ts = now2 - dt0;
  Serial.print(" passed days since the set time   ");
  Serial.print(" ");
  Serial.print(ts.days(), DEC);

  Serial.print(" days ");
  Serial.print(ts.hours(), DEC);
  Serial.print(" hours ");
  Serial.print(ts.minutes(), DEC);
  Serial.print(" minutes ");
  Serial.print(ts.seconds(), DEC);
  Serial.print(" seconds (");

  Serial.println();
  Serial.println();

  // set temperature according to dayz
  SetTemperature(ts.days());


  Serial.print("Temperature: ");
  Serial.print(rtc.getTemperature());
  Serial.println(" C");

  Serial.println();
  delay(3000);
}

void getAmmonia() {
  sensorValue = analogRead(0);  // read analog input pin 0
  if (sensorValue > MaxAmmoniaLevel) {
    if (millis() - lastMillisUpdate >= 6000) {
      int i = 0;
      while (i < 2) {
        lcd.clear();
        lcd.setCursor(10, 2);
        lcd.setFont(&Small_LCD_Fonts::SmallFont);
        lcd.print(" Warning");
        lcd.setCursor(10, 3);
        lcd.print(" Ammonia level");
        lcd.setCursor(10, 4);
        lcd.print(" Is high ");
        delay(2000);
        i++;
      }
      lastMillisUpdate =millis();
    }

  } else {
  }

  Serial.println(sensorValue, DEC);  // prints the value read
}


void SetTemperature(int dayz) {  // the function to set the correct temp aacording to the number of days
  currentDay = dayz;
  if (dayz < 1) {
    maxTemperature = 32;
    minTemperature = 30;
  }
  if (dayz < 1) {
    maxTemperature = 34;
    minTemperature = 32;
  }
  if (dayz > 1 && dayz <= 7) {
    maxTemperature = 30;
    minTemperature = 30;
  }
  if (dayz > 7 && dayz <= 14) {
    maxTemperature = 26;
    minTemperature = 26;
  }
  if (dayz > 14 && dayz <= 21) {
    maxTemperature = 22;
    minTemperature = 22;
  }
  if (dayz > 21) {
    maxTemperature = 20;
    minTemperature = 20;
  }
}


void humidityControl() {
  if (!(humidityValue >= 50 && humidityValue <= 70)) {
    if (humidityValue <= 50) {
      // turn on the humidifier
      digitalWrite(humidifier, LOW);
      digitalWrite(exaustFan, HIGH);



    } else if (humidityValue >= 70) {
      // turn on the exaust fan
      digitalWrite(humidifier, HIGH);
      digitalWrite(exaustFan, LOW);
      // digitalWrite(heater, LOW);
    }

  } else {
    // this runs when the humidity is in the range
    digitalWrite(humidifier, HIGH);
    digitalWrite(exaustFan, HIGH);
    // digitalWrite(heater, HIGH);
  }
}
void temperatureControl(int maxTemp, int minTemp) {
  if (!(temperatureValue >= minTemp && temperatureValue <= maxTemp)) {
    if (temperatureValue <= minTemp) {
      // turn on the heater
      digitalWrite(heater, LOW);
      digitalWrite(coolingFan, HIGH);


    } else if (temperatureValue >= maxTemp) {
      // turn off the heater
      digitalWrite(heater, HIGH);
      digitalWrite(coolingFan, LOW);
    }
  } else {
    // turn off the heater
    digitalWrite(heater, HIGH);
    digitalWrite(coolingFan, HIGH);
  }
}

void TemeperatureHumidity() {
  // Delay between measurements.
  if (millis() - lastUpdateValue >= delayMS) {
    // Get temperature event and print its value.
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println(F("Error reading temperature!"));
    } else {
      Serial.print(F("Temperature: "));
      Serial.print(event.temperature);
      Serial.println(F("°C"));
      // assign the value to the variable
      temperatureValue = event.temperature;
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println(F("Error reading humidity!"));
    } else {
      Serial.print(F("Humidity: "));
      Serial.print(event.relative_humidity);
      Serial.println(F("%"));
      humidityValue = event.relative_humidity;
    }
    lastUpdateValue = millis();
  }
}