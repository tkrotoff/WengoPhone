class WengoAccount : SipAccount {
%TypeHeaderCode
#include <coipmanager_base/account/WengoAccount.h>
%End
public:
	WengoAccount();
	WengoAccount(const std::string & wengoLogin, const std::string & wengoPassword);
	WengoAccount(const WengoAccount & wengoAccount);
	explicit WengoAccount(const IAccount * iAccount);
	virtual WengoAccount * clone() const /Factory/;
	virtual ~WengoAccount();
	void setWengoLogin(const std::string & wengoLogin);
	const std::string & getWengoLogin() const;
	void setWengoPassword(const std::string & wengoPassword);
	const std::string & getWengoPassword() const;
	virtual std::string getLogin() const;
	virtual void setLogin(const std::string & login);
	void setSSORequestMade(bool made);
	bool isSSORequestMade() const;
	void setStunServer(const std::string & server);
	const std::string & getStunServer() const;
private:
	void copy(const WengoAccount & wengoAccount);
	void reset();
};
