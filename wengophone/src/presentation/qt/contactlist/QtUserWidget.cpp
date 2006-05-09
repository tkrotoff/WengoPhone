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

#include "QtUserWidget.h"

#include "QtUserWidgetAvatarManager.h"
#include "QtUserList.h"

#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>

#include <util/Picture.h>

#include <qtutil/WidgetFactory.h>

#include <QtGui>

QtUserWidget::QtUserWidget(const std::string & contactId, CUserProfile & cUserProfile, QWidget * parent, Qt::WFlags f)
: QWidget(parent, f), _cUserProfile(cUserProfile) {

	_contactId = contactId;

	contactProfileUpdated();

	_widget = WidgetFactory::create(":/forms/contactlist/userWidget.ui", NULL);
	QGridLayout * layout = new QGridLayout();
	layout->addWidget(_widget);
	layout->setMargin(0);
	setLayout(layout);

 	_avatarLabel = findChild<QLabel *>("avatarLabel");
	_homePhoneLabel = findChild<QLabel *>("homePhoneLabel");
	_cellPhoneLabel = findChild<QLabel *>("cellPhoneLabel");

	_avatarManager = new QtUserWidgetAvatarManager(this,_avatarLabel);

	_avatarLabel->installEventFilter(_avatarManager);

    _smsButton = findChild<QPushButton *>("smsButton");
    _callButton = findChild<QPushButton *>("callButton");
    _chatButton = findChild<QPushButton *>("chatButton");

    QString str = QString::fromUtf8(_contactProfile.getHomePhone().c_str());
    if (!str.isEmpty()) {
        _homePhoneLabel->setText( str );
    }

    str = QString::fromUtf8(_contactProfile.getMobilePhone().c_str());
    if ( !str.isEmpty() )
    {
        _cellPhoneLabel->setText( str );
    }
    else{
        _smsButton->setEnabled(false);
    }

	if (!_contactProfile.hasFreeCall()) {
		_callButton->setEnabled(false);
	}

	if (!_contactProfile.hasIM()) {
		_chatButton->setEnabled(false);
	}

    connect ( _callButton, SIGNAL(clicked()), SLOT ( callButtonClicked() ));
    connect ( _chatButton, SIGNAL(clicked()), SLOT ( chatButtonClicked() ));
    connect ( _smsButton,  SIGNAL(clicked()), SLOT ( smsButtonClicked() ));
}
/*
void QtUserWidget::paintEvent(QPaintEvent * event){

	QPalette  p = palette();
    QPainter painter(this);
	painter.fillRect(rect(),QBrush(QColor(255,255,128)));
}
*/

void QtUserWidget::callButtonClicked(){
    QtUserList * ul = QtUserList::getInstance();
	ul->startCall(QString::fromStdString(_contactId));
}

void QtUserWidget::smsButtonClicked(){
    QtUserList * ul = QtUserList::getInstance();
    ul->startSMS(QString::fromStdString(_contactId));
}

void QtUserWidget::chatButtonClicked(){
    QtUserList * ul = QtUserList::getInstance();
    ul->startChat(QString::fromStdString(_contactId));
}

QPixmap QtUserWidget::getIcon() const {
	QMutexLocker locker(&_mutex);

	Picture picture = _contactProfile.getIcon();
	std::string data = picture.getData();

	QPixmap result;
	result.loadFromData((uchar *)data.c_str(), data.size());

	return result;
}

void QtUserWidget::contactProfileUpdated() {
	QMutexLocker locker(&_mutex);

	_contactProfile = _cUserProfile.getCContactList().getContactProfile(_contactId);
}

