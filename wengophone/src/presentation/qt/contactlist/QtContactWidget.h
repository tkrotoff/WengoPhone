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

#ifndef QTCONTACTWIDGET_H
#define QTCONTACTWIDGET_H

#include "ui_ContactWidget.h"

#include <model/contactlist/ContactProfile.h>

#include <QtGui/QWidget>
#include <QtCore/QString>

class CContact;
class CWengoPhone;
class QLabel;
class QPushButton;
namespace Ui { class ContactWidget; }

/**
 * Qt user widget.
 * Represents a contact info in the contact list.
 *
 * @author Mr K
 * @author Mathieu Stute
 */
class QtContactWidget : public QWidget {
	Q_OBJECT
public:

	QtContactWidget(const std::string & contactId, CWengoPhone & cWengoPhone, QWidget * parent);

	~QtContactWidget();

	void setText(const QString & text) {
		_text = text;
	}

	QString text() const {
		return _text;
	}

	QLabel * getAvatarLabel() const;

	QPixmap getIcon() const;

public Q_SLOTS:

	void contactProfileUpdated();

private Q_SLOTS:

	void callButtonClicked();

	void smsButtonClicked();

	void chatButtonClicked();

	void mobileButtonClicked();

	void landlineButtonClicked();

private:

	void paintEvent(QPaintEvent *);

	void paintContact(QPainter * painter, const QRect & rect);

	QPixmap createAvatar();

	static const QString AVATAR_BACKGROUND;

	std::string _contactId;

	ContactProfile _contactProfile;

	CWengoPhone & _cWengoPhone;

	QString _text;

	Ui::ContactWidget * _ui;
};

#endif	//QTCONTACTWIDGET_H
