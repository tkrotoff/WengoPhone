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

#include <Windows.h>
#include <Winbase.h>

extern "C" {
#include "gaim/conversation.h"
}

#ifdef OS_WIN32
#define snprintf _snprintf
#else
#include <stdio.h>
#endif

#define CHAT_CREATED		0
#define CHAT_NOT_CREATED	-1

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
	mConv->pending_invit = NULL;
	
	AddChatSessionInList(mConv);
	
	//newIMChatSessionCreatedEvent(*this, *chatSession);

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
		LOG_FATAL("imContactSet is empty !!");
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

		createGaimChat(gGC, 0, mlist);
	}
}

void GaimIMChat::closeSession(IMChatSession & chatSession)
{
	mConvInfo_t *mConv = FindChatStructById(chatSession.getId());
	GaimConversation *gConv = (GaimConversation *)mConv->gaim_conv_session;
	
	if (mConv != NULL)
	{
		gaim_conversation_destroy(gConv);
		RemoveChatSessionFromList(chatSession.getId());
	}
}

void GaimIMChat::sendMessage(IMChatSession & chatSession, const std::string & message)
{
	if (message.empty())
		return;

	mConvInfo_t *mConv = FindChatStructById(chatSession.getId());
	GaimConversation *gConv = (GaimConversation *)mConv->gaim_conv_session;
	
	if (gaim_conversation_get_type(gConv) == GAIM_CONV_TYPE_IM)
	{
		gaim_conv_im_send_with_flags(GAIM_CONV_IM(gConv), 
									message.c_str(), GAIM_MESSAGE_SEND);
	}
	else if (gaim_conversation_get_type(gConv) == GAIM_CONV_TYPE_CHAT)
	{
		gaim_conv_chat_send_with_flags(GAIM_CONV_CHAT(gConv), 
									message.c_str(), GAIM_MESSAGE_SEND);
	}
	else
	{
		LOG_FATAL("Unknown conversation type !");
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
	if (!gConv)
		return;

	switch (state)
	{
		case IMChat::TypingStateTyping:
			gState = GAIM_TYPING;
			break;
		
		case IMChat::TypingStateStopTyping:
//			gState = GAIM_TYPED;
			gState = GAIM_NOT_TYPING;
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
	GaimConversation *gConv = NULL;
	GaimAccount *gAccount = gaim_accounts_find(_imAccount.getLogin().c_str(),
											GaimIMPrcl::GetPrclId(_imAccount.getProtocol()));
	int BuddyNbr = chatSession.getIMContactSet().size();

	
	if (!gAccount)
		LOG_FATAL("Account not found !!!");;
	
	if (mConv)
		gConv = (GaimConversation *)mConv->gaim_conv_session;
	else
		LOG_FATAL("ConvInfo not created !!!");

	if (BuddyNbr == 0)
	{
		LOG_ERROR("IMChatSession is empty");
		return;
	}

	if (gaim_conversation_get_type(gConv) == GAIM_CONV_TYPE_IM)
	{
		GList *mlist = NULL;
		IMContactSet &chatContact = (IMContactSet &)chatSession.getIMContactSet();
		IMContactSet::const_iterator it = chatContact.begin();
		const std::string & firstContactId = it->getContactId();
		GaimConnection *gGC;

		gConv = (GaimConversation *) mConv->gaim_conv_session;
		gGC = gaim_conversation_get_gc(gConv);
		mlist = g_list_append(mlist, (char *) contactId.c_str());
		mlist = g_list_append(mlist, (char *) firstContactId.c_str());
		
		if (createGaimChat(gGC, chatSession.getId(), mlist) == CHAT_CREATED)
		{
			gaim_conversation_destroy(gConv);
		}
	}	
	else if (gaim_conversation_get_type(gConv) == GAIM_CONV_TYPE_CHAT)
	{
		serv_chat_invite(gaim_conversation_get_gc(gConv), gaim_conv_chat_get_id(GAIM_CONV_CHAT(gConv)),
			NULL, contactId.c_str());
	}
	else
	{
		LOG_FATAL("Unknown conversation type !");
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

int GaimIMChat::createGaimChat(GaimConnection *gGC, int id, GList *users)
{
	if (!gGC)
		LOG_FATAL("GaimConnection gGC = NULL!!");

	char chatName[100];
	mConvInfo_t *mConv = FindChatStructById(id);
	GaimConversation *gConv;

	if (mConv == NULL)
		mConv = CreateChatSession();
	
	snprintf(chatName, sizeof(chatName), "Chat%d", mConv->conv_id);

	if (_imAccount.getProtocol() == EnumIMProtocol::IMProtocolMSN)
	{
		serv_chat_create(gGC, chatName, users);
		gConv = gaim_find_conversation_with_account(GAIM_CONV_TYPE_CHAT, chatName, gGC->account);

		if (!gConv)
			LOG_FATAL("Chat doesn't exist !!");

		mConv->gaim_conv_session = gConv;
		gConv->ui_data = mConv;
	}
	else	
	{
		GHashTable *components;
		GList *bl;

		components = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
		
		g_hash_table_replace(components, g_strdup("room"), g_strdup(chatName));
		
		if (_imAccount.getProtocol() == EnumIMProtocol::IMProtocolYahoo)
		{
			g_hash_table_replace(components, g_strdup("topic"), g_strdup("Join my conference..."));
			g_hash_table_replace(components, g_strdup("type"), g_strdup("Conference"));
		}
		else if (_imAccount.getProtocol() == EnumIMProtocol::IMProtocolAIMICQ)
		{
			g_hash_table_replace(components, g_strdup("exchange"), g_strdup("16"));
		}

		serv_join_chat(gGC, components);
		g_hash_table_destroy(components);

		gConv = gaim_find_conversation_with_account(GAIM_CONV_TYPE_CHAT, chatName, gGC->account);

		if (!gConv)
		{
			mConv->pending_invit = users;
			return CHAT_NOT_CREATED;
		}

		mConv->gaim_conv_session = gConv;
		gConv->ui_data = mConv;

		for (bl = users; bl != NULL; bl = bl->next)
		{
			serv_chat_invite(gGC, gaim_conv_chat_get_id(GAIM_CONV_CHAT(gConv)),
							"Join my conference...", (char *)bl->data);
		}
	}

	return CHAT_CREATED;
}

bool GaimIMChat::IsChatSessionInList(int convId)
{
	if (FindChatStructById(convId) != NULL)
		return true;
	else
		return false;
}
