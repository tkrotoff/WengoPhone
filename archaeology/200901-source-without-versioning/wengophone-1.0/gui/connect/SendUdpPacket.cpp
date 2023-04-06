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

#include "SendUdpPacket.h"

#include "Connect.h"

#include <phapi.h>

#include <qtimer.h>

#include <cassert>
#include <iostream>
using namespace std;

SendUdpPacket::SendUdpPacket(int timeout)
: QObject() {
	_timeout = timeout;

	//Timer
	_timer = new QTimer(this);
	connect(_timer, SIGNAL(timeout()),
		this, SLOT(send()));
}

SendUdpPacket::~SendUdpPacket() {
	delete _timer;
}

void SendUdpPacket::start() {
	_timer->start(_timeout);
}

void SendUdpPacket::send() {
	if (Connect::getInstance().isConnected()) {
		cerr << "SendUdpPacket: send OPTIONS" << endl;
		phSendOptions(phGetConfig()->identity, "sip:nobody@nobody.com");
	}

	cerr << "SendUdpPacket: phRefresh()" << endl;
	phRefresh();
}

void SendUdpPacket::stop() {
	_timer->stop();
}
