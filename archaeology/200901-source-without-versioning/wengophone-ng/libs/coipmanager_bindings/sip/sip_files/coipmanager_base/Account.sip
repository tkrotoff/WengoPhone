class Account : IAccount {
%TypeHeaderCode
#include <coipmanager_base/account/Account.h>
%End
public:
	Account();
	Account(const std::string & login, const std::string & password,
		EnumAccountType::AccountType accountType);
	Account(const Account & account);
	Account(const IAccount * iAccount);
	virtual Account * clone() const /Factory/;
	virtual ~Account();
	virtual std::string getLogin() const;
	virtual void setLogin(const std::string & login);
	virtual std::string getPassword() const;
	virtual void setPassword(const std::string & password);
	virtual std::string getAlias() const;
	virtual void setAlias(const std::string & alias);
	virtual EnumAccountType::AccountType getAccountType() const;
	virtual void setPresenceState(EnumPresenceState::PresenceState presenceState);
	virtual EnumPresenceState::PresenceState getPresenceState() const;
	virtual void setConnectionState(EnumConnectionState::ConnectionState state);
	virtual EnumConnectionState::ConnectionState getConnectionState() const;
	virtual void setConnectionConfigurationSet(bool set);
	virtual bool isConnectionConfigurationSet() const;
	virtual void setSavePassword(bool savePassword);
	virtual bool isPasswordSaved() const;
	IAccount * getPrivateAccount() const;
protected:
	void copy(const Account & account);
	virtual void setAccountType(EnumAccountType::AccountType accountType);
};
