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

#ifndef WENBOXPLUGIN_H
#define WENBOXPLUGIN_H

#include "Plugin.h"
#include "WenboxAudioDevice.h"

#include <qobject.h>

class Softphone;
class MainWindow;
class AudioCall;
class Sound;
class QTimer;
class QTimerEvent;
class QTime;
class QCheckBox;
//class QPushButton;
//class QRadioButton;
//class QButtonGroup;

class WenboxPlugin : public QObject, public Plugin {
	Q_OBJECT
public:

	WenboxPlugin();

	~WenboxPlugin();

	virtual void init(Softphone & softphone);

	virtual void dispose();

	virtual void run();

	void sendMessage(unsigned int wParam, long lParam);

private slots:

	void endCall();

	void outputCall();

	void inputCall();

	void call();

	void phoneOffHookFromOutside();

	void switchAudioDeviceToWenbox(bool wenboxButtonToggled);

	void wenboxRadioButtonToggled(bool on);

	void switchWenbox(int id);

private:

	/**
	 * Initializes the Wenbox.
	 *
	 * @return true if everything OK, false otherwise
	 */
	bool openDevice();

	void closeDevice();

	void gotoTalk();

	void gotoUSB();

	void gotoPSTN();

	void gotoReady();

	void gotoCallin(const char * phoneNumber);

	void gotoCallout();

	void gotoDefaultUSB();

	void gotoDefaultPSTN();

	void gotoOnlyUSB();

	/**
	 * True if the wenbox is in USB mode, false in PSTN mode.
	 */
	bool _modeUSB;

	/**
	 * Status of the wenbox.
	 */
	unsigned int _status;

	enum {
		STATUS_READY,
		STATUS_CALLIN,
		STATUS_CALLOUT,
		STATUS_TALKING
	};

	Softphone * _softphone;

	MainWindow * _mainWindow;

	/**
	 * QTimer timeout in milliseconds.
	 */
	static const int TIMER_TIMEOUT;

	QTimer * _timer;

	QTime * _time;

	//QPushButton * _wenboxButton;
	/*
	QRadioButton * _wenboxRadio;
	
	QRadioButton * _pcRadio;

	QButtonGroup * _radioButtonGroup;
	*/
	QCheckBox * _wenboxCheckBox;
	
	/**
	 * AudioCall created and stopped using the Wenbox.
	 */
	AudioCall * _audioCall;
};

/*
#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif	//Q_WS_WIN

extern "C" __declspec(dllexport) Plugin * create() {
	return new WenboxPlugin();
}
*/

#endif	//WENBOXPLUGIN_H
