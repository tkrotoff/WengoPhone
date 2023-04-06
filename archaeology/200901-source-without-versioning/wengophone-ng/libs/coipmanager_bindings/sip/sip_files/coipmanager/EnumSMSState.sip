class EnumSMSState /NoDefaultCtors/ {
%TypeHeaderCode
#include <coipmanager/smssessionmanager/EnumSMSState.h>
%End
public:
	enum SMSState {
		/** The SMS has not been sent. */
		SMSStateError,

		/** The SMS has been sent. */
		SMSStateOk
	};

private:

	EnumSMSState();

};
