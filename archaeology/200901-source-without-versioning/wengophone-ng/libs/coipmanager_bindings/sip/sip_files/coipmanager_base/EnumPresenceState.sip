class EnumPresenceState : NonCopyable {
%TypeHeaderCode
#include <coipmanager_base/EnumPresenceState.h>
%End
public:
	enum PresenceState {
		PresenceStateOnline,
		PresenceStateOffline,
		PresenceStateInvisible,
		PresenceStateAway,
		PresenceStateDoNotDisturb,
		PresenceStateUserDefined,
		PresenceStateUnknown,
		PresenceStateUnavailable
	};
	enum MyPresenceStatus {
		MyPresenceStatusOk,
		MyPresenceStatusError
	};
	static std::string toString(PresenceState presenceState);
	static PresenceState fromString(const std::string & presenceState);
};
