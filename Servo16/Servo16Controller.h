//
//  Serv016Controller.h
//  Servo16
//
//  Created by Thomas Rolfs on 12/21/13.
//  Copyright (c) 2013 Solder Spot. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ORSSerialPort/ORSSerialPort.h"

@class ORSSerialPortManager;

#if (MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_7)
@protocol NSUserNotificationCenterDelegate <NSObject>
@end
#endif

@interface Servo16Controller : NSObject <ORSSerialPortDelegate, NSUserNotificationCenterDelegate>

- (IBAction)servoChanged:(id)sender;

@property (unsafe_unretained) IBOutlet NSTextField *portStatus;
@property (unsafe_unretained) IBOutlet NSTextView *serialOutput;

@property (nonatomic, strong) ORSSerialPortManager *serialPortManager;
@property (nonatomic, strong) ORSSerialPort *serialPort;
@property (nonatomic, strong) NSArray *availableBaudRates;
@property (nonatomic, readonly) NSInteger defaultBaudRateIndex;

@end

