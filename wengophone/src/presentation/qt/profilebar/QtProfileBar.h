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

#ifndef QTPROFILEBAR_H
#define QTPROFILEBAR_H

#include "QtNickNameWidget.h"
#include "QtEventWidget.h"
#include "QtCreditWidget.h"

#include <model/webservices/info/WsInfo.h>

#include <imwrapper/EnumPresenceState.h>

#include <QtGui>

class CWengoPhone;
class IPhoneLine;
class QtWengoStyleLabel;
class CUserProfile;
class ConnectHandler;
class IMAccount;
class PresenceHandler;
class UserProfile;
class CHistory;

/**
 * Credit widget
 *
 * @author Mr K.
 * @author Mathieu Stute
 */
class QtProfileBar : public QWidget {

	Q_OBJECT

public:

	QtProfileBar (CWengoPhone & cWengoPhone, CUserProfile & cUserProfile,
		ConnectHandler & connectHandler,QWidget * parent = 0, Qt::WFlags f = 0 );

	~QtProfileBar();

Q_SIGNALS:

	void connectedEventSignal(IMAccount * imAccount);

	void disconnectedEventSignal(IMAccount * imAccount, bool connectionError, const QString & reason);

	void connectionProgressEventSignal(IMAccount * imAccount, int currentStep, int totalSteps, const QString & infoMessage);

	void myPresenceStatusEventSignal(QVariant status);

	void updatedTranslationSignal();

	void wsInfoWengosEvent(float wengos);
	void wsInfoVoiceMailEvent(int count);
	void wsInfoPtsnNumberEvent(const QString & number);
	void wsCallForwardInfoEvent(const QString & mode);
	void phoneLineCreatedEvent();

protected:

	ConnectHandler & _connectHandler;

	QGridLayout * _gridlayout;

	QGridLayout * _widgetLayout;

	QtWengoStyleLabel * _statusLabel;

	QtWengoStyleLabel * _nicknameLabel;

	QtWengoStyleLabel * _eventsLabel;

	QtWengoStyleLabel * _creditLabel;

	QtNickNameWidget * _nickNameWidget;

	QtCreditWidget * _creditWidget;

	QtEventWidget * _eventWidget;

	bool _nickNameWidgetVisible;

	bool _eventsWidgetVisible;

	bool _crediWidgetVisible;

	bool _isOpen;

	QPixmap _statusPixmap;

	QMenu * _statusMenu;

	CUserProfile & _cUserProfile;

	CWengoPhone & _cWengoPhone;

	void createStatusMenu();

	void showNickNameWidget();

	void removeNickNameWidget();

	void showEventsWidget();

	void removeEventsWidget();

	void showCreditWidget();

	void removeCreditWidget();

	void setOpen(bool opened);

	void paintEvent ( QPaintEvent * event );

public Q_SLOTS:

	void statusClicked();

	void nicknameClicked();

	void eventsClicked();

	void creditClicked();

	void setAway();

	void setInvisible();

	void setDND();

	void setOnline();

	// Status menu actions
	void onlineClicked(bool checked);

	void dndClicked(bool checked);

	void invisibleClicked(bool checked);

	void awayClicked(bool checked);

	void forwardClicked(bool checked);

	void myPresenceStatusEventSlot(QVariant status);

	void slotTranslationChanged();

private Q_SLOTS:

	void wsInfoWengosEventSlot(float wengos);
	void wsInfoVoiceMailEventSlot(int count);
	void wsInfoPtsnNumberEventSlot(const QString & number);
	void wsCallForwardInfoEventSlot(const QString & mode);
	void phoneLineCreatedEventSlot();

private:

	/**
	 * Initializes widgets content.
	 */
	void init();

	void connectedEventHandler(ConnectHandler & sender, IMAccount & imAccount);

	void disconnectedEventHandler(ConnectHandler & sender, IMAccount & imAccount,
			bool connectionError, const std::string & reason);

	void connectionProgressEventHandler(ConnectHandler & sender, IMAccount & imAccount,
			int currentStep, int totalSteps, const std::string & infoMessage);

	void myPresenceStatusEventHandler(PresenceHandler & sender,
		const IMAccount & imAccount, EnumPresenceState::MyPresenceStatus status);

	void wsInfoCreatedEventHandler(UserProfile & sender, WsInfo & wsInfo);

	void wsInfoWengosEventHandler(WsInfo & sender, int id, WsInfo::WsInfoStatus status, float wengos);

	void wsInfoVoiceMailEventHandler(WsInfo & sender, int id, WsInfo::WsInfoStatus status, int voicemail);

	void wsInfoPtsnNumberEventHandler(WsInfo & sender, int id, WsInfo::WsInfoStatus status, std::string number);

	void wsCallForwardInfoEventHandler(WsInfo & sender, int id, WsInfo::WsInfoStatus status,
		WsInfo::WsInfoCallForwardMode mode, bool voicemail, std::string dest1, std::string dest2, std::string dest3);

	void cHistoryCreatedEventHandler(CWengoPhone & sender, CHistory &  cHistory);

	void unseenMissedCallsChangedEventHandler(CHistory &, int count);

	void phoneLineCreatedEventHandler(UserProfile & sender, IPhoneLine & phoneLine);

	void setStatusLabel(const QString & on, const QString & off);
};

#endif	//QTPROFILEBAR_H
