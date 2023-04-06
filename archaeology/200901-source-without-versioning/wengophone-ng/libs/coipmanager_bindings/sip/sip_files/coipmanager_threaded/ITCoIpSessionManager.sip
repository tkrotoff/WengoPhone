class ITCoIpSessionManager : QObject /Abstract/ {
%TypeHeaderCode
#include <coipmanager_threaded/ITCoIpSessionManager.h>
%End
public:

	bool canCreateTSession(const ContactList & contactList) const;

	bool canCreateTSession(const Account & account, const ContactList & contactList) const;

};
