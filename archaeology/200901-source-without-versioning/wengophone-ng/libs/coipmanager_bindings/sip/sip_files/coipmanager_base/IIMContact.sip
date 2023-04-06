class IIMContact : Peer {
%TypeHeaderCode
#include <coipmanager_base/imcontact/IIMContact.h>
%End
public:
	IIMContact();
	IIMContact(EnumAccountType::AccountType accountType, const std::string & contactId);
	IIMContact(const IIMContact & imContact);
	virtual IIMContact * clone() const /Factory/;
	virtual ~IIMContact();
	virtual void setPeerId(const std::string & peerId);
	virtual std::string getPeerId() const;
	virtual void setAccountId(const std::string & accountId);
	virtual std::string getAccountId() const;
	virtual void setContactId(const std::string & contactId);
	virtual std::string getContactId() const;
	virtual void setPresenceState(EnumPresenceState::PresenceState presenceState);
	virtual void setAccountType(EnumAccountType::AccountType accountType);
	void addToGroup(const std::string & group);
	void removeFromGroup(const std::string & group);
	StringList getGroupList() const;
protected:
	void copy(const IIMContact & imContact);
};
