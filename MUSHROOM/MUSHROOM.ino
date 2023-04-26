#include <DHT.h>
#define DHTPIN 7       // what pin we're connected to
#define DHTTYPE DHT11  // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);
#include <LCD_I2C.h>
unsigned int lastDisplayUpdate =0;
LCD_I2C lcd(0x27, 16, 2);  // Default address of most PCF8574 modules, change according
int fan = 8;
int humidifier=9;
int h =0;
  int t = 0;
void setup() {
  pinMode(fan, OUTPUT);
  pinMode(humidifier, OUTPUT);
  lcd.begin();
  lcd.backlight();
  Serial.begin(9600);
  Serial.println("DHTxx test!");
  dht.begin();
}
void loop() {
   h = dht.readHumidity();
   t = dht.readTemperature();
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    
  if (t > 28) {
    digitalWrite(fan, HIGH);
  }
  else if (t < 26) {
   digitalWrite(fan, LOW);
  }
 if (h >=90) {
    digitalWrite(humidifier, LOW);
  }
  else if(h<80)  {
    digitalWrite(humidifier, HIGH);
  }
  }


display();
  
}
void display() {
  if (millis() - lastDisplayUpdate >= 1000) {
    lcd.setCursor(0, 0);
    lcd.print("Humidity:");
    lcd.print(h);
    Serial.println(h);
    Serial.println("h");
    lcd.print(" %");
    lcd.setCursor(0, 1);
    lcd.print("Temper:");
    lcd.print(t);
    Serial.println(t);
    Serial.println("t");
    lcd.print(" C");
    lastDisplayUpdate = millis();
  }
}
