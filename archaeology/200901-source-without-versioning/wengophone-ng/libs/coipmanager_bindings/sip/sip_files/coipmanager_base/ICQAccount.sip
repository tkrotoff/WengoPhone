class ICQAccount : OscarAccount {
%TypeHeaderCode
#include <coipmanager_base/account/ICQAccount.h>
%End
public:
	ICQAccount();
	ICQAccount(const std::string & login, const std::string & password);
	ICQAccount(const ICQAccount & account);
	explicit ICQAccount(const IAccount * iAccount);
	virtual ~ICQAccount();
	virtual ICQAccount * clone() const;
protected:
	void copy(const ICQAccount & account);
};
