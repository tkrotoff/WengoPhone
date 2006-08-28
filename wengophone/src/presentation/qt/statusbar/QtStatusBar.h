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

#ifndef OWQTSTATUSBAR_H
#define OWQTSTATUSBAR_H

#include <sipwrapper/EnumPhoneLineState.h>

#include <util/Trackable.h>

#include <qtutil/QObjectThreadSafe.h>

class CWengoPhone;
class Settings;

class QStatusBar;
class QMovie;
class QWidget;
namespace Ui { class StatusBarWidget; }

/**
 * Main window status bar.
 *
 * Contains status icons for Internet connection, sound status, SIP (network) status...
 *
 * @author Tanguy Krotoff
 */
class QtStatusBar : public QObjectThreadSafe, public Trackable {
	Q_OBJECT
public:

	QtStatusBar(CWengoPhone & cWengoPhone, QStatusBar * statusBar);

	~QtStatusBar();

	/**
	 * @see QStatusBar::showMessage()
	 */
	void showMessage(const QString & message, int timeout = 0);

	void phoneLineStateChanged(EnumPhoneLineState::PhoneLineState state);

private:

	/**
	 * Initializes widgets content.
	 */
	void init();

	void updateInternetConnectionState();

	void updatePhoneLineState();

	void initThreadSafe() { }

	void checkSoundConfig(Settings & sender, const std::string & key);

	void checkSoundConfigThreadSafe(const std::string & key);

	void connectionIsUpEventHandler();

	void connectionIsDownEventHandler();

	void connectionStateEventHandlerThreadSafe(bool connected);

	CWengoPhone & _cWengoPhone;

	QStatusBar * _statusBar;

	Ui::StatusBarWidget * _ui;

	QWidget * _statusBarWidget;

	QMovie * _internetConnectionMovie;

	QMovie * _sipConnectionMovie;
};

#endif	//OWQTSTATUSBAR_H
