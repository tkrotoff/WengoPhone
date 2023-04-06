class Peer : Identifiable {
%TypeHeaderCode
#include <coipmanager_base/peer/Peer.h>
%End
public:
	Peer();
	Peer(EnumAccountType::AccountType accountType, const std::string & peerId);
	Peer(const Peer & peer);
	virtual Peer * clone() const /Factory/;
	virtual ~Peer();
	virtual void setPeerId(const std::string & peerId);
	virtual std::string getPeerId() const;
	virtual void setAccountType(EnumAccountType::AccountType accountType);
	virtual EnumAccountType::AccountType getAccountType() const;
	virtual void setPresenceState(EnumPresenceState::PresenceState presenceState);
	virtual EnumPresenceState::PresenceState getPresenceState() const;
	virtual void setAlias(const std::string & alias);
	virtual std::string getAlias() const;
	virtual void setConnectionState(EnumConnectionState::ConnectionState state);
	virtual EnumConnectionState::ConnectionState getConnectionState() const;
	virtual void setIcon(const OWPicture & icon);
	virtual OWPicture getIcon() const;
protected:
	void copy(const Peer & peer);
};
