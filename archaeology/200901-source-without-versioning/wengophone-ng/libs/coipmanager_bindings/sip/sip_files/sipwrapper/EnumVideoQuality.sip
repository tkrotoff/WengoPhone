class EnumVideoQuality : NonCopyable {
%TypeHeaderCode
#include <sipwrapper/EnumVideoQuality.h>
%End
public:
	enum VideoQuality {
		VideoQualityNormal,
		VideoQualityGood,
		VideoQualityVeryGood,
		VideoQualityExcellent
	};
 static std::string toString(VideoQuality videoQuality);
 static VideoQuality toVideoQuality(const std::string & videoQuality);
};
