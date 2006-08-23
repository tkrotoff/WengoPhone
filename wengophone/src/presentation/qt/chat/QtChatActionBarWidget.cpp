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

#include "QtChatActionBarWidget.h"

#include <model/contactlist/ContactProfile.h>

#include <control/contactlist/CContactList.h>

#include <presentation/qt/contactlist/QtContactList.h>
#include <presentation/qt/contactlist/QtContactListManager.h>
#include <presentation/qt/chat/QtChatWidget.h>
#include <presentation/qt/QtWengoPhone.h>

#include <qtutil/QtWengoStyleLabel.h>

#include <util/String.h>

#include <QtGui/QtGui>

#if defined(OS_WINDOWS)
	#include <windows.h>
#endif // OS_WINDOWS

QtChatActionBarWidget::QtChatActionBarWidget(QtWengoPhone * qtWengoPhone, QtChatWidget * chatWidget, QWidget * parent) :
	QtWengoStyleBar(parent), _qtWengoPhone(qtWengoPhone), _chatWidget(chatWidget) {

	setMaximumSize(QSize(10000, 70));
	setMinimumSize(QSize(16, 70));

	_callLabel = new QtWengoStyleLabel(this);
	_callLabel->setPixmaps(
		QPixmap(":/pics/chat/chat_call_bar_button.png"),
		QPixmap(),
		QPixmap(),
		QPixmap(":/pics/chat/chat_call_bar_button_on.png"),
		QPixmap(),
		QPixmap()
		);
	_callLabel->setMaximumSize(QSize(46, 65));
	_callLabel->setMinimumSize(QSize(46, 46));
	connect(_callLabel, SIGNAL(clicked()), SLOT(callContact()));

	_endLabel = new QtWengoStyleLabel(this);
	_endLabel->setPixmaps(
		QPixmap(),
		QPixmap(":/pics/profilebar/bar_end.png"),
		QPixmap(":/pics/profilebar/bar_fill.png"),
		QPixmap(),
		QPixmap(":/pics/profilebar/bar_end.png"),
		QPixmap(":/pics/profilebar/bar_fill.png")
	);
	_endLabel->setMinimumSize(QSize(16, 65));

	init(_callLabel, _endLabel);
	
	addLabel(QString("inviteLabel"),
		QPixmap(":/pics/chat/invite.png"),
		QPixmap(":/pics/chat/invite_on.png"),
		QSize(36, 65));

	connect(_labels["inviteLabel"], SIGNAL(clicked()), _chatWidget, SLOT(showInviteDialog()));
}

void QtChatActionBarWidget::callContact() {
	QString contactId;
	QtContactList * qtContactList;
	ContactProfile contactProfile;
	QtContactListManager * ul ;
	if (_chatWidget) {
		contactId = _chatWidget->getContactId();
		qtContactList = _qtWengoPhone->getContactList();
		contactProfile = qtContactList->getCContactList().getContactProfile(contactId.toStdString());
		ul = QtContactListManager::getInstance();
		ul->startCall(contactId);
		if (_qtWengoPhone->getWidget()->isMinimized()) {
			_qtWengoPhone->getWidget()->showNormal();
		}
#if defined(OS_WINDOWS)
		SetForegroundWindow(_qtWengoPhone->getWidget()->winId());
#endif
	}
}
