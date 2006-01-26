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

#ifndef QTWENGOPHONE_H
#define QTWENGOPHONE_H

#include <presentation/PWengoPhone.h>

#include <QObjectThreadSafe.h>

#include <QMainWindow>

class CWengoPhone;
class PPhoneLine;
class QtPhoneCall;
class QtWengoPhoneLogger;
class QtContactList;

class QPushButton;
class QComboBox;
class QTabWidget;
class QLayout;

/**
 * Qt Presentation component for WengoPhone.
 *
 * @author Tanguy Krotoff
 */
class QtWengoPhone : public QObjectThreadSafe, public PWengoPhone {
	Q_OBJECT
public:

	QtWengoPhone(CWengoPhone & cWengoPhone);

	void addPhoneLine(PPhoneLine * pPhoneLine);

	void setLogger(QtWengoPhoneLogger * qtWengoPhoneLogger);

	void setContactList(QtContactList * qtContactList);

	void updatePresentation();

	void wengoLoginStateChangedEvent(WengoPhone::LoginState state, const std::string & login, const std::string & password);

	void dialpad(const QString & num);

	QWidget * getWidget() const {
		return _wengoPhoneWindow;
	}

	QPushButton * getCallButton() const {
		return _callButton;
	}

	QComboBox * getPhoneComboBox() const {
		return _phoneComboBox;
	}

private Q_SLOTS:

	void callButtonClicked();

	void enableCallButton();

	void showWengoAccount();

	void openWengoAccount();

	void exitApplication();

	void addContact();

	void showConfig();

	void showForum();

	void showHelp();

	void showAbout();

	void showHome();

	void showContactList();

	void showCallHistory();

private:

	void initThreadSafe();

	/**
	 * Initializes pickup and hangup buttons inside the main window.
	 *
	 * This is called by QtPhoneCall to re-initialize the buttons
	 * since QtPhoneCall modifies the behaviour of this buttons.
	 */
	void initButtons();

	void addPhoneLineThreadSafe(PPhoneLine * pPhoneLine);

	void updatePresentationThreadSafe();

	void wengoLoginStateChangedEventThreadSafe(WengoPhone::LoginState state, std::string login, std::string password);

	void showLoginWindow();

	static QLayout * createLayout(QWidget * parent);


	/** Direct link to the control. */
	CWengoPhone & _cWengoPhone;

	QMainWindow * _wengoPhoneWindow;

	QPushButton * _callButton;

	QComboBox * _phoneComboBox;

	QTabWidget * _tabWidget;
};

#endif	//QTWENGOPHONE_H
