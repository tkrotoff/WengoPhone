/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#ifndef WENGOPHONELOGGER_H
#define WENGOPHONELOGGER_H

#include <NonCopyable.h>
#include <Event.h>

/**
 * Logging system.
 *
 * Example:
 * <pre>
 * LOG_DEBUG("debug message");
 * </pre>
 *
 * Logged messages automatically include the name of the class where one
 * of the macro (LOG_*) has been called.
 * This class works on top of the class Logger from LibUtil.
 * There is only one instance of this class inside the application.
 *
 * If a control and a presentation component exist for this class,
 * all logged messages will be showed graphically.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class WengoPhoneLogger {
public:

	/**
	 * @see Logger::messageAddedEvent
	 */
	Event<void (const std::string & message)> messageAddedEvent;

	WengoPhoneLogger();

	//~WengoPhoneLogger();
};

#endif	//WENGOPHONELOGGER_H
