//
//  Servo16Controller.m
//  Servo16
//
// Copyright (c) 2013, Solder Spot
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or
//   other materials provided with the distribution.
// 
// * Neither the name of the copright holders nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#import "Servo16Controller.h"
#import "ORSSerialPort/ORSSerialPortManager.h"
#import "ORSSerialPort/ORSSerialPort.h"

@implementation Servo16Controller

@synthesize speedSlider, freqSlider;

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

-(int) mapSliderSpeed:(int) pos
{
    if ( pos <= 50)
    {
        int val = (int)((3000.0f/180.0f)*pos)/50.0f;
        return val > 0 ? val : 1;
    }
    
    return 3000.0f/180.0f + (int)((10000.0f/180.0f)*(pos-50))/50.0f;
    
}

#pragma mark - Actions

- (IBAction)openOrClosePort:(id)sender
{
	self.serialPort.isOpen ? [self.serialPort close] : [self.serialPort open];
}

- (IBAction)speedChanged:(NSSlider*)sender
{
    int speed = [sender intValue];
	NSData *dataToSend = [[NSString stringWithFormat:@"{rate:-1:%d}", speed] dataUsingEncoding:NSUTF8StringEncoding];
	[self.serialPort sendData:dataToSend];
    [self output:[NSString stringWithFormat:@"Speed : %d.%d degrees per second\n", speed/1000, speed%1000]];
}

- (IBAction)freqChanged:(NSSlider*)sender
{
    int ms = 100 - [sender intValue];
    ms = ms == 0 ? 1 : ms;
	NSData *dataToSend = [[NSString stringWithFormat:@"{ui:%d}", ms] dataUsingEncoding:NSUTF8StringEncoding];
	[self.serialPort sendData:dataToSend];
    [self output:[NSString stringWithFormat:@"Update frequency: %.02f Hz\n", 1000.0f/(float)ms]];
}

- (IBAction)servoChanged:(NSSlider*)sender
{
	NSData *dataToSend = [[NSString stringWithFormat:@"{s:%d:%d}", (int) [sender tag]-200, [sender intValue]] dataUsingEncoding:NSUTF8StringEncoding];
	[self.serialPort sendData:dataToSend];
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

-(void) setAllServos
{
    [self output:@"\nInitializing...\n"];
    [self speedChanged:speedSlider];
    [self freqChanged:freqSlider];
    for( int tag=200;tag<216;tag++)
    {
        NSSlider *slider = [self.view viewWithTag:tag];
        [self servoChanged:slider];
    }
}

-(void) parseForReady:(NSString *)str
{
    static NSString *match = @"Ready!";
    static int pos = 0;
    
    NSUInteger len = [str length];
    
    for( NSUInteger i=0; i<len;i++)
    {
        if( [str characterAtIndex:i] == [match characterAtIndex:pos])
        {
            pos++;
            if( pos >= [match length])
            {
                [self setAllServos];
                pos = 0;
            }
        }
        else
        {
            pos = 0;
        }
    }
    
}

- (void)serialPort:(ORSSerialPort *)serialPort didReceiveData:(NSData *)data
{
	NSString *string = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
	if ([string length] == 0) return;
    [self output:string];
    [self parseForReady:string];
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
    //[self output:[NSString stringWithFormat:@"Ports were connected: %@\n", connectedPorts]];
}

- (void)serialPortsWereDisconnected:(NSNotification *)notification
{
	NSArray *disconnectedPorts = [[notification userInfo] objectForKey:ORSDisconnectedSerialPortsKey];
	NSLog(@"Ports were disconnected: %@", disconnectedPorts);
    [self postUserNotificationForDisconnectedPorts:disconnectedPorts];
    //[self output:[NSString stringWithFormat:@"Ports were disconnected: %@\n", disconnectedPorts]];
	
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
