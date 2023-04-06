class EnumUpdateSection : NonCopyable {
%TypeHeaderCode
#include <coipmanager_base/EnumUpdateSection.h>
%End
public:
	enum UpdateSection {
		UpdateSectionUnknown,
		UpdateSectionAlias,
		UpdateSectionIcon,
		UpdateSectionPresenceState,
		UpdateSectionGroup,
		UpdateSectionVolatileData,
	};
};
