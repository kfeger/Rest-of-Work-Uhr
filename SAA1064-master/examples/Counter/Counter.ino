// Counter.ino - Example from the SAA1064 library for Arduino
// http://photonicsguy.ca/projects/electronics/7segdisplay

#include <Wire.h>	// This library is required to be included as well
#include <SAA1064.h>

SAA1064 Display1 = SAA1064(0);

void setup() {
  Display1.begin();
  Display1.brightness(5);
  Display1.dash();  // Display dashes
}

void loop() {
  delay(100);  
  for(int i=-500;i<=11000;i++) {
    Display1.print(i);
    delay(20);
    if(i>=0){
      delay(400);
    }
  }
}



