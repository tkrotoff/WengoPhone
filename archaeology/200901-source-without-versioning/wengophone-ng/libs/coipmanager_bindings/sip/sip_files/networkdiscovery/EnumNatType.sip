class EnumNatType : NonCopyable {
%TypeHeaderCode
#include <networkdiscovery/EnumNatType.h>
%End
public:
	enum NatType {
		NatTypeUnknown,
		NatTypeOpen,
		NatTypeFullCone,
		NatTypeRestrictedCone,
		NatTypePortRestrictedCone,
		NatTypeSymmetric,
		NatTypeSymmetricFirewall,
		NatTypeBlocked,
		NatTypeFailure
	};
 static std::string toString(NatType natType);
 static NatType toNatType(const std::string & natType);
};
