class Control // Adjust process variables or recover status through host monitor window
{ // Header as such - { Mode,Option1,Option2,Param1,Param2,\n }
  #define mode 0
  #define option0 1
  #define option1 2
  #define param0 3
  #define param1 4
  #define maxWait 10000 // 10 millisecond timeout
  public:
    Control();
    void ver();
    void init();
    void tick();
    void purge();
    char command[5];
    uint8_t state;
    boolean receiving = 0;
    boolean getValue();
    boolean setValue();
    boolean parseToken();
    unsigned long comLastTime;
};

Control::Control(){ state = 0; }

void Control::ver() { Serial.println("\nControl 1-4-20"); }

void Control::init() { Serial.begin(9600); Serial.setTimeout(10); delayMicroseconds(10); }

void Control::tick() { if(micros() > (comLastTime + 10000)) { command[mode] = '\0'; } }

void Control::purge() 
{ 
  while(Serial.available() > 0){ Serial.read(); } // Remove all chars from receive buffer
  command[mode] = '\0'; state = 0; receiving = false; return; 
}

boolean Control::parseToken()
{ 
  if(!receiving) // Start - First char since last time out
  {
    char incoming = Serial.read();
    if(incoming == '?' || incoming == '!') { command[mode] = incoming; receiving = true; state = 1; comLastTime = micros(); }
      else { purge(); } // Not a valid starting character
  }
  else // Receiving now
  {
    if(micros() > (comLastTime + maxWait)) { Serial.println("timeout"); purge(); } // Don't care, too late
    else // Time ok
    {
      comLastTime = micros();
      if(Serial.peek() == -1){ Serial.println("Missing value"); }  // Incomplete string - string will be purged
      char incoming = Serial.read(); // Remove item from serial-in buffer
      switch(state)
      {
        case 1: // Get first single decimal of parameter
          if( incoming >= 48 && incoming <= 57) { command[option1] = (incoming - 48); state = 2; } 
            else { Serial.println("Error?"); purge(); }   
        break;
        
        case 2: // Get second single decimal of parameter
          if( incoming >= 48 && incoming <= 57) { command[option0] = (incoming - 48); state = 3; } 
            else { Serial.println("Error?"); purge(); }
        break;
        
        case 3: // Get first single decimal of value
          if(incoming == '\n') { if(command[mode] == '?') { getValue(); purge(); }}
          else 
          {
            if(command[mode] == '!')
            {
              if( incoming >= 48 && incoming <= 57) { command[param1] = (incoming - 48); state = 4; }
                else { Serial.println("Case3 Error?"); purge(); }
            }
          }
        break;

        case 4: // Get second single decimal of value
          if( incoming >= 48 && incoming <= 57) { command[param0] = (incoming - 48); state = 5; }
            else { Serial.println("Case4 Error?"); purge(); }    
        break;

        case 5:
          if(incoming == '\n') { setValue(); }
          purge();
        break; 
      } // switch  
    } // time ok
  } // receiving now
} // parseToken

boolean Control::getValue()
{ 
  int combined = ((10 * command[option1]) + command[option0]); 
  //These commands are all similar so use one statement for this example
  //Ports D + B digital
  if(combined <14){ Serial.println(uint8_t(digitalRead(combined))); } 
  else
  {
  if(combined > 19){ combined = 20; }
  switch(combined)
  { // An example of commands for things which might not be similar     
    //PortC analog
    case 14: Serial.println((float(analogRead(A0)*5)/1024)); break;
    case 15: Serial.println((float(analogRead(A1)*5)/1024)); break;
    case 16: Serial.println((float(analogRead(A2)*5)/1024)); break;
    case 17: Serial.println((float(analogRead(A3)*5)/1024)); break;
    case 18: Serial.println((float(analogRead(A4)*5)/1024)); break;
    case 19: Serial.println((float(analogRead(A5)*5)/1024)); break;
    case 20: Serial.println("Out of range value"); break;
    //Upto 100 queries
  } // switch
  }//else
} // nextLine

boolean Control::setValue()
{ 
  int fullParam = ((10* command[param1]) + (command[param0])); 
  int combined = ((10 * command[option1]) + command[option0]); 
  if(combined > 13){ return; }
  //For different kinds of command use command-switch like above
  if(combined == 0){ Serial.print("\n"); Serial.print(fullParam, DEC); Serial.println(" received"); }
    else { if(fullParam == 00){digitalWrite(combined,LOW); } if(fullParam == 01){digitalWrite(combined,HIGH); }}
} //setValue
