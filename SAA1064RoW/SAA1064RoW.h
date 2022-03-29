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

#ifndef SAA1064RoW_h
#define SAA1064RoW_h

#include "Arduino.h"
#include <inttypes.h>

#include "RoW_pcb_version.h"

// hier die PCB-Version definieren
//#define PCB_VERSION1
//#define PCB_VERSION2

class SAA1064RoW
{
  private:
	uint8_t _bright;
  protected:
	uint8_t _Address;
//	uint8_t _bright;
	uint8_t readRegister(uint8_t);
	void writeRegister(uint8_t, uint8_t);
  public:
    SAA1064RoW(uint8_t);
	void begin(uint8_t);
	void begin();
	void brightness(uint8_t);
	void print(int);
	void print(int, int);
	void print(double);
    void printDate(int num);
	void printTime(int, bool);
	void printYear(int);
	void printTemp(int);
	void printHum(int);
	void printDashDash(int);
	void print4Dot();
    void printDotDot();
	void dash();
	void blank();
	void dashdp();
    void printAP();
    void printFun();
    void printLongFun();
};

#endif
