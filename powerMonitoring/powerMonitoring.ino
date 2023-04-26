#include <Wire.h>
#include <SoftwareSerial.h>
#define blockA A1
#define blockB A2
// send globals values
float current_A = 0.0, current_B = 0.0;
int voltage = 0;
float powerA = 0.0, powerB = 0.0;
float energyA = 0.0, energyB = 0.0;
unsigned int lastDataSent = 0;
unsigned int lastDataSent2 = 0;
unsigned int lastDataSent3 = 0;

//voltage sensor globals
double sensorValue1 = 0;
int val[100];
int max_v = 0;
double VmaxD = 0;
double VeffD = 0;
double Veff = 0;
unsigned int lastMillis = 0;
SoftwareSerial esp(2, 3);  // the pins for arduino communication to esp RX and TX respectively

#define SAMPLES 300  //Number of samples you want to take everytime you loop

float High_peak, Low_peak;  //Variables to measure or calculate
float Amps_Peak_Peak, Amps_RMS;

void setup() {

  Serial.begin(9600);  //Start Serial Monitor to display current read value on Serial monitor
  esp.begin(230400);
}
void loop() {
  currentSensors();
  voltageSensor();
  power();
  energy();
  Serialprints();
  if (millis() - lastDataSent >= 500) {
    SendData(0);
    lastDataSent = millis();
  }
  if (millis() - lastDataSent2 >= 700) {
    SendData(1);
    lastDataSent2 = millis();
  }
  if (millis() - lastDataSent3 >= 900) {
    SendData(2);
    lastDataSent3 = millis();
  }
  delay(1);
}

void SendData(int i) {
  String dataToSend = "";
  if (i == 0) {
    dataToSend += "{\"v\":\"" + String(voltage) + "\",\"A\":\"" + String(current_A) + "\", \"B\":\"" + String(current_B) + "\",\"S\":\"" + String(i) + "\"}";
  } else if (i == 1) {
    dataToSend += "{\"S\":\"" + String(i) + "\",\"E\":\"" + String(energyA) + "\"}";
  } else if (i == 2) {
    dataToSend += "{\"S\":\"" + String(i) + "\",\"F\":\"" + String(energyB) + "\"}";
  }
  Serial.println(dataToSend);
  esp.println(dataToSend);  //print the data to esp in a JSON format so that we can deserialize the data in the esp
}
void power() {
  powerA = powerA + (current_A * voltage);
  powerB = powerB + (current_B * voltage);
}
void energy() {
  energyA = energyA + (((current_A * voltage)) * (((millis() - lastMillis)) / 3600000.0));
  energyB = energyB + (((current_B * voltage)) * (((millis() - lastMillis)) / 3600000.0));
  lastMillis = millis();
}


void read_Amps(int pin)  //read_Amps function calculate the difference between the high peak and low peak
{                        //get peak to peak value
  int cnt;               //Counter
  High_peak = 0;         //We first assume that our high peak is equal to 0 and low peak is 1024, yes inverted
  Low_peak = 1024;

  for (cnt = 0; cnt < SAMPLES; cnt++)  //everytime a sample (module value) is taken it will go through test
  {
    float ACS_Value = analogRead(pin);  //We read a single value from the module

    if (ACS_Value > High_peak)  //If that value is higher than the high peak (at first is 0)
    {
      High_peak = ACS_Value;  //The high peak will change from 0 to that value found
    }

    if (ACS_Value < Low_peak)  //If that value is lower than the low peak (at first is 1024)
    {
      Low_peak = ACS_Value;  //The low peak will change from 1024 to that value found
    }
  }  //We keep looping until we take all samples and at the end we will have the high/low peaks values

  Amps_Peak_Peak = High_peak - Low_peak;  //Calculate the difference
}




void currentSensors() {
  read_Amps(A1);                                //Launch the read_Amps function
  Amps_RMS = Amps_Peak_Peak * 0.3536 * 0.0257;  //Now we have the peak to peak value normally the formula requires only multiplying times 0.3536
  Serial.print(Amps_RMS);                       //Here I show the RMS value and the peak to peak value, you can print what you want and add the "A" symbol...
  Serial.print("\t");
  Serial.println(Amps_Peak_Peak);
  current_A = Amps_RMS;
  read_Amps(A2);                                //Launch the read_Amps function
  Amps_RMS = Amps_Peak_Peak * 0.3536 * 0.0257;  //Now we have the peak to peak value normally the formula requires only multiplying times 0.3536
  Serial.print(Amps_RMS);                       //Here I show the RMS value and the peak to peak value, you can print what you want and add the "A" symbol...
  Serial.print("\t");
  Serial.println(Amps_Peak_Peak);
  //delay(200);
  current_B = Amps_RMS;
  current_A < 0.16 ? current_A = 0 : current_A = current_A;
  current_B < 0.16 ? current_B = 0 : current_B = current_B;
}

void voltageSensor() {

  for (int i = 0; i < 100; i++) {
    sensorValue1 = analogRead(A0);
    Serial.println(sensorValue1);
    if (analogRead(A0) > 520) {
      val[i] = sensorValue1;
    } else {
      val[i] = 0;
    }
    delay(1);
  }

  max_v = 0;

  for (int i = 0; i < 100; i++) {
    if (val[i] > max_v) {
      max_v = val[i];
    }
    val[i] = 0;
  }
  if (max_v != 0) {


    VmaxD = max_v;
    VeffD = VmaxD / sqrt(2);
    Veff = (((VeffD - 420.76) / -90.24) * -210.2) + 210.2;
  } else {
    Veff = 0;
  }

  voltage = Veff;
  VmaxD = 0;

 // delay(100);
}
void Serialprints() {
  Serial.println("");
  Serial.println("block A");
  Serial.println("---------------");
  Serial.print("Current: ");
  Serial.print(current_A);
  Serial.print("A ");
  Serial.print("voltage: ");
  Serial.print(voltage);
  Serial.print("V ");
  Serial.println("");
  Serial.print("power: ");
  Serial.print(powerA);
  Serial.print("W ");
  Serial.print("energy: ");
  Serial.print(energyA);
  Serial.println("Kwh");
  Serial.println("block B");
  Serial.println("---------------");
  Serial.print("Current: ");
  Serial.print(current_B);
  Serial.print("A ");
  Serial.print("voltage: ");
  Serial.print(voltage);
  Serial.print("V ");
  Serial.println("");
  Serial.print("power: ");
  Serial.print(powerB);
  Serial.print("W ");
  Serial.print("energy: ");
  Serial.print(energyB);
  Serial.println("Kwh ");
}