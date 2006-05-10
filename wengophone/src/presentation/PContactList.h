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

#ifndef PCONTACTLIST_H
#define PCONTACTLIST_H

#include "Presentation.h"

#include <string>

/**
 *
 * @ingroup presentation
 * @author Tanguy Krotoff
 */
class PContactList : public Presentation {
public:

	/**
	 * Called by the control when a group has been added.
	 *
	 * @param contactGroupId the contact group UUID
	 */
	virtual void contactGroupAddedEvent(std::string contactGroupId) = 0;

	/**
	 * Called by the control when a group has been removed.
	 *
	 * @param contactGroupId the contact group UUID
	 */
	virtual void contactGroupRemovedEvent(std::string contactGroupId) = 0;

	/**
	 * Called by the control when a group has been renamed.
	 *
	 * @param contactGroupId the contact group UUID
	 */
	virtual void contactGroupRenamedEvent(std::string contactGroupId) = 0;

	/**
	 * Called by the control when a contact has been added.
	 *
	 * @param contactId the contact UUID
	 */
	virtual void contactAddedEvent(std::string contactId) = 0;

	/**
	 * Called by the control when a contact has been removed.
	 *
	 * @param contactId the contact UUID
	 */
	virtual void contactRemovedEvent(std::string contactId) = 0;

	/**
	 * Called by the control when a contact has moved.
	 *
	 * @param srcContactGroupId the contact group UUID
	 * @param dstContactGroupId the contact group UUID
	 * @param contactId the contact UUID
	 */
	virtual void contactMovedEvent(std::string dstContactGroupId,
		std::string srcContactGroupId, std::string contactId) = 0;

	/**
	 * Called by the control when a contact has changed.
	 *
	 * @param contactId the UUID of the changed Contact
	 */
	virtual void contactChangedEvent(std::string contactId) = 0;
};

#endif	//PCONTACTLIST_H
