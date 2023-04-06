
#include "CCommand.h"

#include "nsGUIEvent.h"

#include <nsIObserverService.h>
#include <nsIProxyObjectManager.h>

NS_IMPL_ISUPPORTS1(CCommand, ICommand)

CCommand::CCommand()
{
  _command = new Command(this);
  _listener = NULL;
  _configurator = NULL;
}

CCommand::~CCommand()
{
  if (_command) {
    delete _command;
  }
}

NS_IMETHODIMP CCommand::Init()
{
  if (_command) {
    _command->Init();
  }
  return NS_OK;
}

NS_IMETHODIMP CCommand::GetAccountList()
{
  if (_command) {
    _command->GetAccountList();
  }
  return NS_OK;
}

NS_IMETHODIMP CCommand::SetListener(IListener *listener)
{
  _listener = listener;

  NS_ADDREF(_listener);

  if (Proxyfy()) {
    _listener = _listenerProxy;
  }
  else {
    _listener = NULL;
  }

  return NS_OK;
}

NS_IMETHODIMP CCommand::SetConfigurator(IConfigurator *configurator)
{
  _configurator = configurator;
  NS_ADDREF(_configurator);

  return NS_OK;
}

NS_IMETHODIMP CCommand::SetUserProfile(const nsAString & aUUID,
				       const nsAString & aUserPassword,
				       PRBool savePassword)
{
  if (_command) {
    _command->SetUserProfile(NS_ConvertUTF16toUTF8(aUUID).get(),
			     NS_ConvertUTF16toUTF8(aUserPassword).get(),
			     (savePassword) ? true : false);
  }
  return NS_OK;
}


NS_IMETHODIMP CCommand::CreateAndSetUserProfile(const nsAString & aUserLogin,
						const nsAString & aUserPassword,
						PRInt16 aProtocol,
						PRBool savePassword)
{
  if (_command) {
    _command->CreateAndSetUserProfile(NS_ConvertUTF16toUTF8(aUserLogin).get(),
				      NS_ConvertUTF16toUTF8(aUserPassword).get(),
				      GetProtocol(aProtocol),
				      (savePassword) ? true : false);
  }
  return NS_OK;
}

NS_IMETHODIMP CCommand::ConnectAccount(const nsAString & aUserLogin,
				       const nsAString & aUserPassword,
				       PRInt16 aProtocol,
				       PRBool savePassword)
{
  if (_command) {
    _command->ConnectAccount(NS_ConvertUTF16toUTF8(aUserLogin).get(),
			     NS_ConvertUTF16toUTF8(aUserPassword).get(),
			     GetProtocol(aProtocol),
			     (savePassword) ? true : false);
  }
  return NS_OK;
}

NS_IMETHODIMP CCommand::AccountLogOff()
{
  if (_command) {
    _command->AccountLogOff();
  }
  return NS_OK;
}

// NS_IMETHODIMP CCommand::CreateChatSession(PRUint16 aSessionID)
// {
//   if (_command) {
//     _command->CreateChatSession(aSessionID);
//   }

//   return NS_OK;
// }

// NS_IMETHODIMP CCommand::ChatSession_start(PRUint16 aSessionID)
// {
//   if (_command) {
//     ;
//   }
//   return NS_OK;
// }

// NS_IMETHODIMP CCommand::ChatSession_getMessageHistory(PRUint16 aSessionID)
// {
//   if (_command) {
//     ;
//   }
//   return NS_OK;
// }

// NS_IMETHODIMP CCommand::ChatSession_sendMessage(PRUint16 aSessionID,
// 						const nsAString & aMessage)
// {
//   if (_command) {
//     ;
//   }
//   return NS_OK;
// }

// NS_IMETHODIMP CCommand::ChatSession_addContact(PRUint16 aSessionID,
// 					       const nsAString & aContactID,
// 					       PRInt16 aProtocol)
// {
//   if (_command) {

//     // TODO : is it mandatory ?
//     unsigned SessionID = aSessionID;

// //     _command->AddContact2ChatSession(SessionID,
// // 				     NS_ConvertUTF16toUTF8(aContactID).get(),
// // 				     GetProtocol(aProtocol));
//   }
//   return NS_OK;
// }

// NS_IMETHODIMP CCommand::ChatSession_removeContact(PRUint16 aSessionID,
// 						const nsAString & aContactUUID)
// {
//   if (_command) {
//     ;
//   }
//   return NS_OK;
// }

/*
** Private implentation
*/

EnumProtocol::Protocol CCommand::GetProtocol(PRInt16 aProtocol) 
{
  EnumProtocol::Protocol protocol;

  switch (aProtocol)
    {
    case ICommand::PROTOCOL_MSN:
      protocol = EnumProtocol::ProtocolMSN;
      DebugMessage("** internal ** use MSN protocol");
      break;
    case ICommand::PROTOCOL_Yahoo:
      DebugMessage("** internal ** use Yahoo protocol");
      protocol = EnumProtocol::ProtocolYahoo;
      break;
//     case ICommand::PROTOCOL_AIMICQ:
//       DebugMessage("** internal ** use AIM ICQ protocol");
//       protocol = EnumProtocol::ProtocolAIMICQ;
//       break;
    case ICommand::PROTOCOL_Jabber:
      DebugMessage("** internal ** use Jabber protocol");
      protocol = EnumProtocol::ProtocolJabber;
      break;
    case ICommand::PROTOCOL_SIP:
      DebugMessage("** internal ** use SIP protocol");
      protocol = EnumProtocol::ProtocolSIP;
      break;
    case ICommand::PROTOCOL_Wengo:
      DebugMessage("** internal ** use Wengo protocol");
      protocol = EnumProtocol::ProtocolWengo;
      break;
    case ICommand::PROTOCOL_IAX:
      DebugMessage("** internal ** use IAX protocol");
      protocol = EnumProtocol::ProtocolIAX;
      break;
    default:
      DebugMessage("** internal ** use unknown protocol");
      protocol = EnumProtocol::ProtocolUnknown;
      break;
    }
  return protocol;
}

void CCommand::DebugMessage(const std::string & message) {
  
  nsCString string;
  string.Append(message.c_str());

  _listener->DebugMessage(NS_ConvertUTF8toUTF16(string));
}

void CCommand::SendFeedback(unsigned sessionID,
			    const std::string & feed,
			    const std::string & param) {
  
  nsCString str_feed;
  str_feed.Append(feed.c_str());
  nsCString str_param;
  str_param.Append(param.c_str());
  
  _listener->Feedback(sessionID,
		      NS_ConvertUTF8toUTF16(str_feed),
		      NS_ConvertUTF8toUTF16(str_param));
}

char *CCommand::ConfGetChar(const std::string & prefID) {
  
  nsCString string1;
  string1.Append(prefID.c_str());

  char **retval;
  _configurator->GetChar(NS_ConvertUTF8toUTF16(string1),
			 retval);
  return *retval;
}

void CCommand::ConfSetChar(const std::string & prefID,
			   const std::string & value) {
  
  nsCString string1;
  string1.Append(prefID.c_str());
  nsCString string2;
  string2.Append(value.c_str());

  _configurator->SetChar(NS_ConvertUTF8toUTF16(string1),
			 NS_ConvertUTF8toUTF16(string2));
}

bool CCommand::Proxyfy() {

  nsresult rv;

  // Récupère le proxy manager sous forme de service
//   nsComPtr<nsIPproxyObjectManager> proxyObjMgr =
//     do_GetService("@mozilla.org/xpcomproxy;1", &rv);
  nsCOMPtr<nsIProxyObjectManager> proxyObjMgr = 
    do_GetService("@mozilla.org/xpcomproxy;1", &rv);
  
  if (NS_FAILED(rv))
    return false;

  rv = proxyObjMgr->GetProxyForObject(NS_CURRENT_EVENTQ,
				      IListener::GetIID(),
				      _listener,
				      PROXY_ASYNC|PROXY_ALWAYS, //PROXY_SYNC|PROXY_ALWAYS,
				      getter_AddRefs(_listenerProxy));

  if (NS_FAILED(rv))
    return false;
  
  return true;
}
