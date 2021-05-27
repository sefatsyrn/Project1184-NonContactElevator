
//spı device communication
#include "SPI.h"
//nfc comm lib
#include "PN532_SPI.h"
#include "snep.h"
#include "NdefMessage.h"
#include "Ndef.h"
//canbus lib seed studio
#include "mcp2515_can.h"
//screen
#include<Wire.h>
#include<LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd (0x27, 16, 2); //screen connection

//canbus connection
const int SPI_CS_PIN = 9;
mcp2515_can CAN(SPI_CS_PIN);

//nfc communication
PN532_SPI pn532spi(SPI, 10);
SNEP nfc(pn532spi);
uint8_t ndefBuf[128];

int buzzerPin = 8; 

int redPin = 6;
int greenPin = 5;
int bluePin = 3;


void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  pinMode(buzzerPin, OUTPUT);
  
  Serial.begin(9600);
  SPI.begin();
  
  lcd.begin();
  lcd.noBacklight();

  lcd.setCursor(0, 0);
  lcd.print("----WELCOME-----");
  lcd.setCursor(0, 1);
  lcd.print("  PROJECT 1184  ");
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("                ");

  while (CAN_OK != CAN.begin(CAN_500KBPS, MCP_8MHz)) //canbus check
  {
    Serial.println("CAN BUS init Failed");
    delay(100);
  }
}

void loop() {

  bool wr = true;

  while (wr) {
    Serial.println("\nRead With Phone");
    setColor(255, 255, 0);
    lcd.setCursor(0, 0);
    lcd.print("READ WITH PHONE ");
    NdefMessage message = NdefMessage();
    
    message.addTextRecord("40"); // FLOOR size of the building
    int messageSize = message.getEncodedSize();
    message.encode(ndefBuf);
    
    if (0 >= nfc.write(ndefBuf, messageSize)) {
      Serial.println("Writing to Phone Is Failed!");
      setColor(255, 0, 0);
      lcd.setCursor(0, 1);
      lcd.print("READING FAILED!");
      digitalWrite(buzzerPin, HIGH);
      delay(1500);
      digitalWrite(buzzerPin, LOW);
      lcd.setCursor(0, 1);
      lcd.print("                ");
    } 
    else {
      Serial.println("Writing to Phone is Successful.");
      
      setColor(0, 255, 0);
      lcd.setCursor(0, 1);
      lcd.print("READING SUCCESS");
      
      digitalWrite(buzzerPin, HIGH);
      delay(150);
      digitalWrite(buzzerPin, LOW);
      delay(150);
      digitalWrite(buzzerPin, HIGH);
      delay(150);
      digitalWrite(buzzerPin, LOW);
      
      wr = false;
      
      delay(1050);

      lcd.setCursor(0, 1);
      lcd.print("                ");
    }
  }
  delay(500);

  int rd = 0; //max 3 write fail for phone
  
  while (rd < 3) {

    Serial.println("\nWrite the Chosen Floor to the NFC Using the Smartphone App.");
    setColor(255, 255, 0);
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(2, 0);
    lcd.print("WRITE FLOOR");
    
    int msgSize = nfc.read(ndefBuf, sizeof(ndefBuf));
    if (msgSize > 0) {
      NdefMessage msg  = NdefMessage(ndefBuf, msgSize);
      
      Serial.print("Floor: ");
      lcd.setCursor(0, 1);
      lcd.print("    FLOOR:      ");
      msg.print();

      String kat = turnback(); //turnback (function) is from ndef library(changed) ndef.cpp
      int kt = atoi(kat.c_str());

      lcd.setCursor(10, 1);
      lcd.print(kt);
      setColor(0, 255, 0);
      digitalWrite(buzzerPin, HIGH);
      delay(150);
      digitalWrite(buzzerPin, LOW);
      delay(150);
      digitalWrite(buzzerPin, HIGH);
      delay(150);
      digitalWrite(buzzerPin, LOW);
      delay(1550);
      lcd.setCursor(0, 1);
      lcd.print("                ");

      //CANBUS SEND DATA 
      unsigned char stmp[8] = {kt, 0, 0, 0, 0, 0, 0, 0};
      CAN.sendMsgBuf(0x01, 0, 8, stmp);

      Serial.println("\nReading the Data is Successful");
      
      lcd.setCursor(1, 1);
      lcd.print("WRITE SUCCESS");
      delay(1500);
      lcd.setCursor(1, 1);
      lcd.print("                ");

      rd = 3;

  } else {
      Serial.println("\nReading the Data is Failed");
      
      setColor(255, 0, 0);
      lcd.setCursor(2, 1);
      lcd.print("WRITE FAILED!");
      digitalWrite(buzzerPin, HIGH);
      delay(1500);
      digitalWrite(buzzerPin, LOW);
      lcd.setCursor(0, 1);
      lcd.print("                ");
      
       rd++;
    }
  }
}
void setColor(int redValue, int greenValue, int blueValue) {
  redValue = 255 - redValue;
  greenValue = 255 - greenValue;
  blueValue = 255 - blueValue;
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}
