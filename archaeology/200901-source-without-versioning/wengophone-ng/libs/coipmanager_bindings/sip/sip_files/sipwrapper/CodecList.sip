class CodecList : NonCopyable {
%TypeHeaderCode
#include <sipwrapper/CodecList.h>
%End
public:
	enum AudioCodec {
		AudioCodecError,
		AudioCodecPCMU,
		AudioCodecPCMA,
		AudioCodecILBC,
		AudioCodecGSM,
		AudioCodecAMRNB,
		AudioCodecAMRWB,
		AudioCodecSPEEXNB,
		AudioCodecSPEEXWB
	};
	enum VideoCodec {
		VideoCodecError,
		VideoCodecH263,
		VideoCodecH264,
		VideoCodecMPEG4
	};
 static std::string toString(AudioCodec audioCodec);
 static std::string toString(VideoCodec videoCodec);
};
