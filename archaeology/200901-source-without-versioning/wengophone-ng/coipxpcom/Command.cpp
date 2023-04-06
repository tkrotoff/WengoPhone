
#include "Command.h"

#include <coipmanager_threaded/TCoIpManager.h>
#include <coipmanager/account/WengoAccount.h>
#include <coipmanager_threaded/connectmanager/TConnectManager.h>
#include <coipmanager_threaded/datamanager/TUserProfileManager.h>
#include <coipmanager_threaded/chatsessionmanager/TChatSession.h>

#include <coipmanager/contact/Contact.h>
#include <coipmanager/imcontact/IMContact.h>
#include <coipmanager/storage/UserProfileFileStorage.h>

#include <util/Date.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <util/Time.h>
#include <util/String.h>
#include <util/Path.h>

#include <iostream>

Command::Command(ICommandObserver *parent) {

  _parent = parent;
}

Command::~Command() {

  // detroy all chat sessions
//   std::map<unsigned, TChatSession *>::iterator itc = _chatSessions.begin();
//   while (itc != _chatSessions.end())
//     {
//       _parent->DebugMessage("chatSessions [" + String::fromNumber(itc->first) + "]");
//       //_accounts.erase(it->first);
//       ++itc;
//     }

  Logger::logger.messageAddedEvent.disconnect(this,
	boost::bind(&Command::messageAddedEventHandler, this, _1),
	NULL);

  OWSAFE_DELETE(_tCoIpManager);
}

void Command::GetAccountList() {

  CoIpManagerConfig coIpManagerConfig = _tCoIpManager->getCoIpManagerConfig();

  UserProfileFileStorage fileStorage(coIpManagerConfig.getConfigPath());
  StringList userProfileIds = fileStorage.getUserProfileIds();
 
  for (StringList::const_iterator it = userProfileIds.begin();
       it != userProfileIds.end();
       ++it) {

    UserProfile userProfile;
    std::string profileID = *it;
    fileStorage.load(profileID, userProfile);

    AccountList accountList = userProfile.getAccountList();
    for (AccountList::const_iterator it2 = accountList.begin();
	 it2 != accountList.end();
	 ++it2) {
      
      Account account = *it2;
      IAccount *iaccount = account.getPrivateAccount();
      WengoAccount *wengoAccount = (WengoAccount *)iaccount;

      std::string concat =  profileID + "|" + wengoAccount->getWengoLogin() + 
	"|" + wengoAccount->getWengoPassword() + "|" + 
	(wengoAccount->isPasswordSaved() ? "true" : "false");
      _parent->SendFeedback(0, "gui account", concat);
    }
  }

  _parent->SendFeedback(0, "gui open login", "");
}

void Command::Init() {

  // Creating CoIpManager
  CoIpManagerConfig coIpManagerConfig;
  //coIpManagerConfig.set(CoIpManagerConfig::SAVE_ON_QUIT_KEY, true);
  coIpManagerConfig.set(CoIpManagerConfig::CONFIG_PATH_KEY, 
			Path::getConfigurationDirPath() + "wengo.com/");
  // C:\Documents and Settings\[user]\Application Data\wengo.com

  _tCoIpManager = new TCoIpManager(coIpManagerConfig);
  _tCoIpManager->start();
  _tCoIpManager->waitForReady();

  _tCoIpManager->getTConnectManager().accountConnectedEvent.connect(this,
								  boost::bind(&Command::accountConnectedEventHandler, this, _1, _2),
								  NULL);
  _tCoIpManager->getTConnectManager().accountDisconnectedEvent.connect(this,
								     boost::bind(&Command::accountDisconnectedEventHandler, this, _1, _2),
								     NULL);
  _tCoIpManager->getTConnectManager().accountConnectionErrorEvent.connect(this,
									boost::bind(&Command::accountConnectionErrorEventHandler, this, _1, _2, _3),
									NULL);
  _tCoIpManager->getTConnectManager().accountConnectionProgressEvent.connect(this,
									   boost::bind(&Command::accountConnectionProgressEventHandler, this, _1, _2, _3, _4, _5),
									   NULL);
  _tCoIpManager->getTConnectManager().networkDetectionProgressEvent.connect(this,
									  boost::bind(&Command::networkDetectionProgressEventHandler, this, _1, _2, _3, _4, _5),
									  NULL);
  _tCoIpManager->getTUserProfileManager().userProfileLoggedOffEvent.connect(this,
    boost::bind(&Command::userProfileLoggedOffEventHandler, this, _1, _2),
    NULL);
  _tCoIpManager->getTUserProfileManager().userProfileSetEvent.connect(this,
    boost::bind(&Command::userProfileSetEventHandler, this, _1, _2),
    NULL);
  
  Logger::logger.messageAddedEvent.connect(this,
					   boost::bind(&Command::messageAddedEventHandler, this, _1),
					   NULL);
  //// chat sessions
//   _coIpManager->getChatSessionManager().newChatSessionCreatedEvent +=
//     boost::bind(&Command::newChatSessionCreatedEventHandler, this, _1, _2);
}

void Command::SetUserProfile(const std::string & UUID,
			     const std::string & userPassword,
			     bool isSavePassword) {
  
  // _parent->ConfGetChar("wengo.profileid")

  UserProfile userProfile;
  CoIpManagerConfig coIpManagerConfig = _tCoIpManager->getCoIpManagerConfig();

  if (!UUID.empty()) {
    UserProfileFileStorage fileStorage(coIpManagerConfig.getConfigPath());
    if (!fileStorage.load(UUID, userProfile) == UserProfileFileStorage::UserProfileStorageErrorNoError) {
      LOG_ERROR("cannot load profile");
    }
  }
  
  AccountList accountList = userProfile.getAccountList();
  if (accountList.size() > 0) {

    // get first
    Account & account = *accountList.begin();
	WengoAccount *wengoAccount = static_cast<WengoAccount *>(account.getPrivateAccount());
	wengoAccount->setWengoPassword(userPassword);
    account.setSavePassword(isSavePassword);
  }
  else {
    LOG_ERROR("no account in profile");
  }

  _tCoIpManager->getTUserProfileManager().setUserProfile(userProfile);
}

void Command::CreateAndSetUserProfile(const std::string & userLogin,
				      const std::string & userPassword,
				      EnumProtocol::Protocol protocol,
				      bool isSavePassword) {

  CoIpManagerConfig coIpManagerConfig = _tCoIpManager->getCoIpManagerConfig();

  UserProfile userProfile;

  Account account(userLogin, userPassword, protocol);
  account.setSavePassword(isSavePassword);

  //_tCoIpManager->getTUserProfileManager().getTAccountManager().add(account);

  userProfile.getAccountList().push_back(account);

  _tCoIpManager->getTUserProfileManager().setUserProfile(userProfile);
  //SetUserProfile(userProfile.getUUID(), userPassword, isSavePassword);

  if (_tCoIpManager->getTConnectManager().checkValidity(account)) {

    // Todo : move this code
    //   UserProfileFileStorage fileStorage(coIpManagerConfig.getConfigPath());
    //   if (!fileStorage.save(userProfile)) {
    //       LOG_ERROR("cannot save profile");
    //   }
  }
  else {
    // logOff
    AccountLogOff();
  }
}

void Command::ConnectAccount(const std::string & userLogin,
			     const std::string & userPassword,
			     EnumProtocol::Protocol protocol,
			     bool isSavePassword) {
  
	Account *account = _tCoIpManager->getTUserProfileManager().getUserProfile().getAccountList().getAccount(userLogin, protocol);
	account->setSavePassword(isSavePassword);
	WengoAccount *wengoAccount = static_cast<WengoAccount *>(account->getPrivateAccount());
	wengoAccount->setWengoPassword(userPassword);
	_tCoIpManager->getTUserProfileManager().getTAccountManager().update(*account);
  
/*
  if (!account) {

    // should never enter here
    account = new Account(userLogin, userPassword, protocol);
    _tCoIpManager->getTUserProfileManager().getTAccountManager().add(*account);
  }
*/

  _tCoIpManager->getTConnectManager().connect(account->getUUID());

  delete account;

  //if (_coIpManager->getConnectManager().checkValidity(_account)) {

//   }
//   else {
//     _parent->logMessage("alert", "Invalid account (checkValidity returned false)");
//   }
}

void Command::AccountLogOff() {

  // assure that all the ressources are freed

  _tCoIpManager->getTUserProfileManager().logOff();
}

void Command::CreateChatSession(unsigned chatSessionID) {

//   // already exist
//   if (_chatSessions.at(chatSessionID)) {
//     _parent->SendFeedback(chatSessionID, "chatSession already exist", "");
//     return;
//   }
  
//   TChatSession *chatsession = _tCoIpManager->getChatSessionManager().createChatSession();
//   if (!chatsession) {
//     _parent->SendFeedback(sessionID, "chatSession error null", "");
//     return;
//   }

//   _chatSessions[sessionID] = chatsession;
//   _parent->SendFeedback(sessionID, "chatSession created sucessfully", "");
}

void Command::AddContact2ChatSession(unsigned sessionID,
				     const std::string & contactID,
				     EnumProtocol::Protocol protocol) {

//   Contact & contact = Contact();
//   IMContact imContact(protocol, contactID);

  //ChatSession *chatSession = _chatSessions.at(sessionID);

//   if (!chatSession) {
//     _parent->SendFeedback(sessionID, "contact error no chat session", "");
//     return;
//   }

  // should be removed
  //imContact.setAccountId(_account.getUUID());

//   contact.addIMContact(imContact);

//   _parent->DebugMessage("AddContact: IMContact[" + contactID + "] Protocol[" + String::fromNumber(protocol) + "] added");

  //_coIpManager.getConnectManager().connect(account->getUUID());

//   EnumSessionError::SessionError error = chatSession->addContact(contact);

//   switch (error) {
//   case EnumSessionError::SessionErrorNoError:
//     _parent->SendFeedback(sessionID, "contact added", contact.getUUID());
//     break;
//   case EnumSessionError::SessionErrorNoCommonAccountFound:
//     _parent->SendFeedback(sessionID, "contact error no common account found", "");
//     break;
//   default:
//     _parent->SendFeedback(sessionID, "contact error unknown", "");
//     break;
//   }

//   // TODO : do this in another function (within start, stop...)
//   chatSession->start();
}

// events
void Command::accountConnectedEventHandler(TConnectManager *sender, std::string accountId) {

  _parent->SendFeedback(0, "account connected", accountId);
}

void Command::accountDisconnectedEventHandler(TConnectManager *sender, std::string accountId) {

  _parent->SendFeedback(0, "account disconnected", accountId);
}

void Command::accountConnectionErrorEventHandler(TConnectManager *sender, std::string accountId,
	EnumConnectionError::ConnectionError errorCode) {
  
  switch (errorCode) {
    
  case EnumConnectionError::ConnectionErrorUnknown:
    //arent->SendFeedback(String(accountId).toInteger(), "account unknown error", NULL)s;
    _parent->SendFeedback(0, "account error unknown", accountId);
    break;
  case EnumConnectionError::ConnectionErrorServer:
    //_parent->SendFeedback(String(accountId).toInteger(), "account server error", NULL);
    _parent->SendFeedback(0, "account error server", accountId);
    break;
  case EnumConnectionError::ConnectionErrorTimeout:
    //_parent->SendFeedback(String(accountId).toInteger(), "account timeout error", NULL);
    _parent->SendFeedback(0, "account error timeout", accountId);
    break;
  case EnumConnectionError::ConnectionErrorInvalidLoginPassword:
    //_parent->SendFeedback(String(accountId).toInteger(), "account authentification error", NULL);
    _parent->SendFeedback(0, "account error authentification", accountId);
    break;
  default:
    //_parent->SendFeedback(String(accountId).toInteger(), "account error", NULL);
    _parent->SendFeedback(0, "account error default", accountId);
    break;
  }
}

void Command::accountConnectionProgressEventHandler(TConnectManager * sender, std::string accountId,
	unsigned currentStep, unsigned totalSteps, std::string infoMessage) {
  
  _parent->DebugMessage("Connection progress : " + infoMessage);
}

void Command::networkDetectionProgressEventHandler(TConnectManager * sender, std::string accountId,
	unsigned currentStep, unsigned totalSteps, std::string infoMessage) {

  _parent->DebugMessage("Network detection progress : " + infoMessage);
}

void Command::messageAddedEventHandler(std::string message) {

  _parent->DebugMessage(message);
}

void Command::userProfileLoggedOffEventHandler(TUserProfileManager * sender,
					       UserProfile userProfile) {

  _parent->DebugMessage("User Profile Logged off = " + userProfile.getName());
  _parent->SendFeedback(0, "account loggedoff", userProfile.getName());
}

void Command::userProfileSetEventHandler(TUserProfileManager * sender,
					 UserProfile userProfile) {
  
  _parent->DebugMessage("User Profile Set = " + userProfile.getName());
  _parent->SendFeedback(0, "account canconnect", userProfile.getName());
}

// void Command::newChatSessionCreatedEventHandler(ChatSessionManager * sender, ChatSession * chatSession) {

//   if (_chatSession) {
//     _parent->logMessage("alert", "You already have a chat session.");
//     return;
//   }
//   _parent->logMessage("alert", "chat session opened by another user");
//   _chatSession = chatSession;
// }
