class AIMAccount : OscarAccount {
%TypeHeaderCode
#include <coipmanager_base/account/AIMAccount.h>
%End
public:
	AIMAccount();
	AIMAccount(const std::string & login, const std::string & password);
	AIMAccount(const AIMAccount & account);
	explicit AIMAccount(const IAccount * iAccount);
	virtual ~AIMAccount();
	virtual AIMAccount * clone() const;
protected:
	void copy(const AIMAccount & account);
};
