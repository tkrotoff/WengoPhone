class UserProfileFileStorage {
%TypeHeaderCode
#include <coipmanager_base/storage/UserProfileFileStorage.h>
%End
public:
	enum UserProfileStorageError {
		UserProfileStorageErrorNoError,
		UserProfileStorageErrorDoesNotExist,
		UserProfileStorageErrorLoadedFromBackup,
		UserProfileStorageErrorCannotBeLoaded,
	};
	static std::string getProfilePath(const std::string & profileId);
	UserProfileFileStorage(const std::string & pathToProfiles);
	~UserProfileFileStorage();
	StringList getUserProfileIds() const;
	std::string getNameOfUserProfile(const std::string & profileId) const;
	UserProfileStorageError load(const std::string & profileId, UserProfile & userProfile);
	bool save(UserProfile & userProfile);
private:
	static std::string getTempProfilePath(const std::string & profileId);
	static std::string getBackupProfilePath(const std::string & profileId);
	bool loadFromProfiles(const std::string & profileId, UserProfile & userProfile);
	bool loadFromBackups(const std::string & profileId, UserProfile & userProfile);
	bool loadFromDir(const std::string & path, UserProfile & userProfile);
	bool loadProfile(const std::string & url, UserProfile & userProfile);
	bool saveProfile(const std::string & url, const UserProfile & userProfile);
};
