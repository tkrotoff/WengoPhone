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

#include <Logger.h>


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

GaimChatMngr * GaimChatMngr::StaticInstance = NULL;
std::list<GaimIMChat *> GaimChatMngr::_GaimIMChatList;

GaimChatMngr::GaimChatMngr()
{
}

GaimChatMngr *GaimChatMngr::getInstance()
{
	if (!StaticInstance)
		StaticInstance = new GaimChatMngr();
	
	return StaticInstance;
}

void GaimChatMngr::CreateConversationCbk(GaimConversation *conv)
{
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
		GaimAccount *Gaccount = gaim_conversation_get_account(conv);
		const char *GPrclId = gaim_account_get_protocol_id(Gaccount);
		IMAccount *account = GaimAccountMngr::FindIMAccount(gaim_account_get_username(Gaccount),
											GaimEnumIMProtocol::GetEnumIMProtocol(GPrclId));
		
		GaimIMChat *GIMChat = FindIMChat(*account);
		GIMChat->messageReceivedEvent(*GIMChat, *((IMChatSession *)(mConv->conv_session)), std::string(name), std::string(message));

		//if (gaim_conversation_get_type(conv) == GAIM_CONV_TYPE_IM)
		//{
		//	
		//}
		//else
		//{
		//}
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


/* **************** MANAGE CHAT_LIST ****************** */
GaimIMChat *GaimChatMngr::FindIMChat(IMAccount &account)
{
	GaimIMChatIterator i;
	for (i = _GaimIMChatList.begin(); i != _GaimIMChatList.end(); i++)
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

		_GaimIMChatList.push_back(mIMChat);
	}
	
	return mIMChat;
}

void GaimChatMngr::RemoveIMChat(IMAccount &account)
{
	GaimIMChatIterator i;
	for (i = _GaimIMChatList.begin(); i != _GaimIMChatList.end(); i++)
	{
		if ((*i)->equalsTo(account.getLogin(), account.getProtocol()))
		{
			_GaimIMChatList.erase(i);
			delete (*i);
			break;
		}
	}
}
/* ****************************************************** */
