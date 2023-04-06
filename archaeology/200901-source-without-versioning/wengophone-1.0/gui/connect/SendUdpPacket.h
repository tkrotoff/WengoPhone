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

#ifndef SENDUDPPACKET_H
#define SENDUDPPACKET_H

#include <qobject.h>
#include <qstring.h>

class QTimer;

/**
 * Sends a UDP packet in on regular base.
 *
 * Used to keep open client's port when they are behind a firewall/NAT.
 *
 * @author Tanguy Krotoff
 */
class SendUdpPacket : public QObject {
	Q_OBJECT
public:

	/**
	 * Sends a UDP packet in on regular base.
	 *
	 * @param hostname where to send the UDP packet
	 * @param port port number of the host
	 * @param timeout timeout in milliseconds (by default every 30 seconds)
	 */
	SendUdpPacket(int timeout = 30000);

	~SendUdpPacket();

	/**
	 * Starts sending UDP packets.
	 */
	void start();

	/**
	 * Stops sending UDP packets.
	 */
	void stop();

private slots:

	/**
	 * Sends the UDP packet.
	 */
	void send();

private:

	/**
	 * Timer for sending UDP packets on a regular base.
	 */
	QTimer * _timer;

	int _timeout;
};

#endif	//SENDUDPPACKET_H
