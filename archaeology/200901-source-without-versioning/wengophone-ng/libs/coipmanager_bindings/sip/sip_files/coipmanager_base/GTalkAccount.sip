class GTalkAccount : JabberAccount {
%TypeHeaderCode
#include <coipmanager_base/account/GTalkAccount.h>
%End
public:
	GTalkAccount();
	GTalkAccount(const std::string & login, const std::string & password);
	GTalkAccount(const GTalkAccount & account);
	explicit GTalkAccount(const IAccount * iAccount);
	virtual ~GTalkAccount();
	virtual GTalkAccount * clone() const /Factory/;
protected:
	void copy(const GTalkAccount & account);
};
