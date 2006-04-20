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

#include <QWidget>
#include <QString>

class QtUserWidgetAvatarManager;

class QLabel;
class CContact;

class QtUserWidget : public QWidget
{
    Q_OBJECT
public:

    QtUserWidget(CContact & cContact, QWidget * parent = 0, Qt::WFlags f=0);

	virtual void paintEvent(QPaintEvent * event);

	void setText(QString text) {
		_text = text;
	}

	QString text() const {
		return _text;
	}

	CContact & getCContact() const {
		return _cContact;
	}

	QLabel * getAvatarLabel() const {
		return _avatarLabel;
	}

protected:

	QLabel * _avatarLabel;

	QWidget * _widget;

	QLabel * _homePhoneLabel;

	QLabel * _cellPhoneLabel;

	QString _text;

	QtUserWidgetAvatarManager * _avatarManager;

	CContact & _cContact;

};

#endif
