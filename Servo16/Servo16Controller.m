//
//  Servo16Controller.m
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

#import "Servo16Controller.h"
#import "ORSSerialPort/ORSSerialPortManager.h"
#import "ORSSerialPort/ORSSerialPort.h"

@implementation Servo16Controller

- (id)init
{
    self = [super init];
    if (self)
	{
        self.serialPortManager = [ORSSerialPortManager sharedSerialPortManager];
		self.availableBaudRates = [NSArray arrayWithObjects: [NSNumber numberWithInteger:300], [NSNumber numberWithInteger:1200], [NSNumber numberWithInteger:2400], [NSNumber numberWithInteger:4800], [NSNumber numberWithInteger:9600], [NSNumber numberWithInteger:14400], [NSNumber numberWithInteger:19200], [NSNumber numberWithInteger:28800], [NSNumber numberWithInteger:38400], [NSNumber numberWithInteger:57600], [NSNumber numberWithInteger:115200], [NSNumber numberWithInteger:230400],
								   nil];
        _defaultBaudRateIndex = 4;
		
		NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
		[nc addObserver:self selector:@selector(serialPortsWereConnected:) name:ORSSerialPortsWereConnectedNotification object:nil];
		[nc addObserver:self selector:@selector(serialPortsWereDisconnected:) name:ORSSerialPortsWereDisconnectedNotification object:nil];
        
#if (MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_7)
		[[NSUserNotificationCenter defaultUserNotificationCenter] setDelegate:self];
#endif
    }
    return self;
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

-(void) output:(NSString*) string
{
    NSUInteger len = [[self.serialOutput string] length];
	[self.serialOutput.textStorage.mutableString appendString:string];
    [self.serialOutput scrollRangeToVisible:NSMakeRange(len , 0)];
	[self.serialOutput setNeedsDisplay:YES];
}

#pragma mark - Actions

- (IBAction)openOrClosePort:(id)sender
{
	self.serialPort.isOpen ? [self.serialPort close] : [self.serialPort open];
}

- (IBAction)servoChanged:(NSSlider*)sender
{
    NSString *string = [NSString stringWithFormat:@"Servo %d: %d\n", (int) [sender tag], [sender intValue]];
	NSData *dataToSend = [[NSString stringWithFormat:@"{s:%d:%d}", (int) [sender tag], [sender intValue]] dataUsingEncoding:NSUTF8StringEncoding];
	[self.serialPort sendData:dataToSend];
    //[self output:string];
}

#pragma mark - ORSSerialPortDelegate Methods

- (void)serialPortWasOpened:(ORSSerialPort *)serialPort
{
	self.connectButton.title = @"Close";
    self.baudButton.enabled = NO;
    [self output:@"Connected\n"];
}

- (void)serialPortWasClosed:(ORSSerialPort *)serialPort
{
	self.connectButton.title = @"Connect";
    self.baudButton.enabled = YES;
    [self output:@"Disonnected\n"];
}

- (void)serialPort:(ORSSerialPort *)serialPort didReceiveData:(NSData *)data
{
	NSString *string = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
	if ([string length] == 0) return;
    [self output:string];
}

- (void)serialPortWasRemovedFromSystem:(ORSSerialPort *)serialPort;
{
	// After a serial port is removed from the system, it is invalid and we must discard any references to it
	self.serialPort = nil;
	self.connectButton.title = @"Connect";
}

- (void)serialPort:(ORSSerialPort *)serialPort didEncounterError:(NSError *)error
{
	NSLog(@"Serial port %@ encountered an error: %@", serialPort, error);
    [self output:[NSString stringWithFormat:@"Error: %@\n", [error.userInfo objectForKey:NSLocalizedDescriptionKey]]];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
	NSLog(@"%s %@ %@", __PRETTY_FUNCTION__, object, keyPath);
	NSLog(@"Change dictionary: %@", change);
}

#pragma mark - NSUserNotificationCenterDelegate

#if (MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_7)

- (void)userNotificationCenter:(NSUserNotificationCenter *)center didDeliverNotification:(NSUserNotification *)notification
{
	dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, 3.0 * NSEC_PER_SEC);
	dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
		[center removeDeliveredNotification:notification];
	});
}

- (BOOL)userNotificationCenter:(NSUserNotificationCenter *)center shouldPresentNotification:(NSUserNotification *)notification
{
	return YES;
}

#endif

#pragma mark - Notifications

- (void)serialPortsWereConnected:(NSNotification *)notification
{
	NSArray *connectedPorts = [[notification userInfo] objectForKey:ORSConnectedSerialPortsKey];
	NSLog(@"Ports were connected: %@", connectedPorts);
	[self postUserNotificationForConnectedPorts:connectedPorts];
    [self output:[NSString stringWithFormat:@"Ports were connected: %@\n", connectedPorts]];
}

- (void)serialPortsWereDisconnected:(NSNotification *)notification
{
	NSArray *disconnectedPorts = [[notification userInfo] objectForKey:ORSDisconnectedSerialPortsKey];
	NSLog(@"Ports were disconnected: %@", disconnectedPorts);
    [self postUserNotificationForDisconnectedPorts:disconnectedPorts];
    [self output:[NSString stringWithFormat:@"Ports were disconnected: %@\n", disconnectedPorts]];
	
}

- (void)postUserNotificationForConnectedPorts:(NSArray *)connectedPorts
{
#if (MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_7)
	if (!NSClassFromString(@"NSUserNotificationCenter")) return;
	
	NSUserNotificationCenter *unc = [NSUserNotificationCenter defaultUserNotificationCenter];
	for (ORSSerialPort *port in connectedPorts)
	{
		NSUserNotification *userNote = [[NSUserNotification alloc] init];
		userNote.title = NSLocalizedString(@"Serial Port Connected", @"Serial Port Connected");
		NSString *informativeTextFormat = NSLocalizedString(@"Serial Port %@ was connected to your Mac.", @"Serial port connected user notification informative text");
		userNote.informativeText = [NSString stringWithFormat:informativeTextFormat, port.name];
		userNote.soundName = nil;
		[unc deliverNotification:userNote];
	}
#endif
}

- (void)postUserNotificationForDisconnectedPorts:(NSArray *)disconnectedPorts
{
#if (MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_7)
	if (!NSClassFromString(@"NSUserNotificationCenter")) return;
	
	NSUserNotificationCenter *unc = [NSUserNotificationCenter defaultUserNotificationCenter];
	for (ORSSerialPort *port in disconnectedPorts)
	{
		NSUserNotification *userNote = [[NSUserNotification alloc] init];
		userNote.title = NSLocalizedString(@"Serial Port Disconnected", @"Serial Port Disconnected");
		NSString *informativeTextFormat = NSLocalizedString(@"Serial Port %@ was disconnected from your Mac.", @"Serial port disconnected user notification informative text");
		userNote.informativeText = [NSString stringWithFormat:informativeTextFormat, port.name];
		userNote.soundName = nil;
		[unc deliverNotification:userNote];
	}
#endif
}


#pragma mark - Properties

@synthesize connectButton = _connectButton;
@synthesize baudButton = _baudButton;
@synthesize serialOutput = _serialOutput;
@synthesize serialPortManager = _serialPortManager;

- (void)setSerialPortManager:(ORSSerialPortManager *)manager
{
	if (manager != _serialPortManager)
	{
		[_serialPortManager removeObserver:self forKeyPath:@"availablePorts"];
		_serialPortManager = manager;
		NSKeyValueObservingOptions options = NSKeyValueObservingOptionNew | NSKeyValueObservingOptionOld;
		[_serialPortManager addObserver:self forKeyPath:@"availablePorts" options:options context:NULL];
	}
}

@synthesize serialPort = _serialPort;

- (void)setSerialPort:(ORSSerialPort *)port
{
	if (port != _serialPort)
	{
		[_serialPort close];
		_serialPort.delegate = nil;
		
		_serialPort = port;
		
		_serialPort.delegate = self;
        _serialPort.numberOfStopBits = 1;
        _serialPort.parity = ORSSerialPortParityNone;
        BOOL enabled = port!=nil;
        self.connectButton.enabled = enabled;
        self.baudButton.enabled = !port.isOpen;
	}
}

@synthesize availableBaudRates = _availableBaudRates;
@synthesize defaultBaudRateIndex = _defaultBaudRateIndex;

@end
