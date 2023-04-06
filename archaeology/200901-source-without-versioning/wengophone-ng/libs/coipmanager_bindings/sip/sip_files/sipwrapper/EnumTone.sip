class EnumTone : NonCopyable {
%TypeHeaderCode
#include <sipwrapper/EnumTone.h>
%End
public:
	enum Tone {
		Tone0,
		Tone1,
		Tone2,
		Tone3,
		Tone4,
		Tone5,
		Tone6,
		Tone7,
		Tone8,
		Tone9,
		ToneStar,
		TonePound,
		ToneDialtone,
		ToneBusy,
		ToneRingback,
		ToneRingtone,
		ToneCallFailed,
		ToneSilence,
		ToneBackspace,
		ToneCallWaiting,
		ToneCallHeld,
		ToneLoudFastBusy
	};
};
