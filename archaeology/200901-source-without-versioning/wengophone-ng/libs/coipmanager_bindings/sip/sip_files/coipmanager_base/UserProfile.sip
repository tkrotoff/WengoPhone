class UserProfile : Profile {
%TypeHeaderCode
#include <coipmanager_base/userprofile/UserProfile.h>
%End
public:
	UserProfile();
	UserProfile(const UserProfile & userProfile);
	~UserProfile();
	AccountList & getAccountList();
	ContactList & getContactList();
	ContactGroupList & getContactGroupList();
	void setName(const std::string & name);
	const std::string & getName() const;
	bool isEmpty() const;
private:
	void copy(const UserProfile & userProfile);
};
