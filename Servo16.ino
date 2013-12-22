//
//  
//  Servo16.ino
//
//  Created by Thomas Rolfs on 12/20/13.
//  Copyright (c) 2013 Solder Spot. All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the
//  "Software"), to deal in the Software without restriction, including
//  without limitation the rights to use, copy, modify, merge, publish,
//  distribute, sublicense, and/or sell copies of the Software, and to
//  permit persons to whom the Software is furnished to do so, subject to
//  the following conditions:
//  
//  The above copyright notice and this permission notice shall be included
//  in all copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// include needed librarys
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// create servo driver
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);  // change 0x40 to match your servo shield if necessary
String input;

void setup() 
{
  Serial.begin(9600);
  input.reserve(200);
  pwm.begin();
  pwm.setPWMFreq(50);  
  for(int i=0; i<16; i++)
  {
    setAngle(i,90);
  }
  Serial.println("Servo16 Ready!");
}

// helper function that uses angles rather than pulse ticks
void setAngle( int channel, int angle )
{
  Serial.print("S:");
  Serial.print( channel );
  Serial.print(":");
  Serial.println( angle );
  
  
  // 4096 ticks is 20,000 us
  // Angle 0 is 500 us
  // angle 180 is 2,500 us
  long ticks = ((500L + (2000L*angle)/180L)*4096L)/20000L;
  // update the servo channel with the new pusle
  pwm.setPWM(channel, 0, ticks);
  
}

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

void execute( String input )
{
  int pos = 0;
  String cmd = parseTill(':', &pos, input);
  String servo = parseTill(':', &pos, input);
  String angle = parseTill(':', &pos, input);
  
  if( cmd == "s" )
  {
    setAngle(servo.toInt(), angle.toInt());
  }
}


// main loop
void loop()
{

}

void serialEvent() 
{
  static int cmd = 0;
  
  while (Serial.available()) 
  {
    char ch = (char)Serial.read(); 
    if(cmd)
    {
      if( ch =='}')
      {
        execute(input);
        cmd = 0;
      }
      else
      {
        input += ch;
      }
    }
    else
    {
      if( ch == '{' )
      {
        cmd = 1;
        input = "";
      }
    }
  }
}

