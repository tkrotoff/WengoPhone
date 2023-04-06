class EnumConferenceCallState : NonCopyable {
%TypeHeaderCode
#include <sipwrapper/EnumConferenceCallState.h>
%End
public:
	enum ConferenceCallState {
		ConferenceCallStateUnknown,
		ConferenceCallStateError,
		ConferenceCallStateStarted,
		ConferenceCallStateStopped,
	};
};
