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

#ifndef QTWSDIRECTORYCONTACT_H
#define QTWSDIRECTORYCONTACT_H

#include <QObject>

class QtWsDirectory;
class ContactProfile;

class QString;
class QWidget;
namespace Ui { class WsDirectoryContact; }

/**
 * Contact widget for search result.
 *
 * @author Mathieu Stute
 */
class QtWsDirectoryContact : public QObject {
	Q_OBJECT
public:

	QtWsDirectoryContact(QtWsDirectory * qtWsDirectory, ContactProfile * contact, bool online, QWidget * parent);

	~QtWsDirectoryContact();

	QWidget * getWidget() const {
		return _wsDirectoryWidget;
	}

	void setNickname(const QString & nickname);

	void setWengoNumber(const QString & number);

	void setSipAddress(const QString & sipAddress);

	void setName(const QString & name);

	void setLocality(const QString & locality);

	void setCountry(const QString & country);

	void setOnline(bool online);

private Q_SLOTS:

	void callLabelClicked();

	void addLabelClicked();

private:

	QtWsDirectory * _qtWsDirectory;

	ContactProfile * _contact;

	Ui::WsDirectoryContact * _ui;

	QWidget * _wsDirectoryWidget;
};

#endif	//QTWSDIRECTORYCONTACT_H
