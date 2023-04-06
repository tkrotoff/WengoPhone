class JabberAccount : IAccount {
%TypeHeaderCode
#include <coipmanager_base/account/JabberAccount.h>
%End
public:
	static const std::string DEFAULT_JABBER_SERVER;
	static const int DEFAULT_JABBER_PORT;
	static const std::string DEFAULT_JABBER_RESOURCE;
	static const bool DEFAULT_JABBER_USE_TLS;
	static const bool DEFAULT_JABBER_REQUIRE_TLS;
	static const bool DEFAULT_JABBER_USE_OLD_SSL;
	static const bool DEFAULT_JABBER_AUTH_PLAIN_IN_CLEAR;
	static const std::string DEFAULT_JABBER_CONNECTION_SERVER;
	JabberAccount();
	JabberAccount(const std::string & login, const std::string & password);
	JabberAccount(const JabberAccount & account);
	explicit JabberAccount(const IAccount * iAccount);
	virtual ~JabberAccount();
	virtual JabberAccount * clone() const /Factory/;
	void setServer(const std::string & jabberServer);
	std::string getServer() const;
	void setServerPort(int jabberServerPort);
	int getServerPort() const;
	void setResource(const std::string & resource);
	std::string getResource() const;
	void setTLS(bool useTLS);
	bool isTLSUsed() const;
	void setTLSRequired(bool requireTLS);
	bool isTLSRequired() const;
	void setOldSSLUsed(bool oldSSLUsed);
	bool isOldSSLUsed() const;
	void setAuthPlainInClearUsed(bool useAuthPlainInClear);
	bool isAuthPlainInClearUsed() const;
	void setConnectionServer(const std::string & connectionServer);
	std::string getConnectionServer() const;
protected:
	void copy(const JabberAccount & account);
};
