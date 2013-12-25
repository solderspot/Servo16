//
//  
//  Servo16_Adafruit.ino
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

// This sketch requires the following libraries:
//   Adafruit_PWMServoDriver: https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library

// include needed librarys
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define NUM_SERVOS 12

// create the servo driver instance
// change 0x40 to match your servo shield if necessary
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);  

// string used to store incomming commands
String cmd = NULL;  

// forward references
String parseTill( char delim, int *pos, String str );

// sketch setup
void setup() 
{
  // init serial port coms
  Serial.begin(9600);

  // init Adafruit's driver and set pulse frequency
  pwm.begin();
  pwm.setPWMFreq(50);  
  
  // tell Servo16 app that we are ready for commands
  Serial.println("Servo16_Adafruit Ready!");
}

// main loop
void loop()
{
  // don't delay loop as serialEvent() will also get delayed
}

// the servo handler for Servorator
void update_servo( int index, int angle)
{
  if (index >= 0 && index < NUM_SERVOS)
  {
    //Serial.println(angle);
    // angles are in tenths of degrees so 180 degrees is actually 1800
    // 4096 ticks is 20,000 us (50Hz)
    // Angle 0 is 500 us
    // angle 1800 is 2,500 us
    long ticks = ((500L + (2000L*angle)/1800L)*4096L)/20000L;
    // update the servo channel with the new pusle
    pwm.setPWM(index, 0, ticks);
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
    //format {s:<servo-index>:<new-angle-in-tenths>}
    String servo = parseTill(':', &pos, str);
    String angle = parseTill(':', &pos, str);
    update_servo(servo.toInt(), angle.toInt());
  }

}

// called after every loop() to handle serial events
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

