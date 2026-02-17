// Hydroponics 2 XProject
// Phoebe Katz (phoebekatz645@gmail.com)
// August 27, 2024
// This code runs the seedling station in Benedum. It checks two temp/humidity sensors: one that sits inside the top level, where the youngest seedlings live, 
// and the other that measures the ambient temp/humidity in the station. It also runs the pumps that provide water to the seedlings and measures the remaining
// water in the reservoir. Finally, it reports all the data that it has to the GIGA display. 

// this code differse from "seedlingToDisplay" in that it does not check data coming from the SHTC3 sensor and the display code has been improved.

// LIBRARIES
#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_SHTC3.h>
#include "Arduino_GigaDisplay_GFX.h"

// OUTPUT
// display screen
GigaDisplay_GFX display;

// INPUT 
// all sensors provide both temp and humidity
Adafruit_AHTX0 ahtSensor1; // for top level
Adafruit_AHTX0 ahtSensor2; // for bottom? level
Adafruit_SHTC3 shtcSensor = Adafruit_SHTC3(); // for mid? level
// multiplexer to allow for I2C communication to multiple sensors with the same address
#define TCAADDR 0x74

// DISPLAY COLORS
#define BLACK 0x0000
#define GREEN 0x2aa2
#define WHITE 0xffff
#define CREAM 0xeef7
#define BLUE 0x4b17

// SENSOR STATUS
// working -> true
bool ahtSensor1Status = false;
bool ahtSensor2Status = false;
bool shtcSensorStatus = false;

// COUNTER
int count = 0; // monitors when pump should turn on

// START
void setup() 
{
  Wire.begin();

  // configure display
  display.begin();
  display.setRotation(1); // establish horizontal orientation of board
  display.fillScreen(GREEN);
  display.setTextSize(3); //adjust text size
  display.setTextColor(CREAM);

  // Temp/Humidity Probe Check. Top Level
  channelSelect(6);
  display.setCursor(25, 45);
  string aht1Log;
  if (ahtSensor1.begin()) {
    aht1Log = "Found AHT Sensor 1! (#6 on multiplexer)";
    ahtSensor1Status = true;
  }
  else {
    aht1Log = "Could not find AHT Sensor 1. (#6 on multiplexer)";
  }
  Serial.println(aht1Log)
  display.println(aht1Log)

  // Temp/Humidity Probe Check. Middle? Level
  channelSelect(3);
  display.setCursor(22, 205);
  string aht2Log;
  if (ahtSensor2.begin()) {
    aht2Log = "Found AHT Sensor 2! (#3 on multiplexer)";
    ahtSensor2Status = true;
  }
  else {
    aht2Log = "Could not find AHT Sensor 2 (#3 on multiplexer)";
  }
  Serial.println(aht2Log);
  display.println(aht2Log);

  // Ambient Temp/Humidity Check. Bottom? Level
  channelSelect(4)
  display.setCursor(25, 355);
  string shtcLog;
  if (!shtcSensor.begin()) {
    shtcLog = "Found SHTC Sensor!";
    shtcSensorStatus = true;
  }
  else {
    shtcLog = "Could not find SHTC Sensor.";
  }
  Serial.println(shtcLog);
  display.println(shtcLog);

  display.fillScreen(GREEN);

  // draw rectangles (3 vertically stacked for levels of seedling station)
  drawThickRoundRect(10, 10, 370, 150, 25, 3, BLACK); // top level 
  drawThickRoundRect(10, 170, 370, 150, 25, 3, BLACK); // medium level
  drawThickRoundRect(10, 330, 370, 150, 25, 3, BLACK); // bottom level

  // draw rectangles for liquid level monitoring
  // drawThickRoundRect(390,10,190,460,25,3,BLACK); // Adult plant station
  drawThickRoundRect(595, 10, 190, 460, 25, 3, BLACK); // seedling station

  // draw title
  display.setCursor(460, 210);
  display.println("The");
  display.setCursor(395, 240);
  display.println("Hydroponics");
  display.setCursor(460, 270);
  display.println("Club");
}

// Daph's quick modularizarition of the sensor code
// I just got tired of copy and pasting my changes..
void attemptSensor(int channel){
  sensors_event_t probeHumidity, probeTemp;
  channelSelect(channel);
  string displayLine1 = "";
  string displayLine2 = "";
  
  // TEMP/HUMIDITY PROBE 1 (top level)
  if (channel == 6){
    if (ahtSensor1Status){
      ahtSensor1.getEvent(&probeHumidity, &probeTemp)
    }
    else {
      displayLine1 = "Could not find AHT Sensor 1. (#6 on multiplexer)"
    }
  }
  // TEMP/HUMIDITY PROBE 2 (middle level/ambient)
  else if (channel == 3){
    if (ahtSensor2Status){
      ahtSensor2.getEvent(&probeHumidity, &probeTemp)
    }
    else {
      displayLine1 = "Could not find AHT Sensor 2. (#3 on multiplexer)"
    }
  }
  // bottom level (bottom level/ambient)
  else if (channel == 4){
    if (shtcSensorStatus){
      shtcSensor.getEvent(&probeHumidity, &probeTemp)
    }
    else {
      displayLine1 = "Could not find SHTC Sensor. (#4 on multiplexer)"
    }
  }

  if (!displayLine1) {
    int temp = ((probeTemp.temperature)*1.8) + 32; // convert to Fahrenheit
    displayLine1 = "Temp: " + String(temp) + " F";
    displayLine2 = "Humidity: " + String(probeHumidity.relative_humidity) + "% rH";
  }

  display.fillRect(12, 12, 360, 140, GREEN);
  display.setCursor(25, 35);
  display.print(displayLine1);
  display.setCursor(25, 65); // yDiff = 30
  display.print(displayLine2);

}

void loop() 
{
  // attempt using sensors
  attemptSensor(3);
  attemptSensor(6);
  attemptSensor(4);
 
  // LIQUID LEVEL SENSOR
  float resistance = analogRead(0); // read resistance from analog pin 0
  
  // conversion to real values
  float difference = (resistance - 505) / 362; // calculates percent difference from max R
  float eqResistance = 2400 - difference*1900;

  // resistance -> height
  float height = (((resistance - 500) / 370) * 31.2 ) + 2;
  float percentOfTotal = (height / 28.0) * 100.0;

  // display liquid level
  partialFillRoundRect(595, 10, 190, 460, 25, 14, BLUE);
  
  display.setCursor(640, 240);
  display.println(percentOfTotal);
  display.setCursor(750, 240); // xDiff = 80
  display.println("%"); 
  display.setCursor(680, 270); // xDiff = 60
  display.println("full"); 

  // RELAY (PUMP CONTROL)
  if (count == 1440) // turn pump on once a day
  {
    digitalWrite(2,1); // set digital pin 2 (turn on pump)
    delay(30000); // pump on for 30 s
    digitalWrite(2,0); // turn off pump

    count = 0;
  }
 count ++;

  delay(60000); // repeat loop every minute
}

void channelSelect(uint8_t channel) // changing multiplexer (TCA's) output channel
{
  Wire.beginTransmission(TCAADDR);
  Wire.write(1<<channel);
  Wire.endTransmission();
}

void drawThickRoundRect(uint16_t originX, uint16_t originY, uint16_t width, uint16_t height, uint16_t radius, uint16_t thickness, uint16_t color)
{
  for(int i = 0; i<thickness; i++)
  {
    // uint16_t x = originX + i; 
    //uint16_t width = lengthX - (2*i);
    //uint16_t height = l
    display.drawRoundRect(originX + i, originY + i, width - (2*i), height - (2*i), radius, color);
  }
}

void partialFillRoundRect(uint16_t originX, uint16_t originY, uint16_t lengthX, uint16_t lengthY, uint16_t radius, uint16_t waterHeight, uint16_t color)
{
  //originX: x-coordinate of the top left corner of the full rectangle (if not rounded)
  //originY: y-coordinate of the top left corner of the rull rectangle (if not rounded)
  //lengthX: horizontal width of the full rectangle
  //lengthY: vertical height of the full rectangle
  //waterHeight:height in centimetres as measured by the liquid level sensor (physical max = 28cm/11in)
  //color: color you want for the new rectangle (the fill)

  uint16_t height = (waterHeight * lengthY) / 28; // height, in pixels, that will be shown on the display
  uint16_t y2 = originY + ( lengthY - height);
  display.fillRoundRect(originX, y2, lengthX, height,radius,color);
  display.fillRect(originX, y2 , radius, radius, color);
  display.fillRect( (originX + lengthX) - radius, y2, radius, radius, color);
}
