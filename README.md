Servo16
=======

Simple OSX app to control servos on an Arduino baord, or any device connected via a serial link.

![image](screenshot.png)


How it Works
------------

The app lets you connect to any serial device using the Port and Baud controls. 

Any change in one of the servo sliders results in a command string being written to the connect port. The command string has the format:

`{s:<servo-num>:<new-angle>}`

The <sero-num> and <new-angle> are integers. The angles are in tenths of degrees and have the range 0..1800.

Changes to the Sero Speed slider generates the command:

`{rate:<servo-num>:<ms-per-degree>}`

This is a global speed slider so <servo-num> will be -1, meaning all servos. <ms-per-degree> is the number of milliseconds it takes to turn one degree.

Changes to the Update Freq slider generates the command:

`{ui:<ms-interval>}`

Where <ms-interval> is the time in milliseconds between servo updates.

The 'ui' and 'rate' commands are meant for use with the [SS_Servorator](https://github.com/solderspot/SS_Servorator) library.

**Note:** If the device sends the string "Ready!" to the app, the app will transmit all the current slider states. 

Examples Sketches
-----------------

Examine the README.md in the 'sketches' subfoler to see examples of how to control servos using Servo16. Some of the examples require additional libraries.

Warning
-------

Be very careful when powering servos. It is possible to cause physical damage and/or serious injury while using them. Use this software at your own risk.

You can see the app in action [in this video clip
](http://youtu.be/Ep0Tf669s-o).
