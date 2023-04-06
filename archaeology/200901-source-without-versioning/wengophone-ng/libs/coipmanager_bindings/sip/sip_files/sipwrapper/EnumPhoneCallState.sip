class EnumPhoneCallState : NonCopyable {
%TypeHeaderCode
#include <sipwrapper/EnumPhoneCallState.h>
%End
public:
	enum PhoneCallState {
		PhoneCallStateUnknown,
		PhoneCallStateError,
		PhoneCallStateResumed,
		PhoneCallStateTalking,
		PhoneCallStateDialing,
		PhoneCallStateRinging,
		PhoneCallStateClosed,
		PhoneCallStateIncoming,
		PhoneCallStateHold,
		PhoneCallStateMissed,
		PhoneCallStateRedirected,
	};
 static std::string toString(PhoneCallState state);
};
