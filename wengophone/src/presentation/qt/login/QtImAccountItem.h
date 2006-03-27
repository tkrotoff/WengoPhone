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