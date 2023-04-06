class TContactManager : QObject {
%TypeHeaderCode
#include <coipmanager_threaded/datamanager/TContactManager.h>
%End
public:
	TContactManager(TCoIpManager & tCoIpManager);
	~TContactManager();
	void add(const Contact & contact);
	void remove(const std::string & contactId);
	void update(const Contact & contact,
		EnumUpdateSection::UpdateSection section = EnumUpdateSection::UpdateSectionUnknown);
signals:
	void contactAddedSignal(std::string contactId);
	void contactRemovedSignal(std::string contactId);
	void contactUpdatedSignal(std::string contactId, EnumUpdateSection::UpdateSection section);
};
