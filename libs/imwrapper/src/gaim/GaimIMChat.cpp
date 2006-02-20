/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <global.h> 
#include <Logger.h>
#include <imwrapper/IMContact.h>
#include "GaimIMChat.h"
#include "GaimEnumIMProtocol.h"

extern "C" {
#include "gaim/conversation.h"
}

#ifdef OS_WIN32
#define snprintf _snprintf
#else
#include <stdio.h>
#endif

std::list<mConvInfo_t *> GaimIMChat::_GaimChatSessionList;

GaimIMChat::GaimIMChat(IMAccount & account)
	: IMChat(account), _account(account)
{

}

void GaimIMChat::createSession()
{
	mConvInfo_t	conv;
	IMChatSession *chatSession = new IMChatSession(*this);

	conv.conv_session = (void *) chatSession;
	conv.conv_id = chatSession->getId();
	conv.gaim_conv_session = NULL;

	AddChatSessionInList(&conv);
	newIMChatSessionCreatedEvent(*this, *chatSession);
}

void GaimIMChat::closeSession(IMChatSession & chatSession)
{
	if (FindChatStructById(chatSession.getId()) != NULL)
	{
		RemoveChatSessionFromList(chatSession.getId());
	}
}

void GaimIMChat::sendMessage(IMChatSession & chatSession, const std::string & message)
{
	mConvInfo_t *mConv = FindChatStructById(chatSession.getId());
	int BuddyNbr = chatSession.getIMContactList().size();
	
	if (BuddyNbr == 1)
	{
		gaim_conv_im_send_with_flags(GAIM_CONV_IM((GaimConversation *)mConv->gaim_conv_session), 
									message.c_str(), GAIM_MESSAGE_SEND);
	}
	else
	{
		gaim_conv_chat_send_with_flags(GAIM_CONV_CHAT((GaimConversation *)mConv->gaim_conv_session), 
									message.c_str(), GAIM_MESSAGE_SEND);
	}
}

void GaimIMChat::addContact(IMChatSession & chatSession, const std::string & contactId)
{
	mConvInfo_t *mConv = FindChatStructById(chatSession.getId());
	GaimConversation *conv;
	GaimAccount *Gaccount = gaim_accounts_find(_account.getLogin().c_str(),
											GaimEnumIMProtocol::GetPrclId(_account.getProtocol()));
	int BuddyNbr = chatSession.getIMContactList().size();
	
	if (Gaccount)
	{
		if (BuddyNbr == 0)
		{
			conv = gaim_conversation_new(GAIM_CONV_TYPE_IM, Gaccount, contactId.c_str());
			mConv->gaim_conv_session = conv;
			conv->ui_data = mConv;
		}
		else if (BuddyNbr == 1)
		{
			char ChatName[100];
			const IMChatSession::IMContactList &ChatContact = chatSession.getIMContactList();
			IMChatSession::IMContactList::const_iterator it = ChatContact.begin();
			std::string FirstContactId = (*it).getContactId();

			conv = (GaimConversation *) mConv->gaim_conv_session;
			gaim_conversation_destroy(conv);
			snprintf(ChatName, sizeof(ChatName), "Chat_%d", chatSession.getId());
			conv = gaim_conversation_new(GAIM_CONV_TYPE_CHAT, Gaccount, ChatName);
			mConv->gaim_conv_session = conv;
			conv->ui_data = mConv;
			gaim_conv_chat_add_user(GAIM_CONV_CHAT(conv), FirstContactId.c_str(), NULL, GAIM_CBFLAGS_NONE, false);
			gaim_conv_chat_add_user(GAIM_CONV_CHAT(conv), contactId.c_str(), NULL, GAIM_CBFLAGS_NONE, true);
		}	
		else
		{
			gaim_conv_chat_add_user(GAIM_CONV_CHAT((GaimConversation *)mConv->gaim_conv_session), 
									contactId.c_str(), NULL, GAIM_CBFLAGS_NONE, true);
		}

		contactAddedEvent(*this, chatSession, contactId);
	}
}

void GaimIMChat::removeContact(IMChatSession & chatSession, const std::string & contactId)
{
	mConvInfo_t *mConv = FindChatStructById(chatSession.getId());
	GaimConvChat *conv = GAIM_CONV_CHAT((GaimConversation *)(mConv->gaim_conv_session));

	gaim_conv_chat_remove_user(conv, contactId.c_str(), NULL);
	
	contactRemovedEvent(*this, chatSession, contactId);
}


bool GaimIMChat::equalsTo(std::string login, EnumIMProtocol::IMProtocol protocol)
{
	if (login.compare(_account.getLogin()) == 0 
		&& _account.getProtocol() == protocol)
		return true;
	else
		return false;
}

void GaimIMChat::AddChatSessionInList(mConvInfo_t *conv)
{
	if (IsChatSessionInList(conv->conv_id) == false)
	{
		_GaimChatSessionList.push_back(conv);
	}
}

void GaimIMChat::RemoveChatSessionFromList(int convId)
{
	GaimChatSessionIterator i;
	for (i = _GaimChatSessionList.begin(); i != _GaimChatSessionList.end(); i++)
	{
		if ((*i)->conv_id == convId)
		{
			_GaimChatSessionList.erase(i);
			break;
		}
	}
}

mConvInfo_t *GaimIMChat::FindChatStructById(int convId)
{
	GaimChatSessionIterator i;
	for (i = _GaimChatSessionList.begin(); i != _GaimChatSessionList.end(); i++)
	{
		if ((*i)->conv_id == convId)
		{
			return (*i);;
		}
	}

	return NULL;
}


bool GaimIMChat::IsChatSessionInList(int convId)
{
	GaimChatSessionIterator i;
	for (i = _GaimChatSessionList.begin(); i != _GaimChatSessionList.end(); i++)
	{
		if ((*i)->conv_id == convId)
		{
			return true;
		}
	}

	return false;
}
