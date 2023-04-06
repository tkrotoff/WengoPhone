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

#include "WenboxPlugin.h"

#include "Softphone.h"
#include "MainWindow.h"
#include "SessionWindow.h"
#include "PhonePageWidget.h"
#include "AudioCallManager.h"
#include "util/emit_signal.h"
#include "config/Audio.h"
#include "config/ConfigWindow.h"
#include "config/AudioWidget.h"
#include "sip/SipAddress.h"

#include "YLTELBOX.h"

#include <Sound.h>
#include <AudioDevice.h>

#include <qstring.h>
#include <qdialog.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtimer.h>
#include <qmainwindow.h>
#include <qdatetime.h>
#include <qobjectlist.h>
#include <qframe.h>
#include <qmessagebox.h>

#include <cassert>
#include <iostream>
using namespace std;

const int WenboxPlugin::TIMER_TIMEOUT = 4000;

WenboxPlugin::WenboxPlugin() {
	cout << "WengoPlugin: WenboxPlugin loaded" << endl;
	_audioCall = NULL;
}

WenboxPlugin::~WenboxPlugin() {
	gotoPSTN();
}

void WenboxPlugin::init(Softphone & softphone) {
	_softphone = &softphone;
	_mainWindow = &_softphone->getMainWindow();
	
	_time = NULL;
	
	//retrive the main window radio buttons (wenbox & sound card)
	_wenboxCheckBox = (QCheckBox *) _mainWindow->getWidget()->child("WenboxCheckBox", "QCheckBox");
	_wenboxCheckBox->setEnabled(false);
	
	//synchronyse the configuration panel with the main window
	QWidget * audioWidget = _mainWindow->getConfigWindow().getAudioWidget().getWidget();
//	QRadioButton * audioConfigRadioButton = (QRadioButton *) audioWidget->child("audioConfigRadioButton");
	QRadioButton * wenboxConfigRadioButton = (QRadioButton *) audioWidget->child("wenboxRadioButton");
	connect(wenboxConfigRadioButton, SIGNAL(toggled(bool)), _wenboxCheckBox, SLOT(setChecked(bool)));
	connect(_wenboxCheckBox, SIGNAL(toggled(bool)), wenboxConfigRadioButton, SLOT(setChecked(bool)));
	
	//connect main window & the configuration panel radio buttons with the switch wenbox slot
	connect(wenboxConfigRadioButton, SIGNAL(toggled(bool)), this, SLOT(wenboxRadioButtonToggled(bool)));
	connect(_wenboxCheckBox, SIGNAL(toggled(bool)), this, SLOT(wenboxRadioButtonToggled(bool)));
}

void WenboxPlugin::dispose() {
	gotoPSTN();
	closeDevice();
}

void WenboxPlugin::run() {
	if (openDevice()) {
		
		//Only if the wenbox was initialized properly
		gotoDefaultUSB();

		//Saves the audio settings in order to restore them later on
		Audio::getInstance().storeSettings();

		switchAudioDeviceToWenbox(true);

		//inputCall, outputCall and endCall signals
		AudioCallManager & audioCallManager = AudioCallManager::getInstance();
		connect(&audioCallManager, SIGNAL(inputCall()),
			this, SLOT(inputCall()));
		connect(&audioCallManager, SIGNAL(outputCall()),
			this, SLOT(outputCall()));
		connect(&audioCallManager, SIGNAL(endCall()),
			this, SLOT(endCall()));
		
		_wenboxCheckBox->setEnabled(true);
		_wenboxCheckBox->setChecked(true);
		
		_timer = new QTimer(this);
		connect(_timer, SIGNAL(timeout()),
			this, SLOT(call()));
		_timer->start(1000);
	}
}

void YLUSBTELCallBack(void * wParam, void * lParam, unsigned long parameterOfApplications) {
	WenboxPlugin * pThis = (WenboxPlugin *) parameterOfApplications;
	pThis->sendMessage((unsigned int) wParam, (long) lParam);
}

void WenboxPlugin::sendMessage(unsigned int wParam, long lParam) {

	switch(wParam) {

	case YL_CALLBACK_MSG_USBPHONE_VERSION: {
		char str[10];
		unsigned short ver = (unsigned short) lParam;
		sprintf(str, (ver>=0x0520)?"%04x - B2K":"%04x - Err", ver);
		cerr << "WenboxPlugin: " << str << endl;
		break;
	}

	case YL_CALLBACK_MSG_USBPHONE_SERIALNO: {
		QString serialNumber = QString("%1").arg((char *) lParam);
		cerr << serialNumber << endl;
		break;
	}

	case YL_CALLBACK_GEN_OFFHOOK:
		if (_modeUSB && _status == STATUS_CALLIN) {
			gotoTalk();
			_audioCall = AudioCallManager::getInstance().getActiveAudioCall();
			if (_audioCall) {
				PhonePageWidget & phonePageWidget = _audioCall->getSessionWindow().getPhonePageWidget();
				emit_signal(phonePageWidget.getVideoCallButton(), SIGNAL(clicked()));
			}
		}
		cerr << "WenboxPlugin: offhook" << endl;

		break;

	case YL_CALLBACK_GEN_HANGUP:
		//Removed because of the bug phone occupied when in PSTN line
		//Status should be READY
		//if (_modeUSB && (_status == STATUS_CALLOUT || _status == STATUS_TALKING)) {
			gotoReady();
		//}
		cerr << "WenboxPlugin: hangup" << endl;

		_audioCall = AudioCallManager::getInstance().getActiveAudioCall();
		if (_audioCall) {
			//Close the SessionWindow anymore
			_audioCall->getSessionWindow().closeWindow();

			//Just close the call
			/*PhonePageWidget & phonePageWidget = _audioCall->getSessionWindow().getPhonePageWidget();
			emit_signal(phonePageWidget.getHangUpButton(), SIGNAL(clicked()));*/

			_audioCall = NULL;
		}

		delete _time;
		_time = NULL;

		break;

	case YL_CALLBACK_GEN_KEYBUF_CHANGED: {
		QString keyStr = QString("%1").arg((char *)lParam);
		cerr << keyStr << endl;

		if (!keyStr.isEmpty()) {
			QComboBox * phoneComboBox = _mainWindow->getPhoneComboBox();
			phoneComboBox->setCurrentText(keyStr);
		}
		break;
	}

	case YL_CALLBACK_GEN_KEYDOWN: {
		if (_status == STATUS_READY) {
			delete _time;
			_time = NULL;
			_time = new QTime();
			_time->start();
		}

		//Button pressed
		unsigned char key = (unsigned char)(unsigned long) lParam;
		char ckey[2];
		sprintf(ckey, "%02X", key);
		cerr << "WenboxPlugin: " << ckey << endl;
		if (key == KEY_SEND) {
			if (_status == STATUS_READY) {
				call();
			}
		}

		break;
	}

	case YL_CALLBACK_GEN_PSTNRING_START:
		cerr << "WenboxPlugin: ring start" << endl;
		break;

	case YL_CALLBACK_GEN_PSTNRING_STOP:
		cerr << "WenboxPlugin: ring stop" << endl;
		break;

	case YL_CALLBACK_GEN_INUSB:
		cerr << "WenboxPlugin: USB mode" << endl;
		_modeUSB = true;
		break;

	case YL_CALLBACK_GEN_INPSTN:
		cerr << "WenboxPlugin: PSTN mode" << endl;
		_modeUSB = false;
		break;

	case YL_CALLBACK_MSG_WARNING: {
		unsigned short r = (unsigned short) lParam;
		if (r == YL_RETURN_DEV_VERSION_ERROR) {
			cerr << "WenboxPlugin: error: device version error" << endl;
		}
		break;
	}

	case YL_CALLBACK_MSG_ERROR: {
		unsigned short r = (unsigned short) lParam;
		if (r == YL_RETURN_HID_COMM_ERROR) {
			cerr << "WenboxPlugin: error: HID communication error" << endl;
		}
		break;
	}
	}
}

bool WenboxPlugin::openDevice() {
	unsigned long status = YL_DeviceIoControl(YL_IOCTL_OPEN_DEVICE, (void *) YLUSBTELCallBack,
						(unsigned long) this, 0, 0);

	switch(status) {

	case YL_RETURN_SUCCESS: {
		gotoReady();

		//Change the default audio device if it is the wenbox
		//Uses QString::find() rather than operator==, more chances to get the equality
		if (WenboxAudioDevice::getWenboxAudioDeviceName().find(AudioDevice::getDefaultPlaybackDevice()) != -1) {
			WenboxAudioDevice::rollbackDefaultAudioDevice();
		}

		return true;
	}

	case YL_RETURN_MAP_ERROR:
		cerr << "WenboxPlugin: error: memory map error" << endl;
		break;

	case YL_RETURN_HID_ISOPENED:
		cerr << "WenboxPlugin: error: re-open invalid" << endl;
		break;

	case YL_RETURN_NO_FOUND_HID:
		cerr << "WenboxPlugin: error: no device found" << endl;
		break;
	}

	return false;
}

void WenboxPlugin::closeDevice() {
	YL_DeviceIoControl(YL_IOCTL_CLOSE_DEVICE, 0, 0, 0, 0);
	cerr << "WenboxPlugin: device closed" << endl;
}

void WenboxPlugin::gotoUSB() {
	YL_DeviceIoControl(YL_IOCTL_GEN_GOTOUSB, 0, 0, 0, 0);
}

void WenboxPlugin::gotoPSTN() {
	YL_DeviceIoControl(YL_IOCTL_GEN_GOTOPSTN, 0, 0, 0, 0);
}

void WenboxPlugin::gotoReady() {
	_status = STATUS_READY;
	cerr << "WenboxPlugin: status ready" << endl;
	YL_DeviceIoControl(YL_IOCTL_GEN_READY, 0, 0, 0, 0);
}

void WenboxPlugin::gotoCallin(const char * phoneNumber) {
	if (!phoneNumber) {
		return;
	}

	if (_status != STATUS_READY) {
		cerr << "WenboxPlugin: error: only status ready can receive callin" << endl;
		return;
	}

	_status = STATUS_CALLIN;

	cerr << "WenboxPlugin: call in: " << phoneNumber << endl;
	char keybuf[51];
	sprintf(keybuf, phoneNumber);

	YL_DeviceIoControl(YL_IOCTL_GEN_CALLIN, (void *)(char *) keybuf, 0, 0, 0);
}

void WenboxPlugin::phoneOffHookFromOutside() {
	//Special trick
	//Wenbox stop ringing
	gotoTalk();
}

void WenboxPlugin::gotoCallout() {
	if (_status != STATUS_READY) {
		cerr << "WenboxPlugin: error: only status ready can callout" << endl;
		return;
	}

	_status = STATUS_CALLOUT;
	cerr << "WenboxPlugin: call out" << endl;
	char keybuf[51];

	QString phoneNumber = _mainWindow->getPhoneComboBox()->currentText();
	_audioCall = AudioCallManager::getInstance().createAudioCall(SipAddress::fromPhoneNumber(phoneNumber), NULL, true);
	sprintf(keybuf, phoneNumber);

	YL_DeviceIoControl(YL_IOCTL_GEN_CALLOUT, (void *)(char *) keybuf, 0, 0, 0);
}

void WenboxPlugin::gotoTalk() {
	if (_status != STATUS_CALLIN && _status != STATUS_CALLOUT) {
		cerr << "WenboxPlugin: error: only status callin or callout can into talking" << endl;
		return;
	}
	_status = STATUS_TALKING;
	cerr << "WenboxPlugin: talking" << endl;
	YL_DeviceIoControl(YL_IOCTL_GEN_TALKING, 0, 0, 0, 0);
}

void WenboxPlugin::gotoDefaultUSB() {
	YL_DeviceIoControl(YL_IOCTL_GEN_DEFAULTUSB, 0, 0, 0, 0);
}

void WenboxPlugin::gotoDefaultPSTN() {
	YL_DeviceIoControl(YL_IOCTL_GEN_DEFAULTPSTN, 0, 0, 0, 0);
}

void WenboxPlugin::gotoOnlyUSB() {
	YL_DeviceIoControl(YL_IOCTL_GEN_ONLYUSB, 0, 0, 0, 0);
}

void WenboxPlugin::endCall() {
	if (WenboxAudioDevice::isWenboxAudioDeviceUsed()) {
		cerr << "WenboxPlugin: communisation close" << endl;
		gotoReady();
		YL_DeviceIoControl(YL_IOCTL_CLOSE_SIGNAL, 0, 0, 0, 0);
	}
}

void WenboxPlugin::inputCall() {
	if (WenboxAudioDevice::isWenboxAudioDeviceUsed()) {
		_audioCall = AudioCallManager::getInstance().getActiveAudioCall();
		if (_audioCall) {
			PhonePageWidget & phonePageWidget = _audioCall->getSessionWindow().getPhonePageWidget();
			
			QString phoneNumber = phonePageWidget.getCallerPhoneNumber();
			
			//If the user clicks from the gui
			//Makes the wenbox stop ringing
			connect(phonePageWidget.getAudioCallButton(), SIGNAL(clicked()),
				this, SLOT(phoneOffHookFromOutside()));
			connect(phonePageWidget.getVideoCallButton(), SIGNAL(clicked()),
				this, SLOT(phoneOffHookFromOutside()));
	
			gotoCallin(phoneNumber);
		}
	}
}

void WenboxPlugin::outputCall() {
	if (WenboxAudioDevice::isWenboxAudioDeviceUsed()) {
		//Special trick
		//Send Callout message to the Wenbox so that the speaker of the phone is ready,
		//otherwise there is the phone tonality
		YL_DeviceIoControl(YL_IOCTL_GEN_CALLOUT, (void *)(char *) NULL, 0, 0, 0);
		_status = STATUS_CALLOUT;

		gotoTalk();
	}
}

void WenboxPlugin::call() {
	if (_modeUSB && _time != NULL) {
		if (_time->elapsed() > TIMER_TIMEOUT) {
			delete _time;
			_time = NULL;
			if (_status == STATUS_READY) {
				gotoCallout();
			} else {
				cerr << "WenboxPlugin: error: status not ready" << endl;
			}
		}
	}
}

void WenboxPlugin::switchAudioDeviceToWenbox(bool wenboxButtonToggled) {
	//It's not good to connect several times the same widget to the same action
	    
	QWidget * audioWidget = _mainWindow->getConfigWindow().getAudioWidget().getWidget();
	QRadioButton * wenboxRadioButton = (QRadioButton *) audioWidget->child("wenboxRadioButton");
	assert(wenboxRadioButton && "wenboxRadioButton cannot be NULL");
	
	wenboxRadioButton->setEnabled(true);
	wenboxRadioButton->setChecked(wenboxButtonToggled);
}

void WenboxPlugin::wenboxRadioButtonToggled(bool on) {
	//retrieve the audioSettingsFrame from the configuration panel in order to enable/disable it
	QWidget * audioWidget = _mainWindow->getConfigWindow().getAudioWidget().getWidget();
	QFrame * audioSettingsFrame = (QFrame *) audioWidget->child("audioSettingsFrame");
	assert(audioSettingsFrame && "audioSettingsFrame cannot be NULL");
	
	//Changes (enables or disables) the ConfigWindow components
	if (on) {
		//Switch to Wenbox button
		if (WenboxAudioDevice::switchCurrentAudioDeviceToWenbox()) {
		}
		audioSettingsFrame->setEnabled(false);
	} else {
		//Switch to previous audio settings
		Audio::getInstance().loadStoredSettings();
		audioSettingsFrame->setEnabled(true);
	}
	QRadioButton * audioConfigRadioButton = (QRadioButton *) audioWidget->child("audioConfigRadioButton");
	audioConfigRadioButton->setChecked(!on);
}

void WenboxPlugin::switchWenbox(int id) {
	switchAudioDeviceToWenbox(!_wenboxCheckBox->isChecked());
}

