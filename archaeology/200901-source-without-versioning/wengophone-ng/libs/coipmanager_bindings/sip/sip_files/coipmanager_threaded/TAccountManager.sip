class TAccountManager : QObject {

%TypeHeaderCode
#include <coipmanager_threaded/datamanager/TAccountManager.h>
%End

public:

	TAccountManager(TCoIpManager & tCoIpManager);

	~TAccountManager();

	/**
	 * @see AccountManager::add
	 */
	bool add(const Account & account);

	/**
	 * @see AccountManager::remove
	 */
	bool remove(const std::string & accountId);

	/**
	 * @see AccountManager::update
	 */
	bool update(const Account & account,
		EnumUpdateSection::UpdateSection section = EnumUpdateSection::UpdateSectionUnknown);

signals:

	/**
	 * @see AccountManager::accountAddedSignal
	 */
	void accountAddedSignal(std::string accountId);

	/**
	 * @see AccountManager::accountRemovedSignal
	 */
	void accountRemovedSignal(std::string accountId);

	/**
	 * @see AccountManager::accountUpdatedSignal
	 */
	void accountUpdatedSignal(std::string accountId, EnumUpdateSection::UpdateSection section);

};
