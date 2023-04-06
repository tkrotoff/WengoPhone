class OscarAccount : IAccount {
%TypeHeaderCode
#include <coipmanager_base/account/OscarAccount.h>
%End
public:
	static const std::string DEFAULT_OSCAR_SERVER;
	static const int DEFAULT_OSCAR_PORT;
	static const std::string DEFAULT_ENCODING;
	OscarAccount();
	OscarAccount(const std::string & login, const std::string & password);
	OscarAccount(const OscarAccount & account);
	explicit OscarAccount(const IAccount * iAccount);
	virtual ~OscarAccount();
	virtual OscarAccount * clone() const;
	void setServer(const std::string & oscarServer);
	std::string getServer() const;
	void setServerPort(int oscarServerPort);
	int getServerPort() const;
	void setEncoding(const std::string & encoding);
	std::string getEncoding() const;
protected:
	void copy(const OscarAccount & account);
};
