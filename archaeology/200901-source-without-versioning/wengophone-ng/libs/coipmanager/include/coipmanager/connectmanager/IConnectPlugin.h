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

#ifndef OWICONNECTPLUGIN_H
#define OWICONNECTPLUGIN_H

#include <coipmanager/AccountMutator.h>
#include <coipmanager/connectmanager/EnumConnectionError.h>

#include <coipmanager_base/EnumConnectionState.h>
#include <coipmanager_base/account/Account.h>

#include <util/IStateDPContext.h>

#include <QtCore/QObject>

#include <map>
#include <string>

class CoIpManager;

/**
 * Interface for connection/disconnection of Account.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class COIPMANAGER_API IConnectPlugin : public QObject, public IStateDPContext {
	Q_OBJECT
public:

	IConnectPlugin(CoIpManager & coIpManager, const Account & account);

	virtual ~IConnectPlugin();

	/**
	 * Gets the associated Account. The account is retrieved from UserProfile.
	 * Thus it is up-to-date.
	 */
	Account & getAccount();

	/**
	 * Checks the validity of the Account.
	 *
	 * Blocking method.
	 *
	 * @return true if valid
	 */
	virtual bool checkValidity() = 0;

	/**
	 * Connects an Account.
	 *
	 * Connection process may be threaded.
	 */
	void connect();

	/**
	 * Disconnects an Account.
	 *
	 * Connection process may be threaded.
	 */
	void disconnect();

	/**
	 * Gets maximum connection steps.
	 */
	unsigned getConnectionSteps() const;

	/**
	 * Sets maximum connection steps.
	 */
	void setConnectionSteps(unsigned steps);

	/**
	 * Gets current connection step.
	 */
	unsigned getCurrentConnectionStep() const;

	/**
	 * Sets current connection step.
	 */
	void setCurrentConnectionStep(unsigned step);

	/**
	 * Sets the current state. Kind of override of setState(int)
	 */
	void setState(EnumConnectionState::ConnectionState stateId);

	CoIpManager & getCoIpManager();

	/**
	 * @see accountConnectedSignal()
	 */
	void emitAccountConnectionStateSignal(std::string accountId,
		EnumConnectionState::ConnectionState state);

	/**
	 * @see accountConnectionErrorSignal()
	 */
	void emitAccountConnectionErrorSignal(std::string accountId,
		EnumConnectionError::ConnectionError errorCode);

	/**
	 * @see accountConnectionProgressSignal()
	 */
	void emitAccountConnectionProgressSignal(std::string accountId,
		unsigned currentStep, unsigned totalSteps, std::string infoMessage);

Q_SIGNALS:

	/**
	 * Emitted when an Account change its connection state.
	 *
	 * @param accountId UUID of the connected Account.
	 */
	void accountConnectionStateSignal(std::string accountId,
		EnumConnectionState::ConnectionState state);

	/**
	 * Emitted when an error occured while connecting.
	 *
	 * @param errorCode @see ConnectionError
	 * @param accountId UUID of the Account
	 */
	void accountConnectionErrorSignal(std::string accountId,
		EnumConnectionError::ConnectionError errorCode);

	/**
	 * Emitted for information about Account connection progression.
	 *
	 * @param accountId UUID of the Account.
	 * @param currentStep
	 * @param totalSteps
	 * @param infoMessage
	 */
	void accountConnectionProgressSignal(std::string accountId,
		unsigned currentStep, unsigned totalSteps, std::string infoMessage);

protected:

	// Inherited from IStateDPContext
	virtual void setState(int stateId);

	/** Amount of connection steps. */
	unsigned _connectionSteps;

	/** Current connection step. */
	unsigned _currentConnectionStep;

	/** Link to the Account. */
	Account _account;

	CoIpManager & _coIpManager;
};

#endif //OWICONNECTPLUGIN_H
