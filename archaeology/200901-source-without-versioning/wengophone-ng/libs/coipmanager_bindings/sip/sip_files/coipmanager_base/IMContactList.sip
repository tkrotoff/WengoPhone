typedef std::list<IMContact> IMContactList;
class IMContactListHelper {
%TypeHeaderCode
#include <coipmanager_base/imcontact/IMContactList.h>
%End
public:
	static bool has(const IMContactList & imContactList, const std::string & imContactId);
	static bool has(const IMContactList & imContactList, const IMContact & imContact);
	static bool hasAssociatedIMContact(const IMContactList & imContactList, const std::string & accountId);
	static bool hasIMContactOfProtocol(const IMContactList & imContactList, EnumAccountType::AccountType accountType);
	static IMContact * getCopyOfIMContact(const IMContactList & imContactList, const std::string & imContactId) /Factory/;
	static IMContact * getCopyOfIMContact(const IMContactList & imContactList, const IMContact & imContact) /Factory/;
	static IMContactList getCopyOfAssociatedIMContacts(const IMContactList & imContactList, const std::string & accountId);
	static IMContactList getCopyOfIMContactsOfProtocol(const IMContactList & imContactList, EnumAccountType::AccountType accountType);
};
