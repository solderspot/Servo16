//
//  
//  Servorator16_Simple.ino
//
//  Copyright (c) 2013 Solder Spot. All rights reserved.
//
//  Software License Agreement (BSD License)
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the distribution.
//  3. Neither the name of the copyright holders nor the
//  names of its contributors may be used to endorse or promote products
//  derived from this software without specific prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
  // SS_Angle is in tenths of a degree
  //Serial.println(angle/10);
  servo[index].write( angle/10);
}

// parse command and execute
void execute( String str )
{
  // format {<action name>:<action data>....}
  
  int pos = 0;
  String action = parseTill(':', &pos, str);
  
  if( action == "s" )
  {
    //format {s:<servo-index>:<new-angle-in-tenths>}
    String servo = parseTill(':', &pos, str);
    String angle = parseTill(':', &pos, str);
    sr.setServoAngle(servo.toInt(), angle.toInt());
  }
  else if ( action == "rate" )
  {
    //format {rate:<servo>:<ms per degree>}
    String servo = parseTill(':', &pos, str);
    String ms = parseTill(':', &pos, str);
    SS_Index s = servo.toInt();
    if( s < 0 )
    {
      sr.setMaxRate( ms.toInt());
    }
    else
    {
      sr.setServoMaxRate( s, ms.toInt());
    }
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

