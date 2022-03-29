/*
SAA1064RoW.h - Part of the SAA1064RoW library for Arduino
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

#include "SAA1064RoW.h"
#include <Wire.h>

/******************************************************************************
 * Definitions
 ******************************************************************************/

byte DIGIT[] = { 0x3F, 0x6, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x7, 0x7F, 0x67, 0x77, 0x7C, 0x58, 0x5E, 0x79, 0x71, 0x40, 0x63, 0x50, 0x80, 0x1E }; // Extra DP (DP) for error buffer.
byte SEG[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
byte START_UP[] = {0x1,0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x81, 0x83, 0x87, 0x8f, 0x9f, 0xbf, 0xff, 0x00};
uint32_t PROGMEM PlayLong[] = {
0x00000080,
0x00008000,
0x00800000,
0x80000000,
0x01000000,
0x00010000,
0x00000100,
0x00000001,
0x00000040,
0x00004000,
0x00400000,
0x40000000,
0x08000000,
0x00080000,
0x00000800,
0x00000008,
0x00000004,
0x00000010,
0x00000400,
0x00001000,
0x00040000,
0x00100000,
0x04000000,
0x10000000,
0x20000000,
0x02000000,
0x00200000,
0x00020000,
0x00002000,
0x00000200,
0x00000020,
0x00000002,
0x0000007f,
0x00007f7f,
0x007f7f7f,
0x7f7f7f7f,
0x7f7f7f00,
0x7f7f0000,
0x7f000000,
0x80000000,
0x80800000,
0x80808000,
0x80808080
};

#define	SAA1064_ADDRESS 0x38
#define REG_CONTROL 0x00

#define REG_DIGIT1 0x01
#define REG_DIGIT2 0x02
#define REG_DIGIT3 0x03
#define REG_DIGIT4 0x04


#define REG_CON_BITS 
#define CONTROL_BYTE 0x07
#define DP 0x80
//#define LOW 0x01
//#define MED 0x04
//#define HIGH 0x07
volatile uint8_t _bright;


/******************************************************************************
 * Constructors
 ******************************************************************************/

SAA1064RoW::SAA1064RoW(uint8_t address)
{
  _Address = address;	// TODO Implement different subaddresses
}

#ifdef PCB_VERSION2    // Platine Platine Version 2
    #pragma message ("SAA1064RoW: Platine Version 2")
    void WriteByteToWire(byte dig1, byte dig2, byte dig3, byte dig4) {
        Wire.beginTransmission(SAA1064_ADDRESS); // SAA1064RoWV2 address
        Wire.write(B00000001);
        Wire.write(dig1);   // Digit 1
        Wire.write(dig2);  // Digit 2
        Wire.write(dig3);  // Digit 3
        Wire.write(dig4);   // Digit 4
        Wire.endTransmission();
    }

    void WriteLongToWire(uint32_t Data) {
        Wire.beginTransmission(SAA1064_ADDRESS); // SAA1064RoWV2 address
        Wire.write(B00000001);
        Wire.write((Data >>24) & 0xff);   // Digit 1
        Wire.write((Data >>16) & 0xff);  // Digit 2
        Wire.write((Data >>8) & 0xff);  // Digit 3
        Wire.write(Data & 0xff);   // Digit 4
        Wire.endTransmission();
    }
#else   // Platine Version 1
    #pragma message ("SAA1064RoW: Platine Version 1")
    void WriteByteToWire(byte dig1, byte dig2, byte dig3, byte dig4) {
        Wire.beginTransmission(SAA1064_ADDRESS); // SAA1064RoW address
        Wire.write(B00000001);
        Wire.write(dig3);   // Digit 1
        Wire.write(dig4);  // Digit 2
        Wire.write(dig1);  // Digit 3
        Wire.write(dig2);   // Digit 4
        Wire.endTransmission();
    }

    void WriteLongToWire(uint32_t Data) {
        Wire.beginTransmission(SAA1064_ADDRESS); // SAA1064RoW address
        Wire.write(B00000001);
        Wire.write((Data >>8) & 0xff);  // Digit 3
        Wire.write(Data & 0xff);   // Digit 4
        Wire.write((Data >>24) & 0xff);   // Digit 1
        Wire.write((Data >>16) & 0xff);  // Digit 2
        Wire.endTransmission();
    }
//#else
//    #error ("SAA1064RoW: Platine nicht oder falsch definiert")
#endif

/******************************************************************************
 * User API
 ******************************************************************************/
void SAA1064RoW::begin(uint8_t address)
{
}
void SAA1064RoW::begin()
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

void SAA1064RoW::print(int number)
{
  int g=0;
  int h=0;
  int j=0;

  int er1000;	// 1000er
  int er100;	// 100er
  int er10;	// 10er
  int er1;	// 1er
  boolean negative=false;
  
  if (number>9999 || number<-999) {
      WriteByteToWire(DIGIT[16], DIGIT[16], DIGIT[16]|DP, DIGIT[16]);
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

  er1000=DIGIT[g];
  er100=DIGIT[h];
  er10=DIGIT[j];
  er1=DIGIT[number];



  if(g==0) {
    er1000=0;  // Blank thousands if 0
    if(h==0) {
      er100=0;  // Blank hundreds if 0
      if(j==0) {
        er10=0;  // Blank tens if 0
      }
    }
  }
  if(negative) {
    er1000=DIGIT[16];
  }
  WriteByteToWire(er1000, er100, er10, er1);
}

void SAA1064RoW::print(int number, int Dot)
{
  int g=0;
  int h=0;
  int j=0;

  int er1000;
  int er100;
  int er10;
  int er1;
  boolean negative=false;

  if (number>9999 || number<-999) {
      WriteByteToWire(DIGIT[16], DIGIT[16], DIGIT[16]|DP, DIGIT[16]);
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

  er1000=DIGIT[g];	//1000er
  er100=DIGIT[h];	//100er
  er10=DIGIT[j];	//10er
  er1=DIGIT[number];	//1er


    if (Dot != 5) {
        if(g==0) {
            er1000=0;  // Blank thousands if 0
            if(h==0) {
                er100=0;  // Blank hundreds if 0
                if(j==0) {
                    er10=0;  // Blank tens if 0
                }
            }
        }
    }
    else {          // Ist ein Datum, Tag und Monat ohne führende 0
        if(g == 0)
            er1000 = 0;
        if(j == 0)
            er10 = 0;
    }
  if(g == 0) {
	  er1000 = 0;
  }
  
  if(negative) {
    er1000=DIGIT[16];
  }
  byte b1, b2, b3, b4;
	if((Dot & 8) && !negative) {
        b1 = er1000 |DP;
	}
	else
        b1 = er1000;
	if(Dot & 4) {
        b2 = er100 | DP;
	}
	else
        b2 = er100;
	if(Dot & 2) {
        b3 = er10 | DP;
	}
	else
        b3 = er10;
	if(Dot & 1) {
        b4 = er1 | DP;
	}
	else
        b4 = er1;
	
  WriteByteToWire(b1, b2, b3, b4);
}

void SAA1064RoW::printTime(int number, bool Dot = true)
{
  int g=0;
  int h=0;
  int j=0;

  int er1000;
  int er100;
  int er10;
  int er1;
  boolean negative=false;

  g=number/1000;
  number=number-(g*1000);
  h=number/100;
  number=number-(h*100);
  j=number/10;
  number=number-(j*10);

  er1000=DIGIT[g];	// 1000er
  er100=DIGIT[h];	// 100er
  er10=DIGIT[j];	// 10er
  er1=DIGIT[number];	// 1er

  if(g == 0) {
	  er1000 = 0;
  }
  
    WriteByteToWire(er1000,(Dot?(er100|DP):er100), er10, er1);
}

void SAA1064RoW::print(double num)
{
  int number=num*10;
  int g=0;
  int h=0;
  int j=0;

  int er1000;
  int er100;
  int er10;
  int er1;
  boolean negative=false;

  if(number>9999 || number<-999) {
    WriteByteToWire(DIGIT[16], DIGIT[16], DIGIT[16], DIGIT[16]);
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

  er1000=DIGIT[g];
  er100=DIGIT[h];
  er10=DIGIT[j]|DP;
  er1=DIGIT[number];



  if(g==0) {
    er1000=0;  // Blank thousands if 0
    if(h==0) {
      er100=0;  // Blank hundreds if 0
      if(j==0) {
        er10=0;  // Blank tens if 0
      }
    }
  }
  if(negative) {
    er1000=DIGIT[16];
  }
  
    WriteByteToWire(er1000, er100, er10, er1);
}

void SAA1064RoW::printDate(int num)
{
  int number=num*10;
  int g=0;
  int h=0;
  int j=0;

  int er1000;
  int er100;
  int er10;
  int er1;

  g=number/1000;
  number=number-(g*1000);
  h=number/100;
  number=number-(h*100);
  j=number/10;
  number=number-(j*10);

  er1000 = DIGIT[g];
  er100=DIGIT[h] | DP;    // Tag-Punkt
  er10 = DIGIT[j];
  er1=DIGIT[number] | DP; // Monat-Punkt
  
  byte b1, b2, b3, b4;
  
  b2 = er100;
  b4 = er1;
  if (er1000 == DIGIT[0])
      b1 = 0;
  else
    b1 = er1000;
  if(er10 == DIGIT[0])
      b3 = 0;
  else
    b3 = er10;

  WriteByteToWire(b1, b2, b3, b4);

}

void SAA1064RoW::printTemp(int number)	//Temperatur -99..99 Grad C
{
  int g=0;
  int h=0;
  int j=0;

  int er1000;
  int er100;
  int er10;
  int er1;
  boolean negative=false;

  Wire.beginTransmission(SAA1064_ADDRESS);
  Wire.write(B00000001);
  if(number>99 || number<-99) {
      WriteByteToWire(DIGIT[16], DIGIT[16], DIGIT[16], DIGIT[17]);
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

  er1000=DIGIT[g];
  er100=DIGIT[h];
  er10=DIGIT[j];
  er1=DIGIT[number];



  if(g==0) {
    er1000=0;  // Blank thousands if 0
    if(h==0) {
      er100=0;  // Blank hundreds if 0
      if(j==0) {
        er10=0;  // Blank tens if 0
      }
    }
  }
  if(negative) {
    er1000=DIGIT[16];
  }
  
  WriteByteToWire(er100, er10, er1, DIGIT[17]);
  
}

void SAA1064RoW::printHum(int number)	//Feuchte 0..100 %rH
{
  int g=0;
  int h=0;
  int j=0;

  int er1000;
  int er100;
  int er10;
  int er1;
  boolean negative=false;

  if(number>100 || number<0) {
      WriteByteToWire(DIGIT[16], DIGIT[16], DIGIT[16], DIGIT[18]);
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

  er1000=DIGIT[g];
  er100=DIGIT[h];
  er10=DIGIT[j];
  er1=DIGIT[number];



  if(g==0) {
    er1000=0;  // Blank thousands if 0
    if(h==0) {
      er100=0;  // Blank hundreds if 0
      if(j==0) {
        er10=0;  // Blank tens if 0
      }
    }
  }
  if(negative) {
    er1000=DIGIT[16];
  }
  
  WriteByteToWire(er100, er10, er1, DIGIT[18]);
}


void SAA1064RoW::printYear(int number)	//Temperatur -99..99 Grad C
{
  int g=0;
  int h=0;
  int j=0;

  int er1000;
  int er100;
  int er10;
  int er1;
  boolean negative=false;

  if(number>999 || number< -99) {
      WriteByteToWire(DIGIT[16], DIGIT[16], DIGIT[16], DIGIT[17]);
    return;
  }

  if(number<0) {
    number*=-1;
    negative=true;
  }
  g=number/1000;
  number=number-(g*1000);
  h=number/100;
  number=number-(h*100);
  j=number/10;
  number=number-(j*10);

  er1000=DIGIT[g];
  er100=DIGIT[h];
  er10=DIGIT[j];
  er1=DIGIT[number];



  if(g==0) {
    er1000=0;  // Blank thousands if 0
    if(h==0) {
      er100=0;  // Blank hundreds if 0
      if(j==0) {
        er10=0;  // Blank tens if 0
      }
    }
  }
  
  if(negative) {
    //           -  10er  1er   J 
    WriteByteToWire(64, er10, er1, 30);
  }
  else {
    WriteByteToWire(er100, er10, er1, 30);
  }


  Wire.endTransmission();

}

void SAA1064RoW::printDashDash (int dd) {
	if (dd == 1) {		// Grad C
        WriteByteToWire(0, DIGIT[16], DIGIT[16], DIGIT[17]);
		}
	else if (dd == 2) {	// %rH
        WriteByteToWire(0, DIGIT[16], DIGIT[16], DIGIT[18]);
		}
	else {	// nüscht
        WriteByteToWire(0, DIGIT[16], DIGIT[16], 0);
		}
}

void SAA1064RoW::print4Dot (void) {
    WriteByteToWire(DP, DP, DP, DP);
}

void SAA1064RoW::printDotDot (void) {
    WriteByteToWire(DP, 0, 0, DP);
}


void SAA1064RoW::brightness(uint8_t bright)	// 1 thru 7
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

void SAA1064RoW::blank()
{
  WriteByteToWire(0, 0, 0, 0);
}

void SAA1064RoW::dash()
{
  WriteByteToWire(B01000000, B01000000, B01000000, B01000000);
}

void SAA1064RoW::printAP()    // -AP-
{
  WriteByteToWire(64, 119, 115, 64);
}

void SAA1064RoW::dashdp()
{
}
// Private Methods /////////////////////////////////////////////////////////////


uint8_t SAA1064RoW::readRegister(uint8_t address)
{
  return 1;
}

void SAA1064RoW::writeRegister(uint8_t address, uint8_t data)
{
}

void SAA1064RoW::printFun() {
    int i = 0;
    for(int i = 0; i < 16; i++) {
        WriteByteToWire(START_UP[i], 0, 0, 0);
        delay(100);
    }    
    for(int i = 0; i < 16; i++) {
        WriteByteToWire(0, START_UP[i], 0, 0);
        delay(100);
    }    
    for(int i = 0; i < 16; i++) {
        WriteByteToWire(0, 0, START_UP[i], 0);
        delay(100);
    }    
    for(int i = 0; i < 16; i++) {
        WriteByteToWire(0, 0, 0, START_UP[i]);
        delay(100);
    }    
    for(int i = 0; i < 16; i++) {
        WriteByteToWire(START_UP[i], START_UP[i], START_UP[i], START_UP[i]);
        delay(100);
    }    
}

void SAA1064RoW::printLongFun(void) {
    uint32_t DisplayData;
    for(int i = 0; i < sizeof(PlayLong)/4; i++) {
        DisplayData = pgm_read_dword(&PlayLong[i]);
        WriteLongToWire(DisplayData);
        delay(250);
    }
}







