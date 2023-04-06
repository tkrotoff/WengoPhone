class SipAccount : IAccount {
%TypeHeaderCode
#include <coipmanager_base/account/SipAccount.h>
%End
public:
	SipAccount();
	SipAccount(const std::string & identity, const std::string & password);
	SipAccount(const SipAccount & sipAccount);
	SipAccount(const IAccount * iAccount);
	virtual SipAccount * clone() const /Factory/;
	virtual ~SipAccount();
	void setRegisterServerHostname(const std::string & server);
	const std::string & getRegisterServerHostname() const;
	void setRegisterServerPort(unsigned port);
	unsigned getRegisterServerPort() const;
	void setUseSIPProxyServer(bool useSipProxy);
	bool useSIPProxyServer() const;
	void setSIPProxyServerHostname(const std::string & server);
	const std::string & getSIPProxyServerHostname() const;
	void setSIPProxyServerPort(unsigned port);
	unsigned getSIPProxyServerPort() const;
	void setHttpTunnelSupported(bool supported);
	bool isHttpTunnelSupported() const;
	void setHttpTunnelServerHostname(const std::string & server);
	const std::string & getHttpTunnelServerHostname() const;
	void setHttpsTunnelServerHostname(const std::string & server);
	const std::string & getHttpsTunnelServerHostname() const;
	void setHttpTunnelServerPort(unsigned port);
	unsigned getHttpTunnelServerPort() const;
	void setUseSSLForHttpTunnel(bool useSSL);
	bool useSSLForHttpTunnel() const;
	void setHttpTunnelNeeded(bool needed);
	bool isHttpTunnelNeeded() const;
	void setVirtualLineId(int lineId);
	int getVirtualLineId() const;
	void setNatType(EnumNatType::NatType natType);
	EnumNatType::NatType getNatType() const;
	//Inherited from SipPeer
	void setRealm(const std::string & realm);
	std::string getRealm() const;
	void setDisplayName(const std::string & displayName);
	std::string getDisplayName() const;

protected:
	void copy(const SipAccount & sipAccount);
};
