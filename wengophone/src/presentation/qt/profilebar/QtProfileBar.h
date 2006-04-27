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

#include <QtGui>

// #include "QtClickableLabel.h"
// #include "QtProfileLabel.h"
#include "QtNickNameWidget.h"
#include "QtEventWidget.h"
#include "QtCreditWidget.h"

#include <model/webservices/info/WsInfo.h>

#include <idle/Idle.h>

class UserProfile;
class CWengoPhone;
class QtWengoStyleLabel;

class QtProfileBar : public QWidget
{
	Q_OBJECT

public:
	QtProfileBar (CWengoPhone & cWengoPhone, UserProfile & userProfile, QWidget * parent = 0, Qt::WFlags f = 0 );

	void setWengos(float wengos);

protected:

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

	QPixmap _statusPixmap;

	QMenu * _statusMenu;

	UserProfile & _userProfile;

	CWengoPhone & _cWengoPhone;

	void createStatusMenu();

	void showNickNameWidget();

	void removeNickNameWidget();

	void showEventsWidget();

	void removeEventsWidget();

	void showCreditWidget();

	void removeCreditWidget();

    void setOpen(bool opened);

public Q_SLOTS:

	void statusClicked();

	void nicknameClicked();

	void eventsClicked();

	void creditClicked();

	// Status menu actions
	void onlineClicked(bool checked);

	void dndClicked(bool checked);

	void invisibleClicked(bool checked);

	void awayClicked(bool checked);

	void forwardClicked(bool checked);

private:

	void wsInfoCreatedEventHandler(UserProfile & sender, WsInfo & wsInfo);

	void wsInfoWengosEventHandler(WsInfo & sender, int id, WsInfo::WsInfoStatus status, float wengos);

	void wsInfoVoiceMailEventHandler(WsInfo & sender, int id, WsInfo::WsInfoStatus status, int voicemail);

	/**
	 * Idle status has changed, IMAccount presence state should be changed.
	 */
	void idleStatusChangedEventHandler(Idle & sender, Idle::Status status);
};

#endif	//QTPROFILEBAR_H
