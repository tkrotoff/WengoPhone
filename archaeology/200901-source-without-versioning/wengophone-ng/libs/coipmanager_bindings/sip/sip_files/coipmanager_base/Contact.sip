class Contact : Profile {

%TypeHeaderCode
#include <coipmanager_base/contact/Contact.h>
%End

public:

	Contact();

	Contact(const Contact & contact);

	virtual Contact * clone() const /Factory/;

	~Contact();

	/**
	 * Add an IMContact to the Contact.
	 *
	 * @param imContact IMContact to add
	 * @return true if actually added (was not present)
	 */
	bool addIMContact(const IMContact & imContact);

	/**
	 * Remove an IMContact from the Contact.
	 *
	 * @param imContactId the id of the IMContact to remove
	 * @return true if actually removed (was present)
	 */
	bool removeIMContact(const std::string & imContactId);

	/**
	 * Updates an IMContact.
	 *
	 * @return true if actually updated (was present)
	 */
	bool updateIMContact(const IMContact & imContact);

	/**
	 * Gets the IMContactList.
	 */
	const IMContactList & getIMContactList() const;

	/**
	 * Adds the Contact to a group/tag.
	 *
	 * @return true if the Contac was not in the group
	 */
	bool addToGroup(const std::string & groupId);

	/**
	 * Removes the Contact from a group/tag.
	 *
	 * @return true if Contact was actually in the group
	 */
	bool removeFromGroup(const std::string & groupId);

	/**
	 * Gets the group/tag ids list to whom this Contact belongs to.
	 */
	StringList getGroupList() const;

	/**
	 * Sets the group/tag list of this Contact.
	 */
	void setGroupList(const StringList & stringList);

	std::string getAlias() const;

	EnumPresenceState::PresenceState getPresenceState() const;

	/**
	 * @return a display name computed from available Contact information.
	 */
	std::string getDisplayName() const;

	/**
	 * @return a display name in a short way.
	 */
	std::string getShortDisplayName() const;
};
