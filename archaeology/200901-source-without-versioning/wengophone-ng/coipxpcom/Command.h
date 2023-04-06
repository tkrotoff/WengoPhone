
#ifndef COMMAND_H
#define COMMAND_H

//#include "Listener.h"
#include "ICommandObserver.h"
#include "EnumConnectionError.h"


//#include <coipmanager/chatsessionmanager/ChatSessionManager.h>

#include <coipmanager/EnumProtocol.h>
#include <coipmanager/userprofile/UserProfile.h>

#include "dllexport.h"

#include <event/Trackable2.h>

#include <string>
#include <map>

class TCoIpManager;
class TConnectManager;
class TChatSessionManager;
class TChatSession;
class TUserProfileManager;

class DLLEXPORT Command : Trackable2 {
public:

	Command(ICommandObserver *parent);
	~Command();

	// interface implementation
	void Init();
	void ConnectAccount(const std::string & userLogin,
			    const std::string & userPassword,
			    EnumProtocol::Protocol protocol,
			    bool isSavePassword);
	void CreateAndSetUserProfile(const std::string & userLogin,
				     const std::string & userPassword,
				     EnumProtocol::Protocol protocol,
				     bool isSavePassword);
	void SetUserProfile(const std::string & UUID,
			    const std::string & userPassword,
			    bool isSavePassword);
	void AccountLogOff();
 	void CreateChatSession(unsigned chatSessionID);
 	void AddContact2ChatSession(unsigned sessionID,
				    const std::string & email,
				    EnumProtocol::Protocol protocol);

 	void GetAccountList();

private:

	// events
	void accountConnectedEventHandler(TConnectManager * sender, std::string accountId);
	void accountDisconnectedEventHandler(TConnectManager * sender, std::string accountId);
	void accountConnectionErrorEventHandler(TConnectManager * sender, std::string accountId,
					 EnumConnectionError::ConnectionError errorCode);
	void accountConnectionProgressEventHandler(TConnectManager * sender, std::string accountId,
					    unsigned currentStep, unsigned totalSteps, std::string infoMessage);
	void networkDetectionProgressEventHandler(TConnectManager * sender, std::string accountId,
					   unsigned currentStep, unsigned totalSteps, std::string infoMessage);
	void messageAddedEventHandler(std::string message);
	void newChatSessionCreatedEventHandler(TChatSessionManager * sender,
					       TChatSession * chatSession);
	void userProfileLoggedOffEventHandler(TUserProfileManager * sender,
					      UserProfile userProfile);
	void userProfileSetEventHandler(TUserProfileManager * sender,
					UserProfile userProfile);

	// local

	// attributs
	TCoIpManager * _tCoIpManager;

	// parent (IListener)
	ICommandObserver * _parent;

	// internal containers
	std::map<unsigned, TChatSession *> _chatSessions;
	//std::map<std::string, Account *> _accounts;

};

#endif	//COMMAND_H
