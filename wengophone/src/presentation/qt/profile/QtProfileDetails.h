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

#ifndef QTPROFILEDETAILS_H
#define QTPROFILEDETAILS_H

#include <util/NonCopyable.h>

#include <QObject>

class ContactProfile;
class CWengoPhone;
class Profile;
class QWidget;
class QDialog;
class UserProfile;
namespace Ui { class ProfileDetails; }

/**
 * Profile/Contact/UserProfile details window.
 *
 * @see Profile
 * @see UserProfile
 * @see Contact
 * @author Tanguy Krotoff
 */
class QtProfileDetails : public QObject, NonCopyable {
	Q_OBJECT
public:

	QtProfileDetails(CWengoPhone & cWengoPhone,
		ContactProfile & contactProfile, QWidget * parent);

	QtProfileDetails(CWengoPhone & cWengoPhone,
		UserProfile & userProfile, QWidget * parent);

	int show();

	QDialog * getWidget() const {
		return _profileDetailsWindow;
	}

public Q_SLOTS:

	void changeUserProfileAvatar();

private Q_SLOTS:

	void saveContact();

	void saveUserProfile();

	void cancelButtonClicked();

private:

	void init(QWidget * parent);

	void readProfile();

	void readProfileAvatar();

	void saveProfile();

	Ui::ProfileDetails * _ui;

	QDialog * _profileDetailsWindow;

	CWengoPhone & _cWengoPhone;

	Profile & _profile;
};

#endif	//QTPROFILEDETAILS_H
