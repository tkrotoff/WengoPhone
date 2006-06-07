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

#ifndef CCONTACTLIST_H
#define CCONTACTLIST_H

#include <model/contactlist/ContactProfile.h>

#include <vector>
#include <string>

class ContactList;
class Contact;
class ContactGroup;
class ContactProfile;
class CWengoPhone;
class PContactList;
class Thread;

/**
 *
 * @ingroup control
 * @author Tanguy Krotoff
 */
class CContactList {
	friend class CUserProfile;
public:

	CContactList(ContactList & contactList, Thread & modelThread);

	~CContactList();

	/**
	 * Gets a list of pair of <group UUID, group name>.
	 */
	std::vector< std::pair<std::string, std::string> > getContactGroups() const;

	/**
	 * Gets the name of a ContactGroup given its UUID.
	 *
	 * @param groupId the UUID of the desired group
	 * @return the group name, a null string if not found
	 */
	std::string getContactGroupName(const std::string & groupId) const;

	/**
	 * Gets the UUID of a ContactGroup from its name.
	 *
	 * @param groupName the name of the group to find
	 * @return the UUID of the group. An empty string if not found
	 */
	std::string getContactGroupIdFromName(const std::string & groupName) const;

	/**
	 * Constructs a ContactProfile from a Contact contained in the model.
	 *
	 * If no Contact with the given contactId exists an empty ContactProfile
	 * is returned.
	 *
	 * Used by the GUI to get data from the model.
	 *
	 * @param contactId the Contact UUID od the desired Contact
	 * @return a ContactProfile based on the found Contact
	 */
	ContactProfile getContactProfile(const std::string & contactId) const;

    std::string findContactThatOwns(const IMContact & imContact);

	/**
	 * Gets all existing Contact Ids.
	 *
	 * @return a vector of string containing the UUID of the Contacts.
	 */
	std::vector<std::string> getContactIds() const;

	/**
	 * Add a Contact to the ContactList.
	 *
	 * This method is used by GUI to add a Contact into the model.
	 *
	 * The Contact will be created from the given ContactProfile.
	 *
	 * @param contactProfile the ContactProfile that will be used to create the Contact
	 */
	void addContact(const ContactProfile & contactProfile);

	/**
	 * Removes a Contact from the ContactList.
	 *
	 * @param contactId the UUID of the Contact to remove
	 */
	void removeContact(const std::string & contactId);

	/**
	 * Updates a Contact from a ContactProfile.
	 *
	 * @param contactProfile the ContactProfile which will full the Contact
	 */
	void updateContact(const ContactProfile & contactProfile);

	/**
	 * Adds a contact group.
	 *
	 * @param name name of the ContactGroup to add
	 */
	void addContactGroup(const std::string & name);

	/**
	 * Removes a contact group.
	 *
	 * @param id the UUID of the group to remove
	 */
	void removeContactGroup(const std::string & id);

	/**
	 * Changes a group name.
	 *
	 * @param groupId the UUID of the group to be changed
	 * @param name the desired name
	 */
	void renameContactGroup(const std::string & groupId, const std::string & name);

	/**
	 * Merges two contacts.
	 *
	 * @param dstContactId the id of the destination contact
	 * @param srcContactId the id of the source contact
	 */
	void merge(const std::string & dstContactId, const std::string & srcContactId);

private:

	/**
	 * Returns a pointer to the Contact associated with the given contactId
	 * or NULL if not found.
	 *
	 * Used by the Control to retrieve a Contact grom a contact UUID given by
	 * the GUI.
	 *
	 * @param contactId the UUID of the desired Contact
	 * @return the found Contact
	 */
	Contact * getContact(const std::string & contactId) const;

	/**
	 * @see ContactList::contactAddedEvent
	 */
	void contactAddedEventHandler(ContactList & sender, Contact & contact);

	/**
	 * @see ContactList::contactRemovedEvent
	 */
	void contactRemovedEventHandler(ContactList & sender, Contact & contact);

	/**
	 * @see ContactList::contactGroupAddedEvent
	 */
	void contactGroupAddedEventHandler(ContactList & sender, ContactGroup & contactGroup);

	/**
	 * @see ContactList::contactGroupRemovedEvent
	 */
	void contactGroupRemovedEventHandler(ContactList & sender, ContactGroup & contactGroup);

	/**
	 * @see ContactList::contactGroupRenamedEvent
	 */
	void contactGroupRenamedEventHandler(ContactList & sender, ContactGroup & contactGroup);

	/**
	 * @see ContactList::contactGroupMovedEvent
	 */
	void contactMovedEventHandler(ContactList & sender, ContactGroup & dstContactGroup,
		ContactGroup & srcContactGroup, Contact & contact);

	/**
	 * @see ContactList::contactChangedEvent
	 */
	void contactChangedEventHandler(ContactList & sender, Contact & contact);

	/**
	 * @see addContact
	 */
	void addContactThreadSafe(ContactProfile contactProfile);

	/**
	 * @see removeContact
	 */
	void removeContactThreadSafe(std::string contactId);

	/**
	 * @see updateContact
	 */
	void updateContactThreadSafe(ContactProfile contactProfile);

	/**
	 * @see addContactGroup
	 */
	void addContactGroupThreadSafe(std::string name);

	/**
	 * @see removeContactGroup
	 */
	void removeContactGroupThreadSafe(std::string id);

	/**
	 * @see renameContactGroup
	 */
	void renameContactGroupThreadSafe(std::string groupId, std::string name);

	/**
	 * @see merge
	 */
	void mergeThreadSafe(std::string dstContactId, std::string srcContactId);

	/** Direct link to the model. */
	ContactList & _contactList;

	/** Direct link to the presentation via an interface. */
	PContactList * _pContactList;

	/** Reference to model Thread. Used to post event to the model thread. */
	Thread & _modelThread;
};

#endif	//CCONTACTLIST_H
