Example Sketches
================

There are four example sketches for use with the Servo16 app. You can copy their folders into to the Arduino sketch book folder to try them out. Some examples require additional libraries.

  *  **Servo16_Simple**
  
  	 This is simplest example. It only uses the Arduinos Servo library. It also only supports the `{s:<servo-num>:<angle>}` command, so the 'Servo Speed' and 'Update Freq' sliders have no effect. 
  	 
  *  **Servo16_Adafruit**
  
  	 This example is similar to Servo16_Simple but uses the [Adafruit-PWM-Servo-Driver-Library](http://https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library) to control servos on their 16 channel servo shield. Again, only the `{s:<servo-num>:<angle>}` command is supported.

  *  **Servorator16_Simple**
  
  	 This example uses the [SS_Servorator Library](https://github.com/solderspot/SS_Servorator.git) to provide speed control to servo movements. The 'Servo Speed' and 'Update Freq' sliders are supported. 
  	 
  *  **Servorator16_Adafruit**
  
     Similar to Servorator16_Simple but the Adafruit library is used as in the Servo16_Adafruit. example.
