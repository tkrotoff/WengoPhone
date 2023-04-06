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

#ifndef CONNECT_H
#define CONNECT_H

#include <singleton/Singleton.h>

#include <qobject.h>

class Register;
class Timer;
class SendUdpPacket;
class RegistrationTask;

/**
 * Connect/reconnect/disconnect WengoPhone from the SIP server.
 *
 * @author Tanguy Krotoff
 */
class Connect : public QObject, public Singleton<Connect> {
	Q_OBJECT

	friend class Singleton<Connect>;
	friend class RegistrationTask;
public:

	~Connect();

	/**
	 * Makes a SIP register (connect).
	 */
	void connect();

	/**
	 * Makes a SIP unregister (disconnect).
	 */
	void disconnect();

	void disable();

	/**
	 * Changes to PC to PC mode.
	 *
	 * In PC to PC mode, WengoPhone does not use the central server.
	 *
	 * @param PCtoPCmode true if PC to PC mode, false otherwise
	 */
	void setPCtoPCmode(bool PCtoPCmode) {
		_PCtoPCmode = PCtoPCmode;
	}

	bool isConnected() const;

	int getPhoneLineId() const;

	static bool isHttpTunnelEnabled() {
		return _httpTunnelEnabled;
	}

signals:

	/**
	 * Signal connected.
	 *
	 * Means WengoPhone has successfully done the register or unregister process.
	 * Used by SipPrimitives
	 */
	void connected();

	/**
	 * Signal register done.
	 *
	 * Means WengoPhone has received an answer from the server after a register/unregister
	 * request. This does not mean that the register was accepted by the server.
	 * Used when WengoPhone quits, we have to wait for the unregister process to be done.
	 */
	void registerDone();

public slots:

	void checkRegisterAnswer();

private:

	Connect();

	Connect(const Connect &);
	Connect & operator=(const Connect &);

	void reconnect();

	/**
	 * Gets if WengoPhone is in communication.
	 *
	 * A register cannot be made if a communication is established.
	 *
	 * @return true if a communication is established
	 */
	bool isCommunicating() const;

	/**
	 * Shows a message box that says that we are in communication
	 * and register or unregister cannot be done during a conversation.
	 */
	void showIsCommunicatingMessage() const;

	/**
	 * Timer to register every 50 minutes if registration is OK.
	 * Otherwise the server keeps the client registration for only 1 hour.
	 */
	Timer * _registrationTimer;

	static const int PHAPIPROGRESSIONTIMER_TIMEOUT;

	/**
	 * Number of limited retry if the register failed.
	 *
	 * Equals 3 times.
	 */
	static const int LIMIT_NB_RETRY;

	/**
	 * Timer to check progression of phApi.
	 *
	 * @see phApiProgression()
	 */
	Timer * _phApiProgressionTimer;

	/**
	 * Register component.
	 *
	 * Deals with phApi and the SIP command register.
	 */
	Register * _register;

	/**
	 * Sends UDP packet using phApi to bypass certain types of firewall/NAT.
	 */
	SendUdpPacket * _sendUdpPacket;

	/**
	 * PC to PC mode means that WengoPhone is going to be used
	 * without the server, just using IP addresses (Peer to Peer mode).
	 */
	bool _PCtoPCmode;

	/** Ugly hack for HttpTunnel. */
	static bool _httpTunnelEnabled;

	int _nbRetry;
};

#endif	//CONNECT_H
