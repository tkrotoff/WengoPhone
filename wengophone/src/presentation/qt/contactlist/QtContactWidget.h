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

#include "ui_UserWidget.h"

#include <model/contactlist/ContactProfile.h>

#include <QWidget>
#include <QString>

class CContact;
class CWengoPhone;
class QLabel;
class QPushButton;

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

	static const QString AVATAR_BACKGROUND;

	QtContactWidget(const std::string & contactId, CWengoPhone & cWengoPhone,
			QWidget * parent = 0, Qt::WFlags f = 0);

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

	void landLineButtonClicked();

protected:

    virtual void paintEvent(QPaintEvent *);

private:

    void paintContact(QPainter * painter, const QRect & rect);

	QPixmap createAvatar();

	QWidget * _widget;

	std::string _contactId;

	ContactProfile _contactProfile;

	CWengoPhone & _cWengoPhone;

	QString _text;

	Ui::UserWidget _ui;
};

#endif	//QTCONTACTWIDGET_H
