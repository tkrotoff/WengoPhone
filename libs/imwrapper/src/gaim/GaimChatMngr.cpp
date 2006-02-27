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

#include "GaimChatMngr.h"
#include "GaimAccountMngr.h"
#include "GaimEnumIMProtocol.h"

#include <imwrapper/IMContact.h>
#include <Logger.h>

extern "C" {
#include "gaim/util.h"
}


/* ***************** GAIM CALLBACK ***************** */
void C_CreateConversationCbk(GaimConversation *conv)
{
	GaimChatMngr::CreateConversationCbk(conv);
}

static void C_DestroyConversationCbk(GaimConversation *conv)
{
	GaimChatMngr::DestroyConversationCbk(conv);
}

static void C_WriteChatCbk(GaimConversation *conv, const char *who,
						const char *message, GaimMessageFlags flags,
						time_t mtime)
{
	GaimChatMngr::WriteChatCbk(conv, who, message, flags, mtime);
}

static void C_WriteIMCbk(GaimConversation *conv, const char *who,
						const char *message, GaimMessageFlags flags, time_t mtime)
{
	GaimChatMngr::WriteIMCbk(conv, who, message, flags, mtime);
}

static void C_WriteConvCbk(GaimConversation *conv, const char *name, const char *alias,
						const char *message, GaimMessageFlags flags,
						time_t mtime)
{
	GaimChatMngr::WriteConvCbk(conv, name, alias, message, flags, mtime);
}

static void C_ChatAddUsersCbk(GaimConversation *conv, GList *users, GList *flags, 
						   GList *aliases, gboolean new_arrivals)
{
	GaimChatMngr::ChatAddUsersCbk(conv, users, flags, aliases, new_arrivals);
}

static void C_ChatRenameUserCbk(GaimConversation *conv, const char *old_name,
							const char *new_name, const char *new_alias)
{
	GaimChatMngr::ChatRenameUserCbk(conv, old_name, new_name, new_alias);
}

static void C_ChatRemoveUserCbk(GaimConversation *conv, const char *user)
{
	GaimChatMngr::ChatRemoveUserCbk(conv, user);
}

static void C_ChatRemoveUsersCbk(GaimConversation *conv, GList *users)
{
	GaimChatMngr::ChatRemoveUsersCbk(conv, users);
}

static void C_ChatUpdateUserCbk(GaimConversation *conv, const char *user)
{
	GaimChatMngr::ChatUpdateUserCbk(conv, user);
}

void C_PresentConvCbk(GaimConversation *conv)
{
    GaimChatMngr::PresentConvCbk(conv);
}

gboolean C_HasFocusCbk(GaimConversation *conv)
{
	return GaimChatMngr::HasFocusCbk(conv);
}

static gboolean C_CustomSmileyAddCbk(GaimConversation *conv, const char *smile, gboolean remote)
{
	return GaimChatMngr::CustomSmileyAddCbk(conv, smile, remote);
}

static void C_CustomSmileyWriteCbk(GaimConversation *conv, const char *smile,
								const guchar *data, gsize size)
{
	GaimChatMngr::CustomSmileyWriteCbk(conv, smile, data, size);
}

static void C_CustomSmileyCloseCbk(GaimConversation *conv, const char *smile)
{
	GaimChatMngr::CustomSmileyCloseCbk(conv, smile);
}

static void C_UpdatedCbk(GaimConversation *conv, GaimConvUpdateType type)
{
  	GaimChatMngr::UpdatedCbk(conv, type);
}

GaimConversationUiOps chat_wg_ops =	{
												C_CreateConversationCbk,
												C_DestroyConversationCbk,	/* destroy_conversation */
												C_WriteChatCbk,				/* write_chat           */
												C_WriteIMCbk,				/* write_im             */
												C_WriteConvCbk,				/* write_conv           */
												C_ChatAddUsersCbk,			/* chat_add_users       */
												C_ChatRenameUserCbk,		/* chat_rename_user     */
												C_ChatRemoveUserCbk,		/* chat_remove_user     */
												C_ChatRemoveUsersCbk,		/* chat_remove_users    */
												C_ChatUpdateUserCbk,		/* chat_update_user     */
												C_PresentConvCbk,			/* present				*/
												C_HasFocusCbk,				/* has_focus            */
												C_CustomSmileyAddCbk,		/* custom_smiley_add    */
												C_CustomSmileyWriteCbk,		/* custom_smiley_write	*/
												C_CustomSmileyCloseCbk,		/* custom_smiley_close	*/
												C_UpdatedCbk				/* updated              */
											};

/* ************************************************** */

GaimChatMngr * GaimChatMngr::_staticInstance = NULL;
GaimAccountMngr *GaimChatMngr::_accountMngr = NULL;
std::list<GaimIMChat *> GaimChatMngr::_gaimIMChatList;

GaimChatMngr::GaimChatMngr()
{
	_accountMngr = GaimAccountMngr::getInstance();
}

GaimChatMngr *GaimChatMngr::getInstance()
{
	if (!_staticInstance)
		_staticInstance = new GaimChatMngr();
	
	return _staticInstance;
}

void GaimChatMngr::CreateConversationCbk(GaimConversation *conv)
{
	GaimAccount *gAccount = gaim_conversation_get_account(conv);
	GaimConversationType chatType = gaim_conversation_get_type(conv);
	const char *gPrclId = gaim_account_get_protocol_id(gAccount);	
	IMAccount *account = _accountMngr->FindIMAccount(gaim_account_get_username(gAccount),
								GaimEnumIMProtocol::GetEnumIMProtocol(gPrclId));
	GaimIMChat *mChat = FindIMChat(*account);

	mConvInfo_t *mConv = new mConvInfo_t();
	IMChatSession *chatSession = new IMChatSession(*mChat);

	mConv->conv_session = chatSession;
	mConv->conv_id = chatSession->getId();
	mConv->gaim_conv_session = conv;
	conv->ui_data = mConv;

	mChat->AddChatSessionInList(mConv);

	mChat->newIMChatSessionCreatedEvent(*mChat, *chatSession);

	if (chatType == GAIM_CONV_TYPE_IM)
	{
		mChat->contactAddedEvent(*mChat, *chatSession, gaim_conversation_get_name(conv));
		mChat->sendMessage(*chatSession, std::string("I'm joining the chat (^_^) !"));
	}
	else if (chatType == GAIM_CONV_TYPE_CHAT)
	{
		;
	}
	 
	
	//if (mConv == NULL)
	//{
	//	// We receive an incoming message
	//	// and the ChatSession is not created yet

	//	mConv = gChat->mCreateSession();
	//	conv->ui_data = mConv;
	//	chatSession = (IMChatSession *) mConv->conv_session;
	//	
	//	if (chatType == GAIM_CONV_TYPE_IM)
	//	{
	//		gChat->contactAddedEvent(*gChat, *chatSession, 
	//								gaim_conversation_get_name(conv));
	//	}
	//	else if (chatType == GAIM_CONV_TYPE_CHAT)
	//	{
	//		GList *gChatUsers = gaim_conv_chat_get_users(GAIM_CONV_CHAT(conv));

	//		GaimConvChatBuddy *gUser;
	//		for ( ; gChatUsers; gChatUsers = gChatUsers->next)
	//		{
	//			gUser = (GaimConvChatBuddy *) gChatUsers->data;
	//			gChat->contactAddedEvent(*gChat, *chatSession, gUser->name);
	//		}
	//	}

	//	return;
	//}

	//// Get IMChatSession buddies number to know
	//// if we have to send an event to IMWrapper
	//// (because of an asynchrone management of create/add
	//// operations between IMWrapper and Gaim)
	//chatSession = (IMChatSession *) mConv->conv_session;

	//if (chatType == GAIM_CONV_TYPE_IM)
	//{
	//	if (chatSession->getIMContactList().size() == 0)
	//	{
	//		gChat->contactAddedEvent(*gChat, *chatSession, gaim_conversation_get_name(conv));
	//	}
	//}
	//else if (chatType == GAIM_CONV_TYPE_CHAT)
	//{
	//	const IMChatSession::IMContactList &ChatContact = chatSession->getIMContactList();
	//	IMChatSession::IMContactList::const_iterator it = ChatContact.begin();
	//	const char *contactId = (*it).getContactId().c_str();
	//	
	//	GList *gChatUsers = gaim_conv_chat_get_users(GAIM_CONV_CHAT(conv));
	//	
	//	GaimConvChatBuddy *gUser;

	//	for ( ; gChatUsers; gChatUsers = gChatUsers->next)
	//	{	
	//		gUser = (GaimConvChatBuddy *) gChatUsers->data;
	//		if (gaim_utf8_strcasecmp(gUser->name, contactId) != 0)
	//		{
	//			gChat->contactAddedEvent(*gChat, *chatSession, gUser->name);
	//			break;
	//		}
	//	}
	//}

	fprintf(stderr, "wgconv : gaim_wgconv_new()\n");
}

void GaimChatMngr::DestroyConversationCbk(GaimConversation *conv)
{
	fprintf(stderr, "wgconv : gaim_wgconv_destroy()\n");
	
}

void GaimChatMngr::WriteChatCbk(GaimConversation *conv, const char *who,
							const char *message, GaimMessageFlags flags,
							time_t mtime)
{
	gaim_conversation_write(conv, who, message, flags, mtime);
}

void GaimChatMngr::WriteIMCbk(GaimConversation *conv, const char *who,
							const char *message, GaimMessageFlags flags,
							time_t mtime)
{
	gaim_conversation_write(conv, who, message, flags, mtime);
}

void GaimChatMngr::WriteConvCbk(GaimConversation *conv, const char *name, const char *alias,
							const char *message, GaimMessageFlags flags,
							time_t mtime)
{
	if ((flags & GAIM_MESSAGE_RECV))
	{
		mConvInfo_t *mConv = (mConvInfo_t *)conv->ui_data;
		GaimIMChat *gIMChat = FindIMChatByGaimConv(conv);
		gIMChat->messageReceivedEvent(*gIMChat, *((IMChatSession *)(mConv->conv_session)), std::string(name), std::string(message));
	}
}

void GaimChatMngr::ChatAddUsersCbk(GaimConversation *conv, GList *users,
								GList *flags, GList *aliases, gboolean new_arrivals)
{
}

void GaimChatMngr::ChatRenameUserCbk(GaimConversation *conv, const char *old_name,
									const char *new_name, const char *new_alias)
{
}

void GaimChatMngr::ChatRemoveUserCbk(GaimConversation *conv, const char *user)
{
}

void GaimChatMngr::ChatRemoveUsersCbk(GaimConversation *conv, GList *users)
{
}

void GaimChatMngr::ChatUpdateUserCbk(GaimConversation *conv, const char *user)
{
}

void GaimChatMngr::PresentConvCbk(GaimConversation *conv)
{
}

gboolean GaimChatMngr::HasFocusCbk(GaimConversation *conv)
{
	return TRUE;
}

gboolean GaimChatMngr::CustomSmileyAddCbk(GaimConversation *conv, const char *smile, 
									   gboolean remote)
{
	return TRUE;
}

void GaimChatMngr::CustomSmileyWriteCbk(GaimConversation *conv, const char *smile,
									const guchar *data, gsize size)
{
}

void GaimChatMngr::CustomSmileyCloseCbk(GaimConversation *conv, const char *smile)
{
}

void GaimChatMngr::UpdatedCbk(GaimConversation *conv, GaimConvUpdateType type)
{
}

/* **************** IMWRAPPER/GAIM INTERFACE ****************** */
GaimIMChat *GaimChatMngr::FindIMChatByGaimConv(void *gConv)
{
	GaimAccount *gAccount = gaim_conversation_get_account((GaimConversation *) gConv);
	const char *gPrclId = gaim_account_get_protocol_id(gAccount);
	IMAccount *account = _accountMngr->FindIMAccount(gaim_account_get_username(gAccount),
													GaimEnumIMProtocol::GetEnumIMProtocol(gPrclId));
	GaimIMChat *mChat = FindIMChat(*account);
	
	return mChat;
}


/* **************** MANAGE CHAT_LIST ****************** */
GaimIMChat *GaimChatMngr::FindIMChat(IMAccount &account)
{
	GaimIMChatIterator i;
	for (i = _gaimIMChatList.begin(); i != _gaimIMChatList.end(); i++)
	{
		if ((*i)->equalsTo(account.getLogin(), account.getProtocol()))
		{
			return (*i);
		}
	}

	return NULL;
}

GaimIMChat *GaimChatMngr::AddIMChat(IMAccount &account)
{
	GaimIMChat *mIMChat = FindIMChat(account);

	if (mIMChat == NULL)
	{
		mIMChat = new GaimIMChat(account);

		_gaimIMChatList.push_back(mIMChat);
	}
	
	return mIMChat;
}

void GaimChatMngr::RemoveIMChat(IMAccount &account)
{
	GaimIMChatIterator i;
	for (i = _gaimIMChatList.begin(); i != _gaimIMChatList.end(); i++)
	{
		if ((*i)->equalsTo(account.getLogin(), account.getProtocol()))
		{
			_gaimIMChatList.erase(i);
			delete (*i);
			break;
		}
	}
}
/* ****************************************************** */
