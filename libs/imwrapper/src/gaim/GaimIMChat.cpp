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

#include <util/Logger.h>
#include <cutil/global.h>
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
	: IMChat(account)
{

}

mConvInfo_t *GaimIMChat::CreateChatSession()
{
	mConvInfo_t *mConv = new mConvInfo_t();
	IMChatSession *chatSession = new IMChatSession(*this);
	
	mConv->conv_session = chatSession;
	mConv->conv_id = chatSession->getId();
	
	AddChatSessionInList(mConv);
	
	newIMChatSessionCreatedEvent(*this, *chatSession);

	return mConv;
}

void GaimIMChat::createSession(IMContactSet & imContactSet)
{
	GaimAccount *gAccount = gaim_accounts_find(_imAccount.getLogin().c_str(),
											GaimIMPrcl::GetPrclId(_imAccount.getProtocol()));

	GaimConversation *gConv = NULL;
	IMContactSet::const_iterator it;

	if (!gAccount)
		return;

	if (imContactSet.size() == 0)
	{
		return;
	}
	else if (imContactSet.size() == 1)
	{
		it = imContactSet.begin();
		std::string contactId = (*it).getContactId();

		gConv = gaim_conversation_new(GAIM_CONV_TYPE_IM, gAccount, contactId.c_str());
	}
	else
	{
		GList *mlist = NULL;
		GaimConnection *gGC = gaim_account_get_connection(gAccount);
		
		for (it = imContactSet.begin(); it != imContactSet.end(); it++)
		{
			mlist = g_list_append(mlist, (char *) it->getContactId().c_str());
		}

		serv_chat_create(gGC, "0", mlist);
	}

	//mConvInfo_t	*conv = new mConvInfo_t();
	//IMChatSession *chatSession = new IMChatSession(*this);

	//conv->conv_session = (void *) chatSession;
	//conv->conv_id = chatSession->getId();
	//conv->gaim_conv_session = gConv;
	//gConv->ui_data = conv;

	//AddChatSessionInList(conv);
	//newIMChatSessionCreatedEvent(*this, *chatSession);

	//if (gConv->type == GAIM_CONV_TYPE_IM)
	//	contactAddedEvent(*this, *chatSession, gaim_conversation_get_name(gConv));
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
	if (message.empty())
		return;

	mConvInfo_t *mConv = FindChatStructById(chatSession.getId());
	int BuddyNbr = chatSession.getIMContactSet().size();
	
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

void GaimIMChat::changeTypingState(IMChatSession & chatSession, IMChat::TypingState state)
{
	GaimTypingState gState = GAIM_NOT_TYPING;
	mConvInfo_t *mConv = FindChatStructById(chatSession.getId());
	GaimConversation *gConv = NULL;
	
	if (!mConv)
		return;

	gConv = (GaimConversation *)mConv->gaim_conv_session;

	switch (state)
	{
		case IMChat::TypingStateTyping:
			gState = GAIM_TYPING;
			break;
		
		case IMChat::TypingStateStopTyping:
			gState = GAIM_TYPED;
			break;

		default:
			gState = GAIM_NOT_TYPING;
			break;
	}

	serv_send_typing(gaim_conversation_get_gc(gConv),
					 gaim_conversation_get_name(gConv),
					 gState);
}

void GaimIMChat::addContact(IMChatSession & chatSession, const std::string & contactId)
{
	mConvInfo_t *mConv = FindChatStructById(chatSession.getId());
	GaimConversation *gConv;
	GaimAccount *gAccount = gaim_accounts_find(_imAccount.getLogin().c_str(),
											GaimIMPrcl::GetPrclId(_imAccount.getProtocol()));
	int BuddyNbr = chatSession.getIMContactSet().size();
	
	if (!gAccount)
		return;
	
	if (BuddyNbr == 0)
	{
		gConv = gaim_conversation_new(GAIM_CONV_TYPE_IM, gAccount, 
									contactId.c_str());

		mConv->gaim_conv_session = gConv;
	}
	else if (BuddyNbr == 1)
	{
		GList *mlist = NULL;
		char chatName[100];
		const IMContactSet &chatContact = chatSession.getIMContactSet();
		IMContactSet::const_iterator it = chatContact.begin();
		std::string firstContactId = it->getContactId();
		GaimConnection *gGC;

		gConv = (GaimConversation *) mConv->gaim_conv_session;
		gGC = gaim_conversation_get_gc(gConv);
		gaim_conversation_destroy(gConv);
		snprintf(chatName, sizeof(chatName), "%d", chatSession.getId());
		mlist = g_list_append(mlist, (char *) contactId.c_str());
		mlist = g_list_append(mlist, (char *) firstContactId.c_str());
		serv_chat_create(gGC, chatName, mlist);
	}	
	else
	{
		gaim_conv_chat_add_user(GAIM_CONV_CHAT((GaimConversation *)mConv->gaim_conv_session), 
								contactId.c_str(), NULL, GAIM_CBFLAGS_NONE, true);
		
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
	IMAccount imAccount(login, "", protocol);

	if (_imAccount == imAccount)
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
			return (*i);
		}
	}

	return NULL;
}

bool GaimIMChat::IsChatSessionInList(int convId)
{
	if (FindChatStructById(convId) != NULL)
		return true;
	else
		return false;
}
