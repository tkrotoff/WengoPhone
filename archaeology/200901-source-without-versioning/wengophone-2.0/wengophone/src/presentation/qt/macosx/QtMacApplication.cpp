/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "QtMacApplication.h"

#include <model/config/Config.h>
#include <model/config/ConfigManager.h>

#include <util/Logger.h>
#include <util/String.h>

@implementation QtMacApplicationObjC

- (id) initWithQtMacApplicationInstance:(QtMacApplication *)qtMacApplication
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];

	[super init];

	// Member intialization
	_qtMacApplication = qtMacApplication;
	////

	// Register event handlers

	// Event handler for wengo:// handling
	[[NSAppleEventManager sharedAppleEventManager] setEventHandler:self
		andSelector:@selector(getUrl:withReplyEvent:)
		forEventClass:kInternetEventClass
		andEventID:kAEGetURL];

	// Event handle for reopen event. Emitted when the Dock icon is clicked
	// Or when the application icon has been double-clicked.
	[[NSAppleEventManager sharedAppleEventManager] setEventHandler:self
		andSelector:@selector(appReopen:withReplyEvent:)
		forEventClass:kCoreEventClass
		andEventID:kAEReopenApplication];

	////

	[pool release];

	return self;
}

- (void) dealloc
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];

	// Unregister event handlers
	[[NSAppleEventManager sharedAppleEventManager] removeEventHandlerForEventClass:kInternetEventClass
		andEventID:kAEGetURL];

	[[NSAppleEventManager sharedAppleEventManager] removeEventHandlerForEventClass:kCoreEventClass
		andEventID:kAEReopenApplication];
	////

	[pool release];

	[super dealloc];
}

- (void) getUrl:(NSAppleEventDescriptor *)event withReplyEvent:(NSAppleEventDescriptor *)replyEvent
{
	NSString * url = [[event descriptorAtIndex:1] stringValue];
	std::string strUrl = [url cStringUsingEncoding:NSUTF8StringEncoding];
	_qtMacApplication->openURLRequestEvent(strUrl);
}

- (void) appReopen:(NSAppleEventDescriptor *)event withReplyEvent:(NSAppleEventDescriptor *)replyEvent
{
	_qtMacApplication->applicationReopenEvent();
}

@end

QtMacApplication::QtMacApplication(int & argc, char ** argv) 
	: QApplication(argc, argv) {

	_quitCalled = false;

	_qtMacApplicationObjC = [[QtMacApplicationObjC alloc] initWithQtMacApplicationInstance:this];
}

QtMacApplication::~QtMacApplication() {
	[_qtMacApplicationObjC release];
}

void QtMacApplication::applicationReopenEvent() {
	applicationMustShow();
}

void QtMacApplication::openURLRequestEvent(const std::string & url) {
	// Put the received url in Config in case of no object is currently
	// registered to the Signal.
	String myUrl = String(url);
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.set(Config::CMDLINE_PLACECALL_KEY, myUrl.split("/")[1]);

	openURLRequest(QString::fromUtf8(url.c_str()));
}
