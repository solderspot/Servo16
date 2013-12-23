//
//  
//  Servo16.ino
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

// include needed librarys
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// create the servo driver instance
// change 0x40 to match your servo shield if necessary
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);  

void setup() 
{
  // init serial port coms
  Serial.begin(9600);

  // init Adafruit's driver and set pulse frequency
  pwm.begin();
  pwm.setPWMFreq(50);  

  // tell Servo16 app that we are ready for commands
  Serial.println("Servo16 Ready!");
}

// helper function that uses angles rather than pulse ticks
void setAngle( int channel, int angle )
{
  // 4096 ticks is 20,000 us (50Hz)
  // Angle 0 is 500 us
  // angle 180 is 2,500 us
  long ticks = ((500L + (2000L*angle)/180L)*4096L)/20000L;
  // update the servo channel with the new pusle
  pwm.setPWM(channel, 0, ticks);
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

// parse command and execute
void execute( String str )
{
  // format {<action name>:<action data>....}
  
  int pos = 0;
  String action = parseTill(':', &pos, str);
  
  if( action == "s" )
  {
    //format {s:<servo-index>:<new-angle>}
    String servo = parseTill(':', &pos, str);
    String angle = parseTill(':', &pos, str);
    setAngle(servo.toInt(), angle.toInt());
  }
}


// main loop
void loop()
{
  // serialEvent() does all the work
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

