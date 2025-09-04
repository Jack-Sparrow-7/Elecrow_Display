#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <TFT_eSPI.h>

const byte backLightPin = 27;

TFT_eSPI display = TFT_eSPI();

void setup()
{
  Serial.begin(9600);
  Serial.println("Serial Initialized!");
  display.begin();
  pinMode(backLightPin,OUTPUT);
  digitalWrite(backLightPin,HIGH);
  display.setRotation(1);
  display.fillScreen(TFT_WHITE);
  display.fillCircle(160,120,50,TFT_BLUE);
  display.setCursor(140,110);
  display.setTextSize(3);
  display.print("Rit");
}

void loop()
{
  delay(100);
}