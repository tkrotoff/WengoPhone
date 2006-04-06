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

#ifndef QTIMACCOUNTITEM_H
#define QTIMACCOUNTITEM_H

#include <QTreeWidgetItem>

class UserProfile;
class IMAccount;
class WengoPhone;

class QtIMAccountItem : public QTreeWidgetItem {
public:

	QtIMAccountItem(QTreeWidget * parent, const QStringList & strings);

	void setIMAccount(IMAccount * imAccount){
		_imAccount = imAccount;
	}

	IMAccount * getIMAccount() const {
		return _imAccount;
	}

	void setUserProfile(UserProfile * profile) {
		_userProfile = profile;
	}

	UserProfile * getUserProfile() const {
		return _userProfile;
	}

	void setWengoPhone(WengoPhone * wengophone) {
		_wengoPhone = wengophone;
	}

	WengoPhone * getWengoPhone() const {
		return _wengoPhone;
	}

private:

	IMAccount * _imAccount;

	UserProfile * _userProfile;

	WengoPhone * _wengoPhone;
};

#endif	//QTIMACCOUNTITEM_H
