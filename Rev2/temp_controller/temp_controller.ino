// Draw Boxes - Demonstrate drawRectangle and fillRectangle
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

//
//Features:
//
//Arduino UNO Rev3 / Mega Shield compatible footprint.
//TFT
//Resolution : 240 x 320 pixels
//Size : 2.8" (50.0 x 69.2 x 2.5 mm)
//Colors : 262K
//Backlight : LED
//Driver IC: ST7781R
//4-Wire resistive touch screen

#include <stdint.h>
#include <TouchScreen.h>
#include <TFT.h>

#ifdef SEEEDUINO
  #define YP A2   // must be an analog pin, use "An" notation!
  #define XM A1   // must be an analog pin, use "An" notation!
  #define YM 14   // can be a digital pin, this is A0
  #define XP 17   // can be a digital pin, this is A3 
#endif

#ifdef MEGA
  #define YP A2   // must be an analog pin, use "An" notation!
  #define XM A1   // must be an analog pin, use "An" notation!
  #define YM 54   // can be a digital pin, this is A0
  #define XP 57   // can be a digital pin, this is A3 
#endif 

//Measured ADC values for (0,0) and (210-1,320-1)
//TS_MINX corresponds to ADC value when X = 0
//TS_MINY corresponds to ADC value when Y = 0
//TS_MAXX corresponds to ADC value when X = 240 -1
//TS_MAXY corresponds to ADC value when Y = 320 -1

#define TS_MINX 140
#define TS_MAXX 900

#define TS_MINY 120
#define TS_MAXY 940

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// The 2.8" TFT Touch shield has 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

int fanPin = 44;    // LED connected to digital pin 9

//Increase Fan Speed Draw Parameters
int incFanSpeedPosX = 0;
int incFanSpeedPosY = 250;
int incFanSpeedWidth = 110;
int incFanSpeedHight = 60;
//Decrease Fan Speed Draw Parameters
int decFanSpeedPosX = 120;
int decFanSpeedPosY = 250;
int decFanSpeedWidth = 110;
int decFanSpeedHight = 60;

char fanSpeed[4] = "0";
char prevFanSpeed[4]= "0";
//hold current fan speed 0-255
int currentFanValue = 0;
//fan speed from last loop
int previousFanSpeed = currentFanValue;
//how much to increase/decrease fan speed each push. Out of 255
int fanIncrement = 10;

#include "DHT.h"
#define DHTPIN 22     // what digital pin we're connected to
// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.

char currentTemp[4] = "0";
char previousTemp[4] = "0";

DHT dht(DHTPIN, DHTTYPE);

void setup(){
  
  Serial.begin(9600);

  dht.begin();
  
  Tft.init();  //init TFT library
  
  //Fan Speed display
  Tft.drawString("Fan Speed: ",0,0,2,BLUE);
  Tft.drawString(fanSpeed,170,0,2,BLUE);
  Tft.drawString("%",220,0,2,BLUE);
  //Increase Fan Speed
  Tft.drawRectangle(incFanSpeedPosX, incFanSpeedPosY, incFanSpeedWidth,incFanSpeedHight,BLUE);
  Tft.drawString("+",50,270,2,BLUE);
  //Decrease Fan Speed
  Tft.drawRectangle(decFanSpeedPosX, decFanSpeedPosY, decFanSpeedWidth,decFanSpeedHight,BLUE);
  Tft.drawString("-",170,270,2,BLUE);
  //Current Temp
  Tft.drawString("Temp: ",0,20,2,BLUE);
  Tft.drawString("*F",210,20,2,BLUE);
}


int prevTempValue;
void loop(){
  int temp = GetTemp();
  if(prevTempValue != temp){
    String(temp).toCharArray(currentTemp, 4);
    Tft.drawString(previousTemp,170,20,2,BLACK);
    Tft.drawString(currentTemp,170,20,2,BLUE);
    String(temp).toCharArray(previousTemp, 4);
    prevTempValue = temp;
  }
  // a point object holds x y and z coordinates
  Point p = ts.getPoint();
  p.x = map(p.x, TS_MINX, TS_MAXX, 240, 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, 320, 0);
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > ts.pressureThreshhold){
    Serial.print("X = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.print(p.y);
    Serial.print("\tPressure = "); Serial.println(p.z);
    //Increase Fan Zones
    if( p.x >= incFanSpeedPosX && p.x <= (incFanSpeedPosX + incFanSpeedWidth) && p.y >= incFanSpeedPosY && p.y <= (incFanSpeedPosY + incFanSpeedHight)){
      if(currentFanValue >= 255){
      }
      else{
        currentFanValue += fanIncrement;
        if(currentFanValue >= 255){
          currentFanValue = 255;
        }
        analogWrite(fanPin, currentFanValue);
        String(currentFanValue * 100 / 255).toCharArray(fanSpeed, 4);
        Tft.drawString(prevFanSpeed,170,0,2,BLACK);
        Tft.drawString(fanSpeed,170,0,2,BLUE);
        String(currentFanValue * 100 / 255).toCharArray(prevFanSpeed, 4);
        //Serial.print("Increase Fan Speed :"); Serial.println(currentFanValue);
      }
    }
    else {
      if( p.x >= decFanSpeedPosX && p.x <= (decFanSpeedPosX + decFanSpeedWidth) && p.y >= decFanSpeedPosY && p.y <= (decFanSpeedPosY + decFanSpeedHight)){
        if(currentFanValue <= 0){
        }
        else{
          currentFanValue -= fanIncrement;
          if(currentFanValue <= 0){
            currentFanValue = 0;
          }
          analogWrite(fanPin, currentFanValue);
          String(currentFanValue * 100 / 255).toCharArray(fanSpeed, 4);
          Tft.drawString(prevFanSpeed,170,0,2,BLACK);
          Tft.drawString(fanSpeed,170,0,2,BLUE);
          String(currentFanValue * 100 / 255).toCharArray(prevFanSpeed, 4);
          //Serial.print("Decrease Fan Speed :"); Serial.println(currentFanValue);
        }
      }
    }
  }
}

int GetTemp() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

//  Serial.print("Humidity: ");
//  Serial.print(h);
//  Serial.print(" %\t");
//  Serial.print("Temperature: ");
//  Serial.print(t);
//  Serial.print(" *C ");
//  Serial.print(f);
//  Serial.print(" *F\t");
//  Serial.print("Heat index: ");
//  Serial.print(hic);
//  Serial.print(" *C ");
//  Serial.print(hif);
//  Serial.println(" *F");
  return f;
}


