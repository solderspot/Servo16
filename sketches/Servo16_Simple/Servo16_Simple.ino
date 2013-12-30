//
//  
//  Servo16_Simple.ino
//
// Copyright (c) 2013, Solder Spot
// All rights reserved. 

// include needed librarys
#include <Servo.h>


#define NUM_SERVOS 6

Servo servo[NUM_SERVOS];

// forward reference
String parseTill( char delim, int *pos, String str );

void setup() 
{
  // init serial port coms
  Serial.begin(9600);

  // assign PWM pins to servos
  servo[0].attach(3);
  servo[1].attach(5);
  servo[2].attach(6);
  servo[3].attach(9);
  servo[4].attach(10);
  servo[5].attach(11);
  
  // tell Servo16 app that we are ready for commands
  Serial.println("Servo16_Simple Ready!");
}

// main loop
void loop()
{
  // don't delay loop as serialEvent() will be delayed too
}

// the servo handler for Servorator
void update_servo( int index, long angle)
{
  if (index >= 0 && index < NUM_SERVOS)
  {
    // angle is in 1000ths of degrees
    Serial.println(angle/1000L);
    servo[index].write( angle/1000L);
  }
}

// parse command and execute
void execute( String str )
{
  // format {<action name>:<action data>....}
  
  int pos = 0;
  String action = parseTill(':', &pos, str);
  
  if( action == "s" )
  {
    //format {s:<servo-index>:<new-angle-in-1000ths>}
    String index = parseTill(':', &pos, str);
    String angle = parseTill(':', &pos, str);
    update_servo(index.toInt(), angle.toInt());
  }
}

String cmd = NULL;  // used to store incomming command

// called after every loop() to handle serial coms
void serialEvent() 
{
  
  while (Serial.available()) 
  {
    char ch = (char)Serial.read();
    
    if(cmd)
    {
      if( ch =='}')
      {
        execute(cmd);
        // wait for new command
        cmd = NULL;
      }
      else
      {
        cmd += ch;
      }
    }
    else
    {
      if( ch == '{' )
      {
        // start of new command
        cmd = "";
      }
    }
  }
}

// returns substring of str from pos till delim
String parseTill( char delim, int *pos, String str )
{
  int len = str.length();
  int index = *pos;
  String p = "";
  while ( index<len )
  {
    char ch = str.charAt(index++);
    
    if( ch == delim)
    {
      break;
    }
    p += ch;
  }
  
  *pos = index;
  return p;
}

