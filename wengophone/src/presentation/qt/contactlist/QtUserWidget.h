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

#ifndef QTUSERWIDGET_H
#define QTUSERWIDGET_H

#include <model/contactlist/ContactProfile.h>

#include <QWidget>
#include <QString>
#include <QMutex>

class QtUserWidgetAvatarManager;

class CContact;
class CUserProfile;
class QLabel;
class QPushButton;

class QtUserWidget : public QWidget
{
    Q_OBJECT
public:

    QtUserWidget(const std::string & contactId, CUserProfile & cUserProfile, QWidget * parent = 0, Qt::WFlags f=0);

//	virtual void paintEvent(QPaintEvent * event);

	void setText(QString text) {
		_text = text;
	}

	QString text() const {
		return _text;
	}

	QLabel * getAvatarLabel() const {
		return _avatarLabel;
	}

	QPixmap getIcon() const;

public Q_SLOTS:

	void contactProfileUpdated();

protected Q_SLOTS:

    void callButtonClicked();

    void smsButtonClicked();

    void chatButtonClicked();

protected:

	QLabel * _avatarLabel;

	QWidget * _widget;

	QLabel * _homePhoneLabel;

	QLabel * _cellPhoneLabel;

	QPushButton * _callButton;

	QPushButton * _smsButton;

	QPushButton * _chatButton;

	QString _text;

	QtUserWidgetAvatarManager * _avatarManager;

	std::string _contactId;

	ContactProfile _contactProfile;

	CUserProfile & _cUserProfile;

	mutable QMutex _mutex;

};

#endif
