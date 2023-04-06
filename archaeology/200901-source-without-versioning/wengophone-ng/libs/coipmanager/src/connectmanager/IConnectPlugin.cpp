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

#include <coipmanager/connectmanager/IConnectPlugin.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/connectmanager/IConnectState.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <coipmanager_base/EnumConnectionState.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

IConnectPlugin::IConnectPlugin(CoIpManager & coIpManager, const Account & account)
	: QObject(),
	IStateDPContext(),
	_account(account),
	_coIpManager(coIpManager) {

	setConnectionSteps(2);
}

IConnectPlugin::~IConnectPlugin() {
}

Account & IConnectPlugin::getAccount() {
	UserProfile userProfile = _coIpManager.getUserProfileManager().getCopyOfUserProfile();
	AccountList accountList = userProfile.getAccountList();
	Account *account = AccountListHelper::getCopyOfAccount(accountList, _account.getUUID());
	if (account) {
		_account = Account(*account);
		OWSAFE_DELETE(account);
	} else {
		LOG_ERROR("cannot retrieve account!!");
	}

	return _account;
}

void IConnectPlugin::connect() {
	if (_currentState) {
		static_cast<IConnectState *>(_currentState)->connect();
	}
}

void IConnectPlugin::disconnect() {
	if (_currentState) {
		static_cast<IConnectState *>(_currentState)->disconnect();
	}
}

unsigned IConnectPlugin::getConnectionSteps() const {
	return _connectionSteps;
}

void IConnectPlugin::setConnectionSteps(unsigned steps) {
	_connectionSteps = steps;
}

unsigned IConnectPlugin::getCurrentConnectionStep() const {
	return _currentConnectionStep;
}

void IConnectPlugin::setCurrentConnectionStep(unsigned step) {
	_currentConnectionStep = step;
}

void IConnectPlugin::setState(EnumConnectionState::ConnectionState stateId) {
	setCurrentConnectionStep(stateId);

	accountConnectionStateSignal(_account.getUUID(), stateId);

	IStateDPContext::setState(stateId);
}

void IConnectPlugin::setState(int stateId) {
	IStateDPContext::setState(stateId);
}

CoIpManager & IConnectPlugin::getCoIpManager() {
	return _coIpManager;
}

void IConnectPlugin::emitAccountConnectionStateSignal(std::string accountId,
	EnumConnectionState::ConnectionState state) {
	accountConnectionStateSignal(accountId, state);
}

void IConnectPlugin::emitAccountConnectionErrorSignal(std::string accountId,
	EnumConnectionError::ConnectionError errorCode) {
	
	accountConnectionErrorSignal(accountId, errorCode);
}

void IConnectPlugin::emitAccountConnectionProgressSignal(std::string accountId,
	unsigned currentStep, unsigned totalSteps, std::string infoMessage) {

	accountConnectionProgressSignal(accountId, currentStep, totalSteps, infoMessage);
}
