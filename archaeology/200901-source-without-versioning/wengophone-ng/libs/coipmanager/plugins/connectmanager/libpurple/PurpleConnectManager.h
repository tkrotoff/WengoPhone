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

#ifndef OWPURPLECONNECTMANAGER_H
#define OWPURPLECONNECTMANAGER_H

#include <coipmanager/connectmanager/IConnectManagerPlugin.h>

#include <QtCore/QMutex>

extern "C" {
#include <libpurple/connection.h>
}

/**
 * LibPurple implementation of IConnectManagerPlugin.
 *
 * @author Philippe Bernery
 * @author Julien Bossart
 */
class PurpleConnectManager : public IConnectManagerPlugin {
public:

	PurpleConnectManager(CoIpManager & coIpManager);

	~PurpleConnectManager();
 
	virtual IConnectPlugin * createIConnectPlugin(const Account & account) ;

	/** 
	 * @name LibPurple callbacks
	 * @{
	 */
	static void connProgressCbk(PurpleConnection *gc, const char *text, size_t step, size_t step_count);
	static void connConnectedCbk(PurpleConnection *gc);
	static void connDisconnectedCbk(PurpleConnection *gc);
	static void connNoticeCbk(PurpleConnection *gc, const char *text);
	static void connReportDisconnectCbk(PurpleConnection *gc, const char *text);
	/** @} */

private:

	virtual void initialize();

	virtual void uninitialize();

	/**
	 * Finds the IConnectPlugin associated with the PurpleConnection.
	 */
	IConnectPlugin * findIConnectByPurpleConnection(PurpleConnection *gc);

	/**
	 * Used by static methods to get the current instance. 
	 */
	static PurpleConnectManager * _instance;

	/** List of IConnectPlugin. */
	std::vector<IConnectPlugin *> _iConnectVector;

	/** Mutex for thread safe operations. */
	QMutex * _mutex;
};

#endif //OWPURPLECONNECTMANAGER_H
