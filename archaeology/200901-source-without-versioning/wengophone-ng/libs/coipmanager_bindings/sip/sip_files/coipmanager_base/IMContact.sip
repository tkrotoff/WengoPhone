class IMContact : IIMContact {
%TypeHeaderCode
#include <coipmanager_base/imcontact/IMContact.h>
%End
public:
	IMContact();
	IMContact(EnumAccountType::AccountType accountType, const std::string & contactId);
	IMContact(const IMContact & imContact);
	explicit IMContact(const IIMContact * iIMContact);
	virtual IMContact * clone() const /Factory/;
	~IMContact();
	virtual void setAccountId(const std::string & accountId);
	virtual std::string getAccountId() const;
	virtual std::string getContactId() const;
	virtual EnumAccountType::AccountType getAccountType() const;
	virtual void setAlias(const std::string & alias);
	virtual std::string getAlias() const;
	virtual void setPresenceState(EnumPresenceState::PresenceState presenceState);
	virtual EnumPresenceState::PresenceState getPresenceState() const;
	virtual void setIcon(const OWPicture & icon);
	virtual OWPicture getIcon() const;
	IIMContact * getPrivateIMContact() const;
private:
	void copy(const IMContact & imContact);
	virtual void setContactId(const std::string & contactId);
	virtual void setPeerId(const std::string & peerId);
	virtual std::string getPeerId() const;
	virtual void setAccountType(EnumAccountType::AccountType accountType);
};
