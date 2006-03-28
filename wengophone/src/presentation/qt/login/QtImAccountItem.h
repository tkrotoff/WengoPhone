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

#ifndef QTIMACCOUNTITEM_H
#define QTIMACCOUNTITEM_H

#include <QtGui>

#include <model/profile/UserProfile.h>
#include <imwrapper/IMAccount.h>
#include <model/WengoPhone.h>

class QtImAccountItem : public QTreeWidgetItem
{

public:
	QtImAccountItem ( QTreeWidget * parent, const QStringList & strings, int type = Type );
	/*
	QtImAccountItem ( int type = Type )
	QtImAccountItem ( const QStringList & strings, int type = Type )
	QtImAccountItem ( QTreeWidget * parent, int type = Type )
	QtImAccountItem ( QTreeWidget * parent, const QStringList & strings, int type = Type )
	QtImAccountItem ( QTreeWidget * parent, QTreeWidgetItem * preceding, int type = Type )
	QtImAccountItem ( QTreeWidgetItem * parent, int type = Type )
	QtImAccountItem ( QTreeWidgetItem * parent, const QStringList & strings, int type = Type )
	QtImAccountItem ( QTreeWidgetItem * parent, QTreeWidgetItem * preceding, int type = Type )
	QtImAccountItem ( const QTreeWidgetItem & other )
	*/

	void setImAccount(IMAccount * imaccount);

	IMAccount * getImAccount();

	void setUserProfile(UserProfile * profile) { _userProfile = profile;};

	UserProfile * getUserProfile(){ return _userProfile;};

	void setWengoPhone(WengoPhone * wengophone) {_wengoPhone = wengophone;};

	WengoPhone * getWengoPhone() { return _wengoPhone;};

protected:

protected:

	IMAccount * _imAccount;
	UserProfile * _userProfile;
	WengoPhone * _wengoPhone;

};

#endif