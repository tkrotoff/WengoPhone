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

#ifndef QTUSER_H
#define QTUSER_H

#include <QtGui>
#include "QtContactPixmap.h"
#include <presentation/PContact.h>

class PContact;
class WengoPhone;

class QtUser : QObject
{
	Q_OBJECT
public:

	enum SizeHint { UserSize = 22};

	QtUser (PContact & pContact, WengoPhone & wengoPhone, QObject * parent = 0);

	virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index);

	QString getId() const;

	void setId(const QString & id);

	QString getUserName() const;

	bool hasIM() const { return _pContact.hasIM();}

	bool hasCall() const { return _pContact.hasCall();}

	bool hasVideo() const { return _pContact.hasVideo();}

	void setMouseOn(bool value) { _mouseOn = value;}

	//QtContactPixmap::contactPixmap getStatus() {return _status;}

	void mouseClicked(const QPoint & pos,const QRect & rec);

	QtContactPixmap::contactPixmap getStatus() const;

	void setFunction(bool im, bool call, bool video);

	int	 getIconsStartPosition() const {return _iconsStartPosition;}

	void setButton(const Qt::MouseButton button);

	void setOpenStatus(bool value);

	bool isOpen() const {
		return _openStatus;
	}

	int getHeight() const;

	Qt::MouseButton getButton() const;

	int getText_y() const { return _centeredText_y;}

	void setAvatar(QString path) {_avatarPath = path;}

	QString getAvatar() const { return _avatarPath;}

	PContact & getPContact() const { return _pContact;}


public Q_SLOTS:

Q_SIGNALS:
	void clicked(QtUser * user,int prt);
protected:
	QString	_userId;

	QString	_userName;

	QPixmap	_imPixmap;

	QPixmap	_callPixmap;

	QPixmap	_videoPixmap;

	QRect _painterRect;

	QtContactPixmap::contactPixmap _status;

	bool		_mouseOn;

	bool		_openStatus;

	int			_iconsStartPosition;

	Qt::MouseButton _mouseButton;

	int _centeredText_y;

	int _centeredPx_y;

	QString _avatarPath;

	PContact & _pContact;

	WengoPhone & _wengoPhone;

};

#endif

