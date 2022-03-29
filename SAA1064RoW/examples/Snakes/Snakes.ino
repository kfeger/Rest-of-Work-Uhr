// Snakes.ino - Example from the SAA1064 library for Arduino
// http://photonicsguy.ca/projects/electronics/7segdisplay

#include <Wire.h>	// This library is required to be included as well
#include <SAA1064.h>

SAA1064 Display1 = SAA1064(0);

void setup() {
  Display1.begin();

  Display1.brightness(5);
  Display1.dash();  // Display dashes
  delay(500);
  Display1.brightness(1);
  Display1.snake(0,0);
  Display1.brightness(4);
  Display1.snake(0,0);
  Display1.brightness(7);
  Display1.snake(0,0);
  Display1.brightness(5);
}

void loop() {
//                 x - Pattern (1-3)
//                   i - Loop iterations
//  Display1.snake(x,i);
  Display1.snake(1,3);  // Different snake animations
  Display1.snake(2,3);  // Different snake animations

  delay(1000);  
}



