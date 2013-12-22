//
//  Servo16Controller.h
//  Servo16
//
//  Created by Thomas Rolfs on 12/21/13.
//  Copyright (c) 2013 Solder Spot. All rights reserved.
//
//	Permission is hereby granted, free of charge, to any person obtaining a
//	copy of this software and associated documentation files (the
//	"Software"), to deal in the Software without restriction, including
//	without limitation the rights to use, copy, modify, merge, publish,
//	distribute, sublicense, and/or sell copies of the Software, and to
//	permit persons to whom the Software is furnished to do so, subject to
//	the following conditions:
//
//	The above copyright notice and this permission notice shall be included
//	in all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#import <Foundation/Foundation.h>
#import "ORSSerialPort/ORSSerialPort.h"

@class ORSSerialPortManager;

#if (MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_7)
@protocol NSUserNotificationCenterDelegate <NSObject>
@end
#endif

@interface Servo16Controller : NSObject <ORSSerialPortDelegate, NSUserNotificationCenterDelegate>

- (IBAction)servoChanged:(id)sender;

@property (unsafe_unretained) IBOutlet NSButton *connectButton;
@property (unsafe_unretained) IBOutlet NSButton *baudButton;
@property (unsafe_unretained) IBOutlet NSTextView *serialOutput;
@property (unsafe_unretained) IBOutlet NSView *view;

@property (nonatomic, strong) ORSSerialPortManager *serialPortManager;
@property (nonatomic, strong) ORSSerialPort *serialPort;
@property (nonatomic, strong) NSArray *availableBaudRates;
@property (nonatomic, readonly) NSInteger defaultBaudRateIndex;

@end

