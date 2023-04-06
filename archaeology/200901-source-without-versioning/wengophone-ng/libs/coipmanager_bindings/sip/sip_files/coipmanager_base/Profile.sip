class Profile : Identifiable {
%TypeHeaderCode
#include <coipmanager_base/profile/Profile.h>
%End
public:
	Profile();
	Profile(const Profile & profile);
	~Profile();
	void setFirstName(const std::string & firstName);
	const std::string & getFirstName() const;
	void setLastName(const std::string & lastName);
	const std::string & getLastName() const;
	std::string getCompleteName() const;
	void setSex(EnumSex::Sex sex);
	EnumSex::Sex getSex() const;
	void setBirthdate(const Date & birthdate);
	const Date & getBirthdate() const;
	void setWebsite(const std::string & website);
	const std::string & getWebsite() const;
	void setCompany(const std::string & company);
	const std::string & getCompany() const;
	void setMobilePhone(const std::string & mobilePhone);
	const std::string & getMobilePhone() const;
	void setHomePhone(const std::string & homePhone);
	const std::string & getHomePhone() const;
	void setWorkPhone(const std::string & workPhone);
	const std::string & getWorkPhone() const;
	void setOtherPhone(const std::string & otherPhone);
	const std::string & getOtherPhone() const;
	void setWengoPhoneNumber(const std::string & wengoPhoneNumber);
	const std::string & getWengoPhoneNumber() const;
	void setFax(const std::string & fax);
	const std::string & getFax() const;
	void setPersonalEmail(const std::string & personalEmail);
	const std::string & getPersonalEmail() const;
	void setWorkEmail(const std::string & workEmail);
	const std::string & getWorkEmail() const;
	void setOtherEmail(const std::string & otherEmail);
	const std::string & getOtherEmail() const;
	void setStreetAddress(const StreetAddress & streetAddress);
	StreetAddress getStreetAddress() const;
	void setNotes(const std::string & notes);
	const std::string & getNotes() const;
	void setSmsSignature(const std::string & signature);
	const std::string & getSmsSignature() const;
	bool hasPhoneNumber() const;
	void setAlias(const std::string & alias);
	const std::string & getAlias() const;
	void setWengoPhoneId(const std::string & wengoPhoneId);
	const std::string & getWengoPhoneId() const;
	void setIcon(const OWPicture & icon);
	const OWPicture & getIcon() const;
	void setPresenceState(EnumPresenceState::PresenceState state);
	EnumPresenceState::PresenceState getPresenceState() const;
protected:
	void copy(const Profile & profile);
};
