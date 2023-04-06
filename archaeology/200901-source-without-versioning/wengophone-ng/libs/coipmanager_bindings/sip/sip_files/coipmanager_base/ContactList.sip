typedef std::list<Contact> ContactList;
class ContactListHelper {
%TypeHeaderCode
#include <coipmanager_base/contact/ContactList.h>
%End
public:
	static Contact * getCopyOfContact(const ContactList & contactList, const std::string & uuid) /Factory/;
	static Contact * getCopyOfFirstContactThatOwns(const ContactList & contactList, const std::string & imContactId) /Factory/;
	static Contact * getCopyOfFirstContactThatOwns(const ContactList & contactList, const IMContact & imContact) /Factory/;
	static bool getIMContact(const ContactList & contactList, IMContact & imContact);
};
