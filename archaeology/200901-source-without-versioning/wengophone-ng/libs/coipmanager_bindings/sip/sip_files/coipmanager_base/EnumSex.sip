class EnumSex : NonCopyable {
%TypeHeaderCode
#include <coipmanager_base/profile/EnumSex.h>
%End
public:
	enum Sex {
		SexUnknown,
		SexMale,
		SexFemale
	};
	static std::string toString(Sex sex);
	static Sex toSex(const std::string & sex);
};
