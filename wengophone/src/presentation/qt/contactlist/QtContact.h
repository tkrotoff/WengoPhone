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

#ifndef QTCONTACT_H
#define QTCONTACT_H

#include <presentation/PContact.h>

#include <control/contactlist/CContact.h>

#include <qtutil/QObjectThreadSafe.h>

class QtContactList;

/**
 * Qt Presentation component for Contact.
 *
 * @author Tanguy Krotoff
 */
class QtContact : public QObjectThreadSafe, public PContact {
public:

	QtContact(CContact & cContact, QtContactList * qtContactList);

	~QtContact();

	void updatePresentation();

	std::string getDisplayName() const;

	std::string getId() const;

	bool hasIM() const;

	bool hasCall() const;

	bool hasVideo() const;

	EnumPresenceState::PresenceState getPresenceState() const;

private:

	void contactModifiedEventHandler(Contact & sender);

	void initThreadSafe();

	void updatePresentationThreadSafe();

	CContact & _cContact;

	QtContactList * _qtContactList;

};

#endif	//QTCONTACT_H
