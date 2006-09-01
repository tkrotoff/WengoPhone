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

#ifndef OWQTSIMPLEIMCONTACTMANAGER_H
#define OWQTSIMPLEIMCONTACTMANAGER_H

#include <util/NonCopyable.h>

#include <imwrapper/EnumIMProtocol.h>

#include <QtCore/QObject>

#include <set>

class IMContact;
class ContactProfile;
class CUserProfile;

class QWidget;
namespace Ui { class SimpleIMContactManager; }

/**
 * Simplified widget for adding an IM contact to a given contact.
 *
 * @see QtIMContactManager
 * @see QtSimpleIMAccountManager
 * @author Tanguy Krotoff
 */
class QtSimpleIMContactManager : public QObject, NonCopyable {
	Q_OBJECT
public:

	QtSimpleIMContactManager(ContactProfile & contactProfile,
		CUserProfile & cUserProfile, QWidget * parent);

	~QtSimpleIMContactManager();

	QWidget * getWidget() const;

	void saveIMContacts();

Q_SIGNALS:

	void advancedClicked();

private Q_SLOTS:

	void advancedClickedSlot();

	void searchWengoContactButtonClicked();

private:

	void loadIMContacts();

	/**
	 * Code factorization.
	 */
	std::set<IMContact *> findIMContactsOfProtocol(EnumIMProtocol::IMProtocol imProtocol) const;

	/**
	 * Code factorization.
	 */
	QString getIMContactsOfProtocol(EnumIMProtocol::IMProtocol imProtocol) const;

	/**
	 * Code factorization.
	 */
	void addIMContact(EnumIMProtocol::IMProtocol imProtocol, const QString & text);

	Ui::SimpleIMContactManager * _ui;

	QWidget * _imContactManagerWidget;

	CUserProfile & _cUserProfile;

	ContactProfile & _contactProfile;
};

#endif	//OWQTSIMPLEIMCONTACTMANAGER_H