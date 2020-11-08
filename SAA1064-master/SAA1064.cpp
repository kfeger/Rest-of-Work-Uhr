/*
SAA1064.h - Part of the SAA1064 library for Arduino
Copyright (c) 2013 Jeff Brown All right reserved.
http://photonicsguy.ca/projects/electronics/7segdisplay

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 */


/******************************************************************************
 * Includes
 ******************************************************************************/

#include "SAA1064.h"
#include <Wire.h>
/******************************************************************************
 * Definitions
 ******************************************************************************/

byte DIGIT[] = { 0x3F, 0x6, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x7, 0x7F, 0x67, 0x77, 0x7C, 0x58, 0x5E, 0x79, 0x71, 0x40, 0x63,0x50,0x80, 0x1E }; // Extra 0x80 (DP) for error buffer.
byte SEG[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

#define	SAA1064_ADDRESS 0x38
#define REG_CONTROL 0x00

#define REG_DIGIT1 0x01
#define REG_DIGIT2 0x02
#define REG_DIGIT3 0x03
#define REG_DIGIT4 0x04


#define REG_CON_BITS 
#define CONTROL_BYTE 0x07
//#define LOW 0x01
//#define MED 0x04
//#define HIGH 0x07
volatile uint8_t _bright;


/******************************************************************************
 * Constructors
 ******************************************************************************/

SAA1064::SAA1064(uint8_t address)
{
  _Address = address;	// TODO Implement different subaddresses
}


/******************************************************************************
 * User API
 ******************************************************************************/
void SAA1064::begin(uint8_t address)
{
}
void SAA1064::begin()
{
_bright=32;
  //Wire.begin();
  Wire.beginTransmission(SAA1064_ADDRESS);
  Wire.write(REG_CONTROL);
  //          x - not used
  //           xxx -   Brightness
  //              x -  Segment test
  //               x  - Digit 1/3 blanking
  //                x - Digit 2/4 blanking
  //                 x - 1 for Dynamic mode (4 digit display)
  Wire.write(CONTROL_BYTE | _bright);	// Send control byte ORED with brightness
  Wire.endTransmission();
  delay(100);
}
void SAA1064::print(int number)
{
  int g=0;
  int h=0;
  int j=0;

  int q;	// 1000er
  int w;	// 100er
  int e;	// 10er
  int r;	// 1er
  boolean negative=false;

  Wire.beginTransmission(SAA1064_ADDRESS);
  Wire.write(B00000001);
  if(number>9999 || number<-999) {
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[16]|0x80);
    Wire.write(DIGIT[16]);
    Wire.endTransmission();
    return;
  }
  if(number<0 && number>-1000) {
    number*=-1;
    negative=true;
  }
  g=number/1000;
  number=number-(g*1000);
  h=number/100;
  number=number-(h*100);
  j=number/10;
  number=number-(j*10);

  q=DIGIT[g];
  w=DIGIT[h];
  e=DIGIT[j];
  r=DIGIT[number];



  if(g==0) {
    q=0;  // Blank thousands if 0
    if(h==0) {
      w=0;  // Blank hundreds if 0
      if(j==0) {
        e=0;  // Blank tens if 0
      }
    }
  }
  if(negative) {
    q=DIGIT[16];
  }
  /*Wire.write(q);
  Wire.write(w);
  Wire.write(e);
  Wire.write(r);*/

  Wire.write(e);
  Wire.write(r);
  Wire.write(q);
  Wire.write(w);


  Wire.endTransmission();
}

void SAA1064::print(int number, int Dot)
{
  int g=0;
  int h=0;
  int j=0;

  int q;
  int w;
  int e;
  int r;
  boolean negative=false;

  Wire.beginTransmission(SAA1064_ADDRESS);
  Wire.write(B00000001);
  if(number>9999 || number<-999) {
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[16]|0x80);
    Wire.write(DIGIT[16]);
    Wire.endTransmission();
    return;
  }
  if(number<0 && number>-1000) {
    number*=-1;
    negative=true;
  }
  g=number/1000;
  number=number-(g*1000);
  h=number/100;
  number=number-(h*100);
  j=number/10;
  number=number-(j*10);

  q=DIGIT[g];
  w=DIGIT[h];
  e=DIGIT[j];
  r=DIGIT[number];



  /*if(g==0) {
    q=0;  // Blank thousands if 0
    if(h==0) {
      w=0;  // Blank hundreds if 0
      if(j==0) {
        e=0;  // Blank tens if 0
      }
    }
  }*/
  if(g == 0) {
	  q = 0;
  }
  
  if(negative) {
    q=DIGIT[16];
  }
	if((Dot & 2) && !negative)
		Wire.write(e |0x80);
	else
		Wire.write(e);
	if(Dot & 1)
		Wire.write(r | 0x80);
	else
		Wire.write(r);
	if(Dot & 8)
		Wire.write(q | 0x80);
	else
		Wire.write(q);
	if(Dot & 4)
		Wire.write(w | 0x80);
	else
		Wire.write(w);


  Wire.endTransmission();
}

void SAA1064::printTime(int number, bool Dot = true)
{
  int g=0;
  int h=0;
  int j=0;

  int q;
  int w;
  int e;
  int r;
  boolean negative=false;

  Wire.beginTransmission(SAA1064_ADDRESS);
  Wire.write(B00000001);
  if(number>9999 || number<0) {
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[16]|0x80);
    Wire.write(DIGIT[16]);
    Wire.endTransmission();
    return;
  }
  g=number/1000;
  number=number-(g*1000);
  h=number/100;
  number=number-(h*100);
  j=number/10;
  number=number-(j*10);

  q=DIGIT[g];	// 1000er
  w=DIGIT[h];	// 100er
  e=DIGIT[j];	// 10er
  r=DIGIT[number];	// 1er

  if(g == 0) {
	  q = 0;
  }
  
  if(negative) {
    q=DIGIT[16];
  }
	Wire.write(e); // 10er
	Wire.write(r);	// 1er
	Wire.write(q);	// 1000er
	if (Dot)		// 100er
		Wire.write(w | 0x80);
	else
		Wire.write(w);
	Wire.endTransmission();
}

void SAA1064::print(double num)
{
  int number=num*10;
  int g=0;
  int h=0;
  int j=0;

  int q;
  int w;
  int e;
  int r;
  boolean negative=false;

  Wire.beginTransmission(SAA1064_ADDRESS);
  Wire.write(B00000001);
  if(number>9999 || number<-999) {
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[16]);
    Wire.endTransmission();
    return;
  }
  if(number<0 && number>-1000) {
    number*=-1;
    negative=true;
  }
  g=number/1000;
  number=number-(g*1000);
  h=number/100;
  number=number-(h*100);
  j=number/10;
  number=number-(j*10);

  q=DIGIT[g];
  w=DIGIT[h];
  e=DIGIT[j]|0x80;
  r=DIGIT[number];



  /*if(g==0) {
    q=0;  // Blank thousands if 0
    if(h==0) {
      w=0;  // Blank hundreds if 0
      if(j==0) {
        e=0;  // Blank tens if 0
      }
    }
  }*/
  if(negative) {
    q=DIGIT[16];
  }
  /*
  Wire.write(r);
  Wire.write(e);
  Wire.write(w);
  Wire.write(q);
  */
  Wire.write(e);
  Wire.write(r);
  Wire.write(q);
  Wire.write(w);


  Wire.endTransmission();

}

void SAA1064::printTemp(int number)	//Temperatur -99..99 Grad C
{
  int g=0;
  int h=0;
  int j=0;

  int q;
  int w;
  int e;
  int r;
  boolean negative=false;

  Wire.beginTransmission(SAA1064_ADDRESS);
  Wire.write(B00000001);
  if(number>99 || number<-99) {
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[17]);
    Wire.endTransmission();
    return;
  }
  if(number<0 && number>-1000) {
    number*=-1;
    negative=true;
  }
  g=number/1000;
  number=number-(g*1000);
  h=number/100;
  number=number-(h*100);
  j=number/10;
  number=number-(j*10);

  q=DIGIT[g];
  w=DIGIT[h];
  e=DIGIT[j];
  r=DIGIT[number];



  if(g==0) {
    q=0;  // Blank thousands if 0
    if(h==0) {
      w=0;  // Blank hundreds if 0
      if(j==0) {
        e=0;  // Blank tens if 0
      }
    }
  }
  if(negative) {
    q=DIGIT[16];
  }
  /*
  Wire.write(r);
  Wire.write(e);
  Wire.write(w);
  Wire.write(q);
  */
  Wire.write(r);
  Wire.write(DIGIT[17]);
  Wire.write(w);
  Wire.write(e);


  Wire.endTransmission();

}

void SAA1064::printHum(int number)	//Feuchte 0..100 %rH
{
  int g=0;
  int h=0;
  int j=0;

  int q;
  int w;
  int e;
  int r;
  boolean negative=false;

  Wire.beginTransmission(SAA1064_ADDRESS);
  Wire.write(B00000001);
  if(number>100 || number<0) {
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[18]);
    Wire.endTransmission();
    return;
  }
  if(number<0 && number>-1000) {
    number*=-1;
    negative=true;
  }
  g=number/1000;
  number=number-(g*1000);
  h=number/100;
  number=number-(h*100);
  j=number/10;
  number=number-(j*10);

  q=DIGIT[g];
  w=DIGIT[h];
  e=DIGIT[j];
  r=DIGIT[number];



  if(g==0) {
    q=0;  // Blank thousands if 0
    if(h==0) {
      w=0;  // Blank hundreds if 0
      if(j==0) {
        e=0;  // Blank tens if 0
      }
    }
  }
  if(negative) {
    q=DIGIT[16];
  }
  /*
  Wire.write(r);
  Wire.write(e);
  Wire.write(w);
  Wire.write(q);
  */
  Wire.write(r);
  Wire.write(DIGIT[18]);
  Wire.write(w);
  Wire.write(e);


  Wire.endTransmission();

}


void SAA1064::printYear(int number)	//Temperatur -99..99 Grad C
{
  int g=0;
  int h=0;
  int j=0;

  int q;
  int w;
  int e;
  int r;
  boolean negative=false;

  Wire.beginTransmission(SAA1064_ADDRESS);
  Wire.write(B00000001);
  if(number>99 || number<-99) {
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[16]);
    Wire.write(DIGIT[17]);
    Wire.endTransmission();
    return;
  }
  if(number<0 && number>-1000) {
    number*=-1;
    negative=true;
  }
  g=number/1000;
  number=number-(g*1000);
  h=number/100;
  number=number-(h*100);
  j=number/10;
  number=number-(j*10);

  q=DIGIT[g];
  w=DIGIT[h];
  e=DIGIT[j];
  r=DIGIT[number];



  if(g==0) {
    q=0;  // Blank thousands if 0
    if(h==0) {
      w=0;  // Blank hundreds if 0
      if(j==0) {
        e=0;  // Blank tens if 0
      }
    }
  }
  if(negative) {
    q=DIGIT[16];
  }
  /*
  Wire.write(r);
  Wire.write(e);
  Wire.write(w);
  Wire.write(q);
  */
  Wire.write(r);
  Wire.write(DIGIT[20]);
  Wire.write(w);
  Wire.write(e);


  Wire.endTransmission();

}

void SAA1064::printDashDash (int dd) {
	Wire.beginTransmission(SAA1064_ADDRESS);
	Wire.write(B00000001);
	if (dd == 1) {		// Grad C
		Wire.write(DIGIT[16]);
		Wire.write(DIGIT[17]);
		Wire.write(0);
		Wire.write(DIGIT[16]);
		}
	else if (dd == 2) {	// %rH
		Wire.write(DIGIT[16]);
		Wire.write(DIGIT[18]);
		Wire.write(0);
		Wire.write(DIGIT[16]);
		}
	else {	// nüscht
		Wire.write(DIGIT[16]);
		Wire.write(0);
		Wire.write(0);
		Wire.write(DIGIT[16]);
		}
    Wire.endTransmission();
}

void SAA1064::printDotDot (void) {
	Wire.beginTransmission(SAA1064_ADDRESS);
	Wire.write(B00000001);
    Wire.write(0x80);
    Wire.write(0);
    Wire.write(0);
    Wire.write(0x80);
    Wire.endTransmission();
}


void SAA1064::brightness(uint8_t bright)	// 1 thru 7
{
if(bright>0x7) {
bright==0x7;
}
if(bright==0) {
bright=1;
}

_bright=bright*16;

  Wire.beginTransmission(SAA1064_ADDRESS);
  Wire.write(REG_CONTROL);
  Wire.write(CONTROL_BYTE | _bright);	// Send control byte ORED with brightness
  Wire.endTransmission();
}

void SAA1064::snake(uint8_t mode, uint8_t cycles)        // Snake Animation
{
int _delay=50;

  if(mode==0) {
    for(int i=0;i<=cycles;i++) {
      for(int j=0;j<=5;j++) {
        Wire.beginTransmission(SAA1064_ADDRESS);
        Wire.write(REG_DIGIT1);
        Wire.write(SEG[j]);
        Wire.write(SEG[j]);
        Wire.write(SEG[j]);
        Wire.write(SEG[j]);
        Wire.endTransmission();    // stop transmitting
        delay(250);
      }
    }
  }
  if(mode==1) {
    for(int i=0;i<=cycles;i++) {
      for(int j=0;j<5;j++) {
        Wire.beginTransmission(SAA1064_ADDRESS);
        Wire.write(REG_DIGIT1);
        Wire.write(SEG[5-j]);
        Wire.write(SEG[j+1]);
        Wire.write(SEG[5-j]);
        Wire.write(SEG[j+1]);
        Wire.endTransmission();
        delay(250);
      }
 Wire.beginTransmission(SAA1064_ADDRESS);
        Wire.write(REG_DIGIT1);
        Wire.write(SEG[0]);
        Wire.write(SEG[0]);
        Wire.write(SEG[0]);
        Wire.write(SEG[0]);
        Wire.endTransmission();
        delay(250);
    }
  }

if(mode==2) {
for(int i=0;i<=cycles;i++) {

	Wire.beginTransmission(SAA1064_ADDRESS);
        Wire.write(REG_DIGIT1);
        Wire.write(0);
        Wire.write(SEG[0]);
        Wire.write(0);
        Wire.write(SEG[0]);
        Wire.endTransmission();
        delay(_delay);

	Wire.beginTransmission(SAA1064_ADDRESS);
        Wire.write(REG_DIGIT4);
        Wire.write(0);
        Wire.endTransmission();

	for(i=0;i<4;i++) {
	Wire.beginTransmission(SAA1064_ADDRESS);
        Wire.write(REG_DIGIT1);
        Wire.write(SEG[i]);
        Wire.write(0);
        Wire.write(SEG[i]);
        Wire.endTransmission();
        delay(_delay);
	}

	Wire.beginTransmission(SAA1064_ADDRESS);
        Wire.write(REG_DIGIT1);
        Wire.write(0);
        Wire.endTransmission();

	Wire.beginTransmission(SAA1064_ADDRESS);
        Wire.write(REG_DIGIT2);
        Wire.write(SEG[3]|SEG[7]);
        Wire.write(0);
        Wire.write(SEG[3]|SEG[7]);
        Wire.endTransmission();
        delay(abs(_delay*0.33));

	Wire.beginTransmission(SAA1064_ADDRESS);
        Wire.write(REG_DIGIT2);
        Wire.write(SEG[3]);
        Wire.write(0);
        Wire.write(SEG[3]);
        Wire.endTransmission();
        delay(abs(_delay*0.67));

	Wire.beginTransmission(SAA1064_ADDRESS);
        Wire.write(REG_DIGIT2);
        Wire.write(SEG[4]);
        Wire.write(0);
        Wire.write(SEG[4]);
        Wire.endTransmission();
        delay(_delay);

	Wire.beginTransmission(SAA1064_ADDRESS);
        Wire.write(REG_DIGIT2);
        Wire.write(SEG[5]);
        Wire.write(0);
        Wire.write(SEG[5]);
        Wire.endTransmission();
        delay(_delay);

}
}

  if(mode==3) {
    for(int i=0;i<=cycles;i++) {
      for(int j=0;j<=3;j++) {
        Wire.beginTransmission(SAA1064_ADDRESS);
        Wire.write(REG_DIGIT1);
        Wire.write(SEG[j]);
        Wire.write(0);
        Wire.write(0);
        Wire.write(0);
        Wire.endTransmission();
        delay(50);
      }
      Wire.beginTransmission(SAA1064_ADDRESS);
      Wire.write(REG_DIGIT1);
      Wire.write(0);
      Wire.write(SEG[3]);
      Wire.endTransmission();
      delay(100);
      Wire.beginTransmission(SAA1064_ADDRESS);
      Wire.write(REG_DIGIT2);  // snake on 3rd digit
      Wire.write(0);
      Wire.write(SEG[3]);
      Wire.endTransmission();
      delay(100);
      Wire.beginTransmission(SAA1064_ADDRESS);
      Wire.write(REG_DIGIT3);  // snake on 4th digit
      Wire.write(0);
      Wire.endTransmission();
      for(int j=3;j<=5;j++) {
        Wire.beginTransmission(SAA1064_ADDRESS);
        Wire.write(REG_DIGIT4);
        Wire.write(SEG[j]);
        Wire.endTransmission();
        delay(50);
      }
      Wire.beginTransmission(SAA1064_ADDRESS);
      Wire.write(REG_DIGIT4);
      Wire.write(SEG[0]);
      Wire.endTransmission();
      delay(100);

      Wire.beginTransmission(SAA1064_ADDRESS);
      Wire.write(REG_DIGIT3);
      Wire.write(SEG[0]);
      Wire.write(0);
      Wire.endTransmission();
      delay(100);

      Wire.beginTransmission(SAA1064_ADDRESS);
      Wire.write(REG_DIGIT2);
      Wire.write(SEG[0]);
      Wire.write(0);
      Wire.endTransmission();
      delay(100);
    }
}

// Blank display before returning
  Wire.beginTransmission(SAA1064_ADDRESS);
  Wire.write(REG_DIGIT1);
  Wire.write(0);
  Wire.write(0);
  Wire.write(0);
  Wire.write(0);
  Wire.endTransmission();
}

void SAA1064::blank()
{
  Wire.beginTransmission(SAA1064_ADDRESS); // SAA1064 address
  Wire.write(REG_DIGIT1);
  Wire.write(B00000000);
  Wire.write(B00000000);
  Wire.write(B00000000);
  Wire.write(B00000000);
  Wire.endTransmission();
}

void SAA1064::dash()
{
  Wire.beginTransmission(SAA1064_ADDRESS); // SAA1064 address
  Wire.write(REG_DIGIT1);
  Wire.write(B01000000);
  Wire.write(B01000000);
  Wire.write(B01000000);
  Wire.write(B01000000);
  Wire.endTransmission();
}
void SAA1064::dashdp()
{
}
// Private Methods /////////////////////////////////////////////////////////////


uint8_t SAA1064::readRegister(uint8_t address)
{
  return 1;
}

void SAA1064::writeRegister(uint8_t address, uint8_t data)
{
}







