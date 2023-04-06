class NetworkProxy {
%TypeHeaderCode
#include <networkdiscovery/NetworkProxyDiscovery.h>
%End
public:
 NetworkProxy();
 NetworkProxy(const NetworkProxy & networkProxy);
 std::string getLogin() const;
 void setLogin(const std::string & login);
 std::string getPassword() const;
 void setPassword(const std::string & password);
 std::string getServer() const;
 void setServer(const std::string & server);
 int getServerPort() const;
 void setServerPort(int port);
 EnumProxyAuthType::ProxyAuthType getProxyAuthType() const;
 void setProxyAuthType(EnumProxyAuthType::ProxyAuthType proxyAuthType);
};
