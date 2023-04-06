class TSession : TCoIpModule /Abstract/ {
%TypeHeaderCode
#include <coipmanager_threaded/session/TSession.h>
%End
public:
	EnumSessionError::SessionError addContact(const Contact & contact);
	void removeContact(const std::string & contactId);
	EnumSessionError::SessionError setContactList(const ContactList & contactList);
	ContactList getContactList() const;
	Account * getAccount() const /Factory/;
signals:
	void contactAddedSignal(Contact contact);
	void contactRemovedSignal(Contact contact);
};
