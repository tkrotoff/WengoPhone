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

#include "ContactGroupPopupMenu.h"

#include "model/contactlist/ContactGroup.h"

#include <QtGui>

#include <iostream>
using namespace std;

ContactGroupPopupMenu::ContactGroupPopupMenu(QWidget * parent) : QMenu(parent) {
	_contactGroup = NULL;

	addAction(QIcon(":/pics/contact_remove.png"), tr("Remove Contact Group"), this, SLOT(removeContactGroup()));
	addAction(tr("Rename Contact Group"), this, SLOT(renameContactGroup()));
}

void ContactGroupPopupMenu::showMenu(const ContactGroup * contactGroup) {
	if (!contactGroup) {
		return;
	}

	_contactGroup = contactGroup;
	exec(QCursor::pos());
}

void ContactGroupPopupMenu::removeContactGroup() const {
	
}

void ContactGroupPopupMenu::renameContactGroup() const {
	
}
