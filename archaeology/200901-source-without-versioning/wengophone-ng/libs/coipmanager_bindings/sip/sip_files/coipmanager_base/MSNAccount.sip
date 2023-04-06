class MSNAccount : IAccount {
%TypeHeaderCode
#include <coipmanager_base/account/MSNAccount.h>
%End
public:
	static const std::string DEFAULT_MSN_SERVER;
	static const int DEFAULT_MSN_PORT;
	MSNAccount();
	MSNAccount(const std::string & login, const std::string & password);
	MSNAccount(const MSNAccount & account);
	explicit MSNAccount(const IAccount * iAccount);
	virtual ~MSNAccount();
	virtual MSNAccount * clone() const;
	void setMailNotification(bool mailNotification);
	bool isMailNotified() const;
	void setServer(const std::string & msnServer);
	std::string getServer() const;
	void setServerPort(int msnServerPort);
	int getServerPort() const;
	void setHttpConnection(bool useHttp);
	bool isHttpUsed() const;
protected:
	void copy(const MSNAccount & account);
};
