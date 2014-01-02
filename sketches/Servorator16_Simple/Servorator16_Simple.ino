//
//  
//  Servorator16_Simple.ino
//
// Copyright (c) 2013-2014, Solder Spot
// All rights reserved. 

// This sketch requires the following library:
//   SS_Servorator: https://github.com/solderspot/SS_Servorator

// include needed librarys
#include <Servo.h>
#include <SS_Servorator.h>


#define NUM_SERVOS 6

Servo servo[NUM_SERVOS];
SS_Servorator sr(NUM_SERVOS);

// forward references
SS_ServoHandler update_servo;
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
  
  // register servo handler
  sr.setServoHandler( update_servo, NULL);
  
  // tell Servo16 app that we are ready for commands
  Serial.println("Servorator16_Simple Ready!");
}

// main loop
void loop()
{
  // sr.service() needs to be called regularly so that
  // the servos are updated
  sr.service();
  // sr.service() will call the servo handler when needed 
  
  // don't delay loop as serialEvent() will be delayed too
}

// the servo handler for Servorator
void update_servo( SS_Index index, SS_Angle angle, void *data)
{
  // SS_Angle is in 1000th of degrees
  //Serial.println(angle/1000);
  servo[index].write( angle/1000L);
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
    String servo = parseTill(':', &pos, str);
    String angle = parseTill(':', &pos, str);
    sr.setServoTargetAngle(servo.toInt(), angle.toInt());
  }
  else if ( action == "rate" )
  {
    //format {rate:<servo>:<1000th degrees per second>}
    String servo = parseTill(':', &pos, str);
    String dps = parseTill(':', &pos, str);
    SS_Index s = servo.toInt();
    sr.setServoMaxVelocity( s, dps.toInt());
  }
  else if ( action == "ui")
  {
    //format {si:<ms>}
    String ms = parseTill(':', &pos, str);
    sr.setUpdateInterval( ms.toInt());
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

