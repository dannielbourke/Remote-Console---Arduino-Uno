//Control and data acquisition via serial port
//By Danniel Bourke 1/04/2020
#include "Arduino.h"
#include "Control.h"

unsigned long shadowMillis = 0 ;
Control myTerminal;

void setup() //SETUP SUBSYSTEMS
{ //Configure port lines (1's are outputs, 0's are inputs)
   DDRB = DDRB | B00111111; DDRD = B11111111; DDRC = B00000000;
   myTerminal.init(); myTerminal.ver();
}
void loop() 
{ //MAIN PROGRAM STARTS HERE
   if (Serial.available() > 0) { if(Serial.peek() == '\r') { Serial.read(); } else { myTerminal.parseToken(); }}
   if (shadowMillis < millis()) { shadowMillis = millis(); myTerminal.tick(); }
}
