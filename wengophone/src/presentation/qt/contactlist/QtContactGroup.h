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

#ifndef QTCONTACTGROUP_H
#define QTCONTACTGROUP_H

#include <presentation/PContactGroup.h>
#include <presentation/PContact.h>

#include <control/contactlist/CContactGroup.h>

#include <QObjectThreadSafe.h>

class QtContactList;

class QtContactGroup : public QObjectThreadSafe, public PContactGroup {
public:

	QtContactGroup(CContactGroup & cContactGroup, QtContactList * qtContactList);

	~QtContactGroup();

	std::string getDisplayName() const;

	void addContact(PContact * pContact);

	void updatePresentation();

private:

	void initThreadSafe();

	void updatePresentationThreadSafe();

	void addContactThreadSafe(PContact * pContact);

	CContactGroup & _cContactGroup;

	QtContactList *_qtContactList;
};

#endif	//QTCONTACTGROUP_H

