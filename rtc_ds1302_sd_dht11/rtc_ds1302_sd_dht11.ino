#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <virtuabotixRTC.h> //ArduinoRTClibrary (Special for RTC1302 using it) - Set the Time Using RTC by Makuna Library
#include <SPI.h>
#include <SD.h>
#include "DHT.h"
#define DHTPIN 7
#define DHTTYPE DHT11

LiquidCrystal_I2C lcd(0x27, 16, 2);
const int chipSelect = 10;
File sd_file; //SPI Interface: (Sensor Shield ---> SD Card Module)
//VCC, GND, MOSI, D10, SCK, MISO ===> Match accordingly from left to right
virtuabotixRTC myRTC(5, 4, 2); //CLK, DAT, RST (RTC1302)
float t;
float h;
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(9600);
  dht.begin();
  Wire.begin();
  lcd.init(); lcd.backlight();
  Serial.print("Temp..");
  lcd.print("Initializing...");
  delay(2000); lcd.clear();

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
}

//Function can be as follows:

void tempstatus()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T=");
  lcd.setCursor(2, 0);
  lcd.print(String(myRTC.hours) + String(":") + String(myRTC.minutes) + String(" "));
  lcd.setCursor(7, 0);
  lcd.print(String(" D="));
  lcd.setCursor(10, 0);
  lcd.print(String(myRTC.dayofmonth) + String("/") + String(myRTC.month));
  lcd.setCursor(0, 1);
  lcd.print(String("T:"));
  lcd.setCursor(3, 1);
  lcd.print(t);
  lcd.setCursor(8, 1);
  lcd.print(String("| "));
  lcd.setCursor(9, 1);
  lcd.print(String("H:"));
  lcd.setCursor(12, 1);
  lcd.print(h);
}

void loop()
{
  myRTC.updateTime();
  tempstatus();
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print(String(myRTC.hours) + String(":") + String(myRTC.minutes) + String(":"));
    dataFile.print(String(myRTC.dayofmonth) + String("/") + String(myRTC.month) + String("/") + String(myRTC.year));
    dataFile.println(String("||") + String(dht.readTemperature()) + String(";") + String(dht.readHumidity()));
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}
