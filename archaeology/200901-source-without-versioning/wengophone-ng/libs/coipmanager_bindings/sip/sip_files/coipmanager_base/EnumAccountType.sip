class EnumAccountType : NonCopyable {
%TypeHeaderCode
#include <coipmanager_base/EnumAccountType.h>
%End
public:
	enum AccountType {
		AccountTypeUnknown,
		AccountTypeAIM,
		AccountTypeGTalk,
		AccountTypeIAX,
		AccountTypeICQ,
		AccountTypeJabber,
		AccountTypeMSN,
		AccountTypeSIP,
		AccountTypeWengo,
		AccountTypeYahoo,
	};
	static std::string toString(AccountType AccountType);
	static AccountType toAccountType(const std::string & accountType);
};
