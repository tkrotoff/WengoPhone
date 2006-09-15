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

#include <Carbon/Carbon.h>
#include <Cocoa/Cocoa.h>

#include <iostream>

// Type to convert an integer to 4 chars
typedef struct kIntToChar {
	char a;
	char b;
	char c;
	char d;
} kIntToChar;
////

QtMacApplication::QtMacApplication(int & argc, char ** argv) 
	: QApplication(argc, argv) {

	_quitCalled = false;

}

QtMacApplication::~QtMacApplication() {
}

bool QtMacApplication::macEventFilter(EventHandlerCallRef caller, EventRef event) {
	UInt32 eventClass = GetEventClass(event);
	switch (eventClass) {
	/*
	case kEventClassMouse:
		break;
	case kEventClassKeyboard:
		break;
	case kEventClassTextInput:
		break;
	case kEventClassApplication:
		std::cout << std::endl << "ClassApplication: " << GetEventKind(event) << std::endl;
		break;
	case kEventClassAppleEvent:
		break;
	case kEventClassMenu:
		break;
	*/
	case kEventClassWindow:
		std::cout << std::endl << "ClassWindow: " << GetEventKind(event) << std::endl;
		break;
	/*case kEventClassControl:
		break;
	*/
	case kEventClassCommand:
		if (GetEventKind(event) == kEventCommandProcess) {
			HICommand command;
			GetEventParameter(event, kEventParamDirectObject, typeHICommand,
				NULL, sizeof(command), NULL, &command);
			switch (command.commandID) {
			case kHICommandQuit:
				if (!_quitCalled) {
					_quitCalled = true;
					applicationMustQuit();
				}
				return true;
				break;
			default:
				kIntToChar * toChar = (kIntToChar *) &command.commandID;
				std::cout << std::endl << "command: "
					<< toChar->a << toChar->b
					<< toChar->c << toChar->d
					<< std::endl;
			};
		}
		break;
	/*
	case kEventClassTablet:
		break;
	case kEventClassVolume:
		break;
	case kEventClassAppearance:
		break;
	case kEventClassService:
		break;
	case kEventClassToolbar:
		break;
	case kEventClassToolbarItem:
		break;
	case kEventClassToolbarItemView:
		break;
	case kEventClassAccessibility:
		break;
	case kEventClassSystem:
		break;
	case kEventClassInk:
		break;
	case kEventClassTSMDocumentAccess:
		break;
	*/
	default:
		kIntToChar * chars = (kIntToChar *)&eventClass;
		std::cout << "Unknown class: " << chars->a << chars->b << chars->c << chars->d << ": " << GetEventKind(event) << std::endl;
	};

	return false;
}
