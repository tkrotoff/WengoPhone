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

#ifndef REGISTER_H
#define REGISTER_H

#include <qobject.h>

/**
 * Register on the SIP server.
 *
 * Used only by the class Connect.
 *
 * @author Tanguy Krotoff
 */
class Register : public QObject {
	Q_OBJECT
public:

	Register();

	~Register();

	/**
	 * Configures PhApi.
	 *
	 * @param PCtoPCmode true if in PC to PC mode, false otherwise
	 */
	void configurePhApi(bool PCtoPCmode, bool httpTunnelEnabled);

	/**
	 * Makes a register request to the SIP server.
	 */
	void doRegister();

	/**
	 * Makes a unregister request to the SIP server.
	 */
	void unRegister();

	bool isPhApiInitialized() const {
		return _isPhApiInitialized;
	}

	int getPhoneLineId() const {
		return _phoneLineId;
	}

	/**
	 * Register progress callback routine.
	 *
	 * Only to be used by PhApiCallbacks.
	 *
	 * @param registerId register id
	 * @param status register status code error
	 * @see PhApiCallbacks
	 */
	static void registerProgress(int registerId, int status);

private:

	Register(const Register &);
	Register & operator=(const Register &);

	void initPhApi();

	void terminate();

	void configureSipUser();

	/**
	 * Sends Authentication informations to the SIP server.
	 */
	bool sendAuthInfoToServer();

	bool makeRegister();

	bool _isPhApiInitialized;

	bool _PCtoPCmode;

	static bool _unregisterAsked;

	int _phoneLineId;
};

#endif	//REGISTER_H
