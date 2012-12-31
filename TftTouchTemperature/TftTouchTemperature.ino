#include <OneWire.h>

// TFT Touch Shield
#include <stdint.h>
#include <TouchScreen.h> 
#include <TFT.h>

// Copyright notice for TFT Touch Shield:
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

#define TS_MINX 140
#define TS_MAXX 900

#define TS_MINY 120
#define TS_MAXY 940


// OneWire notice:
//
// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library


OneWire  ds(A4);  // on pin A4

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// The 2.8" TFT Touch shield has 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

int timer = 5 * 60;
int timerStart;
int timerStatus = 0;
long timerStartTime;

long timeTemp;
boolean waitTemp = false;
byte addr[8];
byte type_s;

void setup(void) {
  Serial.begin(9600);

  Tft.init();
  initDraw();
}

void loop(void) {
  loopTimer();  
  loopTemperature();
}  

char* s2c(String tSold) {
  char txtOld[tSold.length() + 1]; 
  tSold.toCharArray(txtOld, tSold.length() + 1);
  return &txtOld[0];
}

void loopTimer() {
  // a point object holds x y and z coordinates
  Point p = ts.getPoint();
  p.x = map(p.x, TS_MINX, TS_MAXX, 240, 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, 320, 0);

  int timerOld = timer;

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > ts.pressureThreshhold) {
   
    if (p.x > 0 && p.x < 50 && p.y > 190 && p.y < 230) {
      Tft.drawString("+", 20, 197, 3, BLUE);

      Serial.println("++++++");
      if (timer > 60 * 60) {
        // nothing
      } 
      else if (timer > 10 * 60) {
        timer += 60;
      } 
      else if(timer > 5 * 60) {
        timer += 30;
      } 
      else if(timer >= 1 * 60) {
        timer += 15;
      } 
      else {
        timer += 5;
      }
      timerStatus = 1;
    }
    if (p.x > 0 && p.x < 50 && p.y > 230 && p.y < 270) {
      Tft.drawString("-", 20, 219, 3, BLUE);

      Serial.println("------");
      Serial.println(timer);

      if (timer > 10 * 60) {
        timer -= 60;
      } 
      else if(timer > 5 * 60) {
        timer -= 30;
      } 
      else if(timer > 1 * 60) {
        timer -= 15;
      } 
      else if(timer > 5) {
        timer -= 5;
      }
      Serial.println(timer);
      Serial.println("------");

      timerStatus = 1;
    }
    if (p.x > 120 && p.x < 200 && p.y > 190 && p.y < 270) {
      Serial.println("RRRRRRRRRR");
      Serial.println(timerStatus);
      if (timerStatus < 2) {
        timerStatus = 2;
        timerStartTime = millis();
        timerStart = timer;
      } 
      else {
        timerStatus = 1;
      }
      Serial.println(timerStatus);
      Serial.println("RRRRRRRRRR");
      delay(200);
    }

    Serial.print("X = "); 
    Serial.print(p.x);
    Serial.print("\tY = "); 
    Serial.print(p.y);
    Serial.print("\tPressure = "); 
    Serial.println(p.z);
  }

  if (timerStatus == 2) {
    timer = timerStart - (millis() - timerStartTime) / 1000;
    if (timer < 0) {
      timer = 0;
      timerStatus = 3;
    }
  }

  if (timerOld != timer) {
    String tSold = timerToString(timerOld);
    char txtOld[tSold.length() + 1]; 
    tSold.toCharArray(txtOld, tSold.length() + 1);

    String tS = timerToString(timer);
    char txt[tS.length() + 1]; 
    tS.toCharArray(txt, tS.length() + 1); 

    for(int i=0; i < tSold.length() && i < tS.length(); i++) {
      if (txt[i] == txtOld[i]) {
        txt[i] = txtOld[i] = ' ';
      }
    }
    Tft.drawString(txtOld, 55, 200, 4, BLACK);
    Tft.drawString(txt, 55, 200, 4, GRAY1);
  
    Tft.drawString("+", 20, 197, 3, BLACK);
    Tft.drawString("-", 20, 219, 3, BLACK);
  }
}

String timerToString(int timer) {
  int minutes = (timer / 60);
  int seconds = (timer % 60);

  String txt = "";
  if (minutes < 10) {
    txt += " ";
  }
  txt += minutes;
  txt += ":";
  if (seconds < 10) {
    txt += "0";
  }
  txt += seconds;
  return txt;
}
  
void loopTemperature() {
  if (!waitTemp) {
    loopTemperature1(addr, type_s);
    waitTemp = true;
    timeTemp = millis();
  } 
  else {
    if (millis() - timeTemp > 1000) {
      loopTemperature2(addr, type_s);
      waitTemp = false;
    }
  }
}


void loopTemperature1(byte* addr, byte& type_s) {
  byte i;

  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    //    delay(250);
    return;
  }

  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return;
  }
  Serial.println();

  // the first ROM byte indicates which chip
  switch (addr[0]) {
  case 0x10:
    Serial.println("  Chip = DS18S20");  // or old DS1820
    type_s = 1;
    break;
  case 0x28:
    Serial.println("  Chip = DS18B20");
    type_s = 0;
    break;
  case 0x22:
    Serial.println("  Chip = DS1822");
    type_s = 0;
    break;
  default:
    Serial.println("Device is not a DS18x20 family device.");
    return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end
}

void loopTemperature2(byte* addr, byte& type_s) {
  byte i;
  byte present = 0;
  byte data[12];
  float celsius, fahrenheit;

  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  Serial.print("  Data = ");
  Serial.print(present,HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

  // convert the data to actual temperature

  unsigned int raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // count remain gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } 
  else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
    // default is 12 bit resolution, 750 ms conversion time
  }
  celsius = 1;
  if(raw & 0x8000) {
    raw = (raw ^ 0xffff) + 1;
    celsius = -1;
  }
  celsius *= (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius, ");
  Serial.print(fahrenheit);
  Serial.println(" Fahrenheit");

  drawTemp(celsius, addr[7] == 0x6B ? 1 : 0); // FIXME 0x6B is the id for the outside temperature sensor
}

void initDraw() {
  Tft.drawString("Temperature", 20, 25, 2, WHITE);
  Tft.drawString("inside:", 20, 50, 2, WHITE);
  Tft.drawString("outside:", 20, 100, 2, WHITE);  

  Tft.drawString("Timer", 20, 170, 2, WHITE);  

  Tft.fillRectangle(10, 197, 38, 19, WHITE);
  Tft.drawRectangle(10, 197, 38, 19, GRAY1);
  Tft.fillRectangle(10, 219, 38, 19, WHITE);
  Tft.drawRectangle(10, 219, 38, 19, GRAY1);

  Tft.drawString("+", 20, 197, 3, BLACK);
  Tft.drawString("-", 20, 219, 3, BLACK);

  String timerStr = timerToString(timer);
  char timerChars[timerStr.length() + 1];
  timerStr.toCharArray(timerChars, timerStr.length() + 1);
  Tft.drawString(timerChars, 55, 200, 4, GRAY1);
}


void drawTemp(double temp, int tempId) {
  String tempString = tempToString(temp);
  char tempTxt[tempString.length() + 1];
  tempString.toCharArray(tempTxt, tempString.length() + 1);

  int p = tempId;
  Tft.fillRectangle(15, 70 + (p*50) - 1, 150, 23, GRAY2);
  Tft.drawRectangle(15, 70 + (p*50) - 2, 150, 25, WHITE);

  Tft.drawString(tempTxt, 20, 70 + (p*50), 3, WHITE);
}


String tempToString(double temp) {
  int temp100 = abs(temp) * 100;
  int Whole = temp100 / 100;  // separate off the whole and fractional portions
  int Fract = temp100 % 100;

  String t = "";
  if (Whole < 10)
  {
    t += " ";
  }
  if (temp < 0) // If its negative
  {
    t += "-";
  } 
  else {
    t += " ";
  }
  t += Whole;
  t += ".";
  if (Fract < 10)
  {
    t += "0";
  }
  t += Fract;  

  return t; 
}

