class EnumProxyAuthType : NonCopyable {
%TypeHeaderCode
#include <networkdiscovery/EnumProxyAuthType.h>
%End
public:
	enum ProxyAuthType {
		ProxyAuthTypeUnknown,
		ProxyAuthTypeBasic,
		ProxyAuthTypeDigest,
		ProxyAuthTypeNTLM,
	};
 static std::string toString(ProxyAuthType proxyAuthType);
 static ProxyAuthType toProxyAuthType(const std::string & authType);
 // EnumAuthType: defined in netlib.h
 //static ProxyAuthType toProxyAuthType(const EnumAuthType & authType);
};
