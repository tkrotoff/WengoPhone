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

#ifndef OWPURPLECONNECT_H
#define OWPURPLECONNECT_H

#include <coipmanager/connectmanager/IConnectPlugin.h>

#include <QtCore/QMutex>

/**
 * LibPurple account connection.
 *
 * @author Philippe Bernery
 * @author Julien Bossart
 */
class PurpleConnect : public IConnectPlugin {
	Q_OBJECT
public:

	PurpleConnect(CoIpManager & coIpManager, const Account & account);

	virtual ~PurpleConnect();

	virtual bool checkValidity();

	static bool createAccountCbk(void * data);

	void emitAccountReadySignal();

Q_SIGNALS:

	void accountReadySignal();

private:

	/**
	 * Creates a LibPurple account if needed.
	 */
	void createAccount();

	static QMutex * _mutex;
};

#endif //OWPURPLECONNECT_H
