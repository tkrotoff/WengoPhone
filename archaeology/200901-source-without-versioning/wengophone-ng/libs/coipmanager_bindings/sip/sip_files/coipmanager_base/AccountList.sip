typedef std::list<Account> AccountList;
class AccountListHelper {
%TypeHeaderCode
#include <coipmanager_base/account/AccountList.h>
%End
public:

	static AccountList getCopyOfAccountsOfProtocol(const AccountList & accountList, 
		EnumAccountType::AccountType accountType);

	static Account * getCopyOfAccount(const AccountList & accountList, const std::string & uuid) /Factory/;

	static Account * getCopyOfAccount(const AccountList & accountList, const std::string & login,
		EnumAccountType::AccountType accountType) /Factory/;
};
