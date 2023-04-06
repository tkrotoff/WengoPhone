class IAccount : Peer {
%TypeHeaderCode
#include <coipmanager_base/account/IAccount.h>
%End
public:
	virtual IAccount * clone() const /Factory/;
	virtual ~IAccount();
	virtual std::string getLogin() const;
	virtual void setLogin(const std::string & login);
	virtual std::string getPassword() const;
	virtual void setPassword(const std::string & password);
	virtual void setConnectionConfigurationSet(bool set);
	virtual bool isConnectionConfigurationSet() const;
	virtual void setSavePassword(bool savePassword);
	virtual bool isPasswordSaved() const;
protected:
	IAccount();
	IAccount(const std::string & login, const std::string & password,
		EnumAccountType::AccountType accountType);
	IAccount(const IAccount & iAccount);
	void copy(const IAccount & iAccount);
	virtual void setPeerId(const std::string & peerId);
	virtual std::string getPeerId() const;
	virtual void setAccountType(EnumAccountType::AccountType accountType);
};
