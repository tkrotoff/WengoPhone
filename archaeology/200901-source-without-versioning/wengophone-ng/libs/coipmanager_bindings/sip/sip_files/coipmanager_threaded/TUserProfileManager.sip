class TUserProfileManager : QObject {
%TypeHeaderCode
#include <coipmanager_threaded/datamanager/TUserProfileManager.h>
%End
public:
	TUserProfileManager(TCoIpManager & tCoIpManager);
	~TUserProfileManager();
	TAccountManager & getTAccountManager();
	TContactManager & getTContactManager();
	UserProfile getCopyOfUserProfile();
	void setUserProfile(const UserProfile & userProfile);
	void updateUserProfile(const UserProfile & userProfile);
	void logOff();
signals:
	void userProfileSetSignal(UserProfile userProfile);
	void userProfileUpdatedSignal(UserProfile userProfile);
	void userProfileLoggedOffSignal(UserProfile userProfile);
};
