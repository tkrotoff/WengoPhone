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

#ifndef QTUSER_H
#define QTUSER_H

#include <model/contactlist/ContactProfile.h>

#include "QtContactPixmap.h"

#include <QtGui>

class CWengoPhone;

class QtContact : QObject {
	Q_OBJECT
public:

	enum SizeHint { UserSize = 22 };

	QtContact(const std::string & contactId, CWengoPhone & wengoPhone, QObject * parent = 0);

	virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index);

	QString getId() const;

	QString getUserName() const;

	bool hasIM() const;

	bool hasCall() const;

	bool hasFreeCall() const;

	bool hasVideo() const;

	void setMouseOn(bool value) { _mouseOn = value; }

	void mouseClicked(const QPoint & pos, const QRect & rec);

	QtContactPixmap::ContactPixmap getStatus() const;

	void setFunction(bool im, bool call, bool video);

	void setButton(const Qt::MouseButton button);

	void setOpenStatus(bool value);

	bool isOpen() const {
		return _openStatus;
	}

	int getHeight() const;

	Qt::MouseButton getButton() const;

	int getText_y() const { return _centeredText_y; }

	void setAvatar(QString path) { _avatarPath = path; }

	QString getAvatar() const { return _avatarPath; }

	void startChat();

	void startSMS();

	void startCall(const QString & number);

	void startCall();

	void startFreeCall();

	QString getMobilePhone() const;

	QString getHomePhone() const;

	QString getWorkPhone() const;

	QString getWengoPhoneNumber() const;

	QString getPreferredNumber() const;

	bool hasPhoneNumber() const;

	QString getDisplayName() const;

	EnumPresenceState::PresenceState getPresenceState() const;

public Q_SLOTS:

	void contactUpdated();

Q_SIGNALS:

	void clicked(QtContact * user, int prt);

protected:

	QString _userName;

	QPixmap _imPixmap;

	QPixmap _callPixmap;

	QPixmap _videoPixmap;

	QtContactPixmap::ContactPixmap _status;

	bool _mouseOn;

	bool _openStatus;

	int _iconsStartPosition;

	Qt::MouseButton _mouseButton;

	int _centeredText_y;

	int _centeredPx_y;

	QString _avatarPath;

	/**
	 * The UUID of the Contact associated with this QtContact.
	 */
	std::string _contactId;

	ContactProfile _contactProfile;

	CWengoPhone & _cWengoPhone;

	mutable QMutex _mutex;
};

#endif	//QTUSER_H
