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

#ifndef OWQTSIMPLEIMACCOUNTMANAGER_H
#define OWQTSIMPLEIMACCOUNTMANAGER_H

#include <util/NonCopyable.h>

#include <QtCore/QObject>

class CWengoPhone;
class UserProfile;

class QWidget;
namespace Ui { class SimpleIMAccountManager; }

/**
 * Simplified widget for adding an IM accounts.
 *
 * @see QtIMAccountManager
 * @see QtSimpleIMContactManager
 * @author Tanguy Krotoff
 */
class QtSimpleIMAccountManager : public QObject, NonCopyable {
	Q_OBJECT
public:

	QtSimpleIMAccountManager(UserProfile & userProfile, QWidget * parent);

	~QtSimpleIMAccountManager();

	QWidget * getWidget() const;

	void saveIMContacts();

Q_SIGNALS:

	void advancedClicked();

private Q_SLOTS:

	void advancedClickedSlot();

private:

	UserProfile & _userProfile;

	Ui::SimpleIMAccountManager * _ui;

	QWidget * _imAccountManagerWidget;
};

#endif	//OWQTSIMPLEIMACCOUNTMANAGER_H
