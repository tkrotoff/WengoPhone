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

#ifndef OW_QTDIRECTORYCONTACT_H
#define OW_QTDIRECTORYCONTACT_H

#include "ui_WsDirectoryContact.h"

#include <QWidget>

class QtWsDirectory;
class Profile;

/**
 * Contact widget for search result.
 *
 * @author Mathieu Stute
 */
class QtWsDirectoryContact : public QWidget {
	Q_OBJECT
public:

	QtWsDirectoryContact(QtWsDirectory * qtWsDirectory, Profile * profile, bool online, QWidget * parent = 0);

	~QtWsDirectoryContact();

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

	Profile * _profile;

	Ui::WsDirectoryContact ui;
};

#endif	//OW_QTDIRECTORYCONTACT_H
