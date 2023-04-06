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

#include "Connect.h"

#include "Register.h"
#include "Softphone.h"
#include "StatusBar.h"
#include "AudioCallManager.h"
#include "presence/Presence.h"
#include "config/AdvancedConfig.h"
#include "sip/SipPrimitives.h"
#include "util/emit_signal.h"

#include <timer/Timer.h>

#include <phapi.h>

#include <qtimer.h>
#include <qmessagebox.h>

#include <iostream>
using namespace std;

const int Connect::PHAPIPROGRESSIONTIMER_TIMEOUT = 100;
const int Connect::LIMIT_NB_RETRY = 5;
bool Connect::_httpTunnelEnabled = false;

class RegistrationTask : public TimerTask {
public:

	RegistrationTask(Connect * connect, Register * reg) {
		_connect = connect;
		_register = reg;
		_connect->_nbRetry = 0;
		httpTunnelEnabled = false;
	}

	virtual void run() {
		cout << "RegistrationTask::run()" << endl;
		_connect->_nbRetry++;
		_register->doRegister();
		_connect->checkRegisterAnswer();
		if (_connect->_nbRetry >= Connect::LIMIT_NB_RETRY) {
			httpTunnelEnabled = true;
			_connect->reconnect();
		}
	}

	static bool httpTunnelEnabled;

private:

	Connect * _connect;

	Register * _register;
};

bool RegistrationTask::httpTunnelEnabled = false;

class PhApiProgressionTask : public TimerTask {
public:

	PhApiProgressionTask(Register * reg) {
		_register = reg;
	}

	virtual void run() {
		//Shows the error message only if a reconnect does not work.
		static bool showMessage = false;

		if (_register->isPhApiInitialized()) {
			if (phPoll() == -2) {
				/*
				 * This occurs when eXosip_thread has been terminated due to an error.
				 * All the phApi engine is now unavailable.
				 * This occurs with the HTTP tunnel when this one is closed because
				 * the Internet link has been cut.
				 * Stops phApi then try to re-initialize it.
				 */
				AudioCall * audioCall = AudioCallManager::getInstance().getActiveAudioCall();
				if (audioCall) {
					SipPrimitives::closeCall(audioCall->getCallId());
					emit_signal(&AudioCallManager::getInstance(), SIGNAL(endCall()));
				}

				Connect::getInstance().disable();
				//if (showMessage) {
				//	showMessage = false;
				//	QMessageBox::critical(Softphone::getInstance().getActiveWindow(),
				//		QObject::tr("Internet link cut"),
				//		QObject::tr("Your Internet connection has been cut, WengoPhone will try to reconnect"));
				//}
				//Connect::getInstance().connect();
				//showMessage = true;
			}
		}
	}

private:

	Register * _register;
};


Connect::Connect() {
	//By default, the SIP status is not OK (false)
	Softphone::getInstance().getStatusBar().setSipStatus(false);

	_register = new Register();

	//Creates a Presence object so that it calls
	//the constructor and observes the class ContactList
	new Presence();

	//phPoll()
	_phApiProgressionTimer = new Timer(new PhApiProgressionTask(_register));
	_phApiProgressionTimer->start(PHAPIPROGRESSIONTIMER_TIMEOUT);

	//Try registerAgain() every 10 seconds during LIMIT_NB_RETRY
	//_registrationTimer = new Timer(new RegistrationTask(this, _register));

	//By default we are not in PC to PC mode
	_PCtoPCmode = false;
}

Connect::~Connect() {
	delete _register;	//Important: terminates phApi
	delete _registrationTimer;
	delete _phApiProgressionTimer;
}

void Connect::reconnect() {
	//Ugly hack for httptunnel
	_httpTunnelEnabled = AdvancedConfig::getInstance().isHttpProxyUsed();
	if (_nbRetry >= Connect::LIMIT_NB_RETRY) {
		_nbRetry = 0;
		_httpTunnelEnabled = RegistrationTask::httpTunnelEnabled;
	}
	cout << "HTTPTUNNEL MODE=" << _httpTunnelEnabled << endl;

	_register->configurePhApi(_PCtoPCmode, _httpTunnelEnabled);

	if (!_PCtoPCmode) {
		//FIXME
		_registrationTimer = new Timer(new RegistrationTask(this, _register));
		_registrationTimer->stop();
		_registrationTimer->start(5 * 1000, LIMIT_NB_RETRY);
		_registrationTimer->singleShotNow();

		//Subscribe again to the presence of all the contacts
		Presence::subscribeToEverybodyPresence();

		//Publish my status online
		Presence::publishOnlineStatus();

		//Asks for the watcher list
		Presence::askForWatcherList();

		//Wait for 5 seconds before to resume register
		QTimer::singleShot(5 * 1000, this, SLOT(checkRegisterAnswer()));
	}
}

void Connect::connect() {
	//When PCtoPCmode = true
	//we must call:
	//phUnregister()
	//phTerminate()
	//reconfigure phcfg
	//phInit()

	if (isCommunicating()) {
		showIsCommunicatingMessage();
		return;
	}

	//Disconnect first each time we want to connect
	//Connect::disconnect();

	_nbRetry = 0;
	reconnect();
}

void Connect::checkRegisterAnswer() {
	if (isConnected()) {
		//Try to register every 50 minutes
		//without limit
		_registrationTimer->stop();
	} else {
		//If ok is true does not mean that register succeded
		//but if ok is false for sure register failed.
		//In order to have a full answer, we should wait for regProgress().
		//This is done via isConnected()
		Softphone::getInstance().getStatusBar().setSipStatus(false);
	}
}

// This method is called when a error occurred in the HTTP Tunnel.
// It doesn't include IsCommunicating() test because we don't care
// about the fact that a current communication will be cut.
// The tunnel is broken so we have to disable the connections.
void Connect::disable()
{
	if (_register->isPhApiInitialized()) {
		//Publish my status offline
		Presence::publishOfflineStatus();

		//_register->configurePhApi(_PCtoPCmode, _httpTunnelEnabled);
		_register->unRegister();

		Softphone::getInstance().getStatusBar().setSipStatus(false);
		//_registrationTimer->stop();

		//Cannot do that otherwise phPoll() does not work anymore
		_phApiProgressionTimer->stop();
	}
}

void Connect::disconnect() {
	if (isCommunicating()) {
		return;
	}

	if (_register->isPhApiInitialized()) {
		//Publish my status offline
		Presence::publishOfflineStatus();

		//_register->configurePhApi(_PCtoPCmode, _httpTunnelEnabled);
		_register->unRegister();

		Softphone::getInstance().getStatusBar().setSipStatus(false);
		//_registrationTimer->stop();

		//Cannot do that otherwise phPoll() does not work anymore
		//_phApiProgressionTimer->stop();
	}
}

bool Connect::isCommunicating() const {
	const AudioCallManager & audioCallManager = AudioCallManager::getInstance();
	if (audioCallManager.isConversationOccuring()) {
		return true;
	}
	return false;
}

void Connect::showIsCommunicatingMessage() const {
	QMessageBox::critical(Softphone::getInstance().getActiveWindow(),
				tr("Cannot make a SIP register"),
				tr("You can not make a SIP register while in communication, try again later"));
}

bool Connect::isConnected() const {
	bool connected = Softphone::getInstance().getStatusBar().isSipEnabled();
	cerr << "Connect: is connected? " << connected << endl;
	return connected;
}

int Connect::getPhoneLineId() const {
	return _register->getPhoneLineId();
}
