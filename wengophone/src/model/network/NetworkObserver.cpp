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

#include "NetworkObserver.h"

#include <util/Logger.h>
#include <thread/Thread.h>
#include <netlib.h>

NetworkObserver::NetworkObserver() 
{
	_isConnected = is_connection_available();
	_timer.timeoutEvent += boost::bind(&NetworkObserver::timeoutEventHandler, this);

	_timer.start(0, 500, 0);
}

NetworkObserver *NetworkObserver::_staticInstance = NULL;

NetworkObserver & NetworkObserver::getInstance()
{
	if (!_staticInstance)
		_staticInstance = new NetworkObserver();

	return * _staticInstance;
}

NetworkObserver::~NetworkObserver() 
{
	_timer.stop();
}

void NetworkObserver::timeoutEventHandler() 
{
	if (is_connection_available())
	{
		if (!_isConnected)
		{
			LOG_DEBUG("Connection is up");
			_isConnected = true;
			connectionIsUpEvent(*this);
		}
	}
	else
	{
		if (_isConnected)
		{
			LOG_DEBUG("Connection is down");
			_isConnected = false;
			connectionIsDownEvent(*this);
		}
	}
}
