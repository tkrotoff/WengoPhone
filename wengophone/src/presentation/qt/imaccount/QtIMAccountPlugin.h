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

#ifndef QTIMACCOUNTPLUGIN_H
#define QTIMACCOUNTPLUGIN_H

#include <QObject>

#include <util/Interface.h>

class IMAccount;
class UserProfile;
class QWidget;

/**
 * Interface for IM accounts (MSN, AIM, Yahoo, Jabber...).
 *
 * @author Tanguy Krotoff
 */
class QtIMAccountPlugin : public QObject, Interface {
	Q_OBJECT
public:

	QtIMAccountPlugin(UserProfile & userProfile, IMAccount * imAccount, QWidget * parent);

	virtual QWidget * getWidget() const = 0;

public Q_SLOTS:

	virtual void save() = 0;

protected:

	virtual void init() = 0;

	IMAccount * _imAccount;

	UserProfile & _userProfile;

	QWidget * _parentWidget;
};

#endif	//QTIMACCOUNTPLUGIN_H
