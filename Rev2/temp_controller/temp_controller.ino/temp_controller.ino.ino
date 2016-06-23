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

void setup(){
  
  Serial.begin(9600);
  
  Tft.init();  //init TFT library
  
  //Fan Speed display
  Tft.drawString("Fan Speed: ",0,0,2,BLUE);
  Tft.drawString("%",220,0,2,BLUE);
  //Increase Fan Speed
  Tft.drawRectangle(incFanSpeedPosX, incFanSpeedPosY, incFanSpeedWidth,incFanSpeedHight,BLUE);
  Tft.drawString("+",50,270,2,BLUE);
  //Decrease Fan Speed
  Tft.drawRectangle(decFanSpeedPosX, decFanSpeedPosY, decFanSpeedWidth,decFanSpeedHight,BLUE);
  Tft.drawString("-",170,270,2,BLUE);
}

char fanSpeed[4];
char prevFanSpeed[4];
//hold current fan speed 0-255
int currentFanValue = 0;
//fan speed from last loop
int previousFanSpeed = currentFanValue;
//how much to increase/decrease fan speed each push. Out of 255
int fanIncrement = 10;

void loop(){
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
        Serial.print("Increase Fan Speed :"); Serial.println(currentFanValue);
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
          Serial.print("Decrease Fan Speed :"); Serial.println(currentFanValue);
        }
      
        //Serial.println("Decrease Fan Speed");
      }
    }
  }
  
//  String(fadeValue * 100 / 255).toCharArray(fanSpeed, 4);
//  Tft.drawString(prevFanSpeed,170,0,2,BLACK);
//  Tft.drawString(fanSpeed,170,0,2,BLUE);
//  analogWrite(fanPin, fadeValue);
//  String(fadeValue * 100 / 255).toCharArray(prevFanSpeed, 4);

  

}
