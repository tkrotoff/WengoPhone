/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWNETWORKOBSERVER_H
#define OWNETWORKOBSERVER_H

#include <networkdiscovery/networkdiscoverydll.h>

#include <QtCore/QTimer>

/**
 * Watch the connection status.
 *
 * @author Julien Bossart
 */
class NETWORKDISCOVERY_API NetworkObserver : public QObject {
	Q_OBJECT
public:

	bool isConnected() const;

	static NetworkObserver & getInstance();

Q_SIGNALS:

	/**
	 * Emitted when the connection is going up.
	 */
	void connectionIsUpSignal();

	/**
	 * Emitted when the connection is going down.
	 */
	void connectionIsDownSignal();

private Q_SLOTS:

	void timeoutSlot();

private:

	NetworkObserver();

	~NetworkObserver();

	static NetworkObserver * _staticInstance;

	QTimer _timer;

	bool _isConnected;
};

#endif	//OWNETWORKOBSERVER_H
