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

#ifndef QTSTATUSBAR_H
#define QTSTATUSBAR_H

#include <model/account/SipAccount.h>

#include <qtutil/QObjectThreadSafe.h>

class CWengoPhone;

class QStatusBar;
class QLabel;

/**
 * Main window status bar.
 *
 * Contains status icons for Internet connection, sound status, IM status...
 *
 * @author Tanguy Krotoff
 */
class QtStatusBar : public QObjectThreadSafe {
	Q_OBJECT
public:

	QtStatusBar(CWengoPhone & cWengoPhone, QStatusBar * statusBar);

	void showMessage(const QString & message);

	QLabel * getInternetConnectionStateLabel() const {
		return _internetConnectionStateLabel;
	}

	QLabel * getPhoneLineStateLabel() const {
		return _phoneLineStateLabel;
	}

	QLabel * getSoundStateLabel() const {
		return _soundStateLabel;
	}

private:

	void initThreadSafe() { }

	void networkDiscoveryStateChangedEventHandler(SipAccount & sender, SipAccount::NetworkDiscoveryState state);

	void networkDiscoveryStateChangedEventHandlerThreadSafe(SipAccount & sender, SipAccount::NetworkDiscoveryState state);

	void checkSoundConfig();

	void checkSoundConfigThreadSafe();

	CWengoPhone & _cWengoPhone;

	QStatusBar * _statusBar;

	QLabel * _phoneLineStateLabel;

	QLabel * _internetConnectionStateLabel;

	QLabel * _soundStateLabel;
};

#endif	//QTSTATUSBAR_H
