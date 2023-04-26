#include <SoftwareSerial.h>
SoftwareSerial esp(2, 3);  // the pins for arduino communication to esp RX and TX respectively
// libraries for the RFID
#include <SPI.h>
#include <MFRC522.h>
//max 30100 libraries
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
//mlx body temp library
#include <Adafruit_MLX90614.h>
// mlx globals
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
// max30100 globals
#define REPORTING_PERIOD_MS 1000
float bodyTemp = 0;
unsigned int lastMillisUpdate = 0;
unsigned int lastDataSent = 0;
int oxygen = 0;
int heartBeat = 0;
PulseOximeter pox;
uint32_t tsLastReport = 0;
// Callback (registered below) fired when a pulse is detected
void onBeatDetected() {
  Serial.println("Beat!");
}

//Globals for the RFID reader
#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);  // Instance of the class
MFRC522::MIFARE_Key key;
// Init array that will store new NUID
byte nuidPICC[4];
int LastTagInterval = 1000;
unsigned int lastTagTime = 0;
String cardId = "";


void MAX30100SetUp() {
  Serial.print("Initializing pulse oximeter..");
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;)
      ;
  } else {
    Serial.println("SUCCESS");
  }

  // The default current for the IR LED is 50mA and it could be changed
  //   by uncommenting the following line. Check MAX30100_Registers.h for all the
  //   available options.
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  // Register a callback for the beat detection
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

// function to set up the rfid reader
void RFID_setUP() {
  SPI.begin();      // Init SPI bus
  rfid.PCD_Init();  // Init MFRC522
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}
void MLX_SetUp() {
  while (!Serial)
    ;
  Serial.println("Adafruit MLX90614 test");
  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1)
      ;
  };
  Serial.print("Emissivity = ");
  Serial.println(mlx.readEmissivity());
  Serial.println("================================================");
}


void setup() {
  Serial.begin(115200);
  esp.begin(115200);
  RFID_setUP();
  MAX30100SetUp();
  MLX_SetUp();
}
void loop() {
  HeartOxygen();
  MLX_Data();
  RFID_Read();

  if (millis() - lastDataSent >= 50) {
    sendDataEsp8266();
    lastDataSent = millis();
  }
}
void sendDataEsp8266() {

  String dataToSend = "";
  dataToSend += "{\"card\":\"";
  dataToSend += cardId + "\",\"temp\":\"" + bodyTemp + "\", \"oxygen\":\"" + oxygen + "\",\"heatBeat\":\"" + heartBeat + "\"}";
  Serial.println(dataToSend);
  esp.println(dataToSend);  //print the data to esp in a JSON format so that we can deserialize the data in the esp
}

void MLX_Data() {
  if (millis() - lastMillisUpdate >= 500) {
    bodyTemp = mlx.readObjectTempC();
    lastMillisUpdate = millis();
  }
}


void HeartOxygen() {
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
  }
}

void RFID_Read() {
  cardId = "";
  // the function to reinitialize the lastread array when the time excedds the defined time
  if (millis() - lastTagTime >= LastTagInterval) {
    // re initialize  nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = "";
    }
  }

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if (!rfid.PICC_ReadCardSerial())
    return;

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);


  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {

    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3]) {


    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }


    printHex(rfid.uid.uidByte, rfid.uid.size);

  } else {
    Serial.println(F("Card read previously."));
  }

  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


void printHex(byte *buffer, byte bufferSize) {

  for (byte i = 0; i < bufferSize; i++) {

    cardId += String(buffer[i], HEX);
    sendDataEsp8266();
  }
}