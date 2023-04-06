/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include "PurpleChatSession.h"

#include <purplewrapper/PurpleEnumIMProtocol.h>
#include <purplewrapper/PurpleWrapper.h>

#include <cutil/global.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <util/Uuid.h>

extern "C" {
#include <libpurple/connection.h>
#include <libpurple/conversation.h>
#include <libpurple/signals.h>
#include <libpurple/util.h>
}

#ifdef OS_WINDOWS
	#define snprintf _snprintf
#endif

typedef struct misc_s {
	misc_s() {
		imContact = NULL;
		instance = NULL;
		data_int = 0;
		cbk_id = 0;
	}

	const IMContact * imContact;
	PurpleChatSession * instance;
	std::string message;
	int data_int;
	int cbk_id;
} misc_t;

QMutex * PurpleChatSession::_mutex = NULL;

PurpleChatSession::PurpleChatSession(CoIpManager & coIpManager)
	: IChatSessionPlugin(coIpManager) {

	memset(&_convInfo, 0, sizeof(mConvInfo_t));
	_convInfo.conv_session = this;
	_convInfo.conv_id = Uuid::generateInteger();
	_mutex = new QMutex();
}

PurpleChatSession::~PurpleChatSession() {
	OWSAFE_DELETE(_mutex);
}

bool PurpleChatSession::addIMContactCbk(void * data) {
	QMutexLocker lock(_mutex);
	const IMContact * imContact = ((misc_t *)data)->imContact;
	PurpleChatSession * instance = ((misc_t *)data)->instance;

	PurpleConversation * pConv = (PurpleConversation *) instance->_convInfo.purple_conv_session;
	if (pConv) {
		// Chat is running
		if (purple_conversation_get_type(pConv) == PURPLE_CONV_TYPE_IM) {
			// Session type is PURPLE_CONV_TYPE_IM. However this type can
			// only contain one contact. We change its type by creating a 'chat'.
			GList * mlist = NULL;
			IMContactList::const_iterator it = instance->_imContactList.begin();
			const std::string & firstContactId = (*it).getContactId();

			mlist = g_list_append(mlist, (char *) imContact->getContactId().c_str());
			mlist = g_list_append(mlist, (char *) firstContactId.c_str());

			createPurpleChat(mlist, &instance->_convInfo, imContact->getAccountType());
		} else if (purple_conversation_get_type(pConv) == PURPLE_CONV_TYPE_CHAT) {
			serv_chat_invite(purple_conversation_get_gc(pConv),
				purple_conv_chat_get_id(PURPLE_CONV_CHAT(pConv)),
				NULL, imContact->getContactId().c_str());
		} else {
			LOG_FATAL("unknown conversation type");
		}
		instance->IChatSessionPlugin::addIMContact(*imContact);

		purple_timeout_remove(((misc_t *)data)->cbk_id);
		delete ((misc_t *) data);
	}

	return TRUE;
}

void PurpleChatSession::addIMContact(const IMContact & imContact) {
	misc_t * data = new misc_t();
	data->imContact = &imContact;
	data->instance = this;

	QMutexLocker lock(_mutex);
	data->cbk_id = purple_timeout_add(0, (GSourceFunc) PurpleChatSession::addIMContactCbk, data);
}

bool PurpleChatSession::removeIMContactCbk(void * data) {
	QMutexLocker lock(_mutex);
	const IMContact * imContact = ((misc_t *)data)->imContact;
	PurpleChatSession * instance = ((misc_t *)data)->instance;
	PurpleConversation * pConv = (PurpleConversation *) instance->_convInfo.purple_conv_session;

	if (pConv) {
		PurpleConvChat * conv = PURPLE_CONV_CHAT(pConv);
		purple_conv_chat_remove_user(conv, imContact->getContactId().c_str(), NULL);

		instance->IChatSessionPlugin::removeIMContact(*imContact);
	}

	purple_timeout_remove(((misc_t *)data)->cbk_id);
	delete ((misc_t *) data);

	return TRUE;
}

void PurpleChatSession::removeIMContact(const IMContact & imContact) {
	misc_t * data = new misc_t();
	data->imContact = &imContact;
	data->instance = this;

	QMutexLocker lock(_mutex);
	data->cbk_id = purple_timeout_add(0, (GSourceFunc) PurpleChatSession::removeIMContactCbk, data);
}

void PurpleChatSession::setIMContactList(const IMContactList & imContactList) {
	PurpleConversation * pConv = (PurpleConversation *) _convInfo.purple_conv_session;
	if (pConv) {
		//TODO: if session is running
	}

	IChatSessionPlugin::setIMContactList(imContactList);
}

bool PurpleChatSession::sendMessageCbk(void * data) {
	QMutexLocker lock(_mutex);
	PurpleChatSession * instance = ((misc_t *)data)->instance;
	char * cleanMess = (char *) ((misc_t *)data)->message.c_str();
	PurpleConversation * pConv = (PurpleConversation *) instance->_convInfo.purple_conv_session;

	// special case for ICQ
	PurpleAccount * pAccount = purple_conversation_get_account(pConv);
	PurplePlugin * prpl = purple_find_prpl(purple_account_get_protocol_id(pAccount));
	if (prpl) {
		PurplePluginProtocolInfo * prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);
		if (prpl_info->list_icon != NULL) {
			if (!strcmp("icq", prpl_info->list_icon(pAccount, NULL))) {
				cleanMess = (char *) purple_markup_strip_html(((misc_t *)data)->message.c_str());
			}
		}
	}
	////

	if (purple_conversation_get_type(pConv) == PURPLE_CONV_TYPE_IM) {
		purple_conv_im_send_with_flags(PURPLE_CONV_IM(pConv),
			cleanMess, PURPLE_MESSAGE_SEND);
	} else if (purple_conversation_get_type(pConv) == PURPLE_CONV_TYPE_CHAT) {
		purple_conv_chat_send_with_flags(PURPLE_CONV_CHAT(pConv),
			cleanMess, PURPLE_MESSAGE_SEND);
	} else {
		LOG_FATAL("unknown conversation type");
	}

	purple_timeout_remove(((misc_t *)data)->cbk_id);
	delete ((misc_t *) data);

	return TRUE;
}

void PurpleChatSession::sendMessage(const std::string & message) {
	if (message.empty()) {
		return;
	}

	misc_t * data = new misc_t();
	data->instance = this;
	data->message = message;

	QMutexLocker lock(_mutex);
	data->cbk_id = purple_timeout_add(0, (GSourceFunc) PurpleChatSession::sendMessageCbk, data);
}

bool PurpleChatSession::setTypingStateCbk(void * data) {
	QMutexLocker lock(_mutex);
	EnumChatTypingState::ChatTypingState state = 
		(EnumChatTypingState::ChatTypingState)((misc_t *)data)->data_int;
	PurpleTypingState gState = PURPLE_NOT_TYPING;
	PurpleConversation * pConv = 
		(PurpleConversation *)((misc_t *)data)->instance->_convInfo.purple_conv_session;

	switch (state) {
	case EnumChatTypingState::ChatTypingStateTyping:
		gState = PURPLE_TYPING;
		break;

	case EnumChatTypingState::ChatTypingStateStopTyping:
//		gState = PURPLE_TYPED;
		gState = PURPLE_NOT_TYPING;
		break;

	default:
		gState = PURPLE_NOT_TYPING;
		break;
	}

	serv_send_typing(purple_conversation_get_gc(pConv),
		purple_conversation_get_name(pConv), gState);

	purple_timeout_remove(((misc_t *)data)->cbk_id);
	delete ((misc_t *) data);
	
	return TRUE;
}

void PurpleChatSession::setTypingState(EnumChatTypingState::ChatTypingState state) {
	if (!_convInfo.purple_conv_session) {
		return;
	}
	
	misc_t * data = new misc_t();
	data->instance = this;
	data->data_int = state;

	QMutexLocker lock(_mutex);
	data->cbk_id = purple_timeout_add(0, (GSourceFunc) PurpleChatSession::setTypingStateCbk, data);
}

bool PurpleChatSession::startCbk(void * data) {
	QMutexLocker lock(_mutex);

	PurpleChatSession * instance = ((misc_t *)data)->instance;
	PurpleAccount * pAccount = purple_accounts_find(PurpleWrapper::purpleLogin(instance->_account).c_str(),
		PurpleIMPrcl::getPrclId(instance->_account.getAccountType()));

	if (!pAccount) {
		LOG_ERROR("cannot find LibPurple account");
		return FALSE;
	}

	if (instance->_imContactList.empty()) {
		LOG_ERROR("no contact in Session, cannot start session");
		return FALSE;
	} else if (instance->_imContactList.size() == 1) {
		IMContactList::const_iterator it = instance->_imContactList.begin();
		std::string contactId = (*it).getContactId();

		//TODO: check this part
		PurpleConversation * pConv = NULL;
		if ((pConv = purple_find_conversation_with_account(PURPLE_CONV_TYPE_IM,
			contactId.c_str(), pAccount))) {
			LOG_INFO("a conversation with this participant already exists");
			// A conversation with this contact already exists
			return FALSE;
		}

		pConv =  purple_conversation_new(PURPLE_CONV_TYPE_IM, pAccount, contactId.c_str());
		instance->_convInfo.purple_conv_session = pConv;
		pConv->ui_data = &instance->_convInfo;
	} else {
		GList * mlist = NULL;
		for (IMContactList::const_iterator it = instance->_imContactList.begin();
			it != instance->_imContactList.end();
			it++) {
			mlist = g_list_append(mlist, (char *) (*it).getContactId().c_str());
		}

		createPurpleChat(mlist, &instance->_convInfo, instance->_account.getAccountType());
	}

	purple_timeout_remove(((misc_t *)data)->cbk_id);
	delete ((misc_t *) data);
	
	return TRUE;
}

void PurpleChatSession::start() {
	misc_t * data = new misc_t();
	data->instance = this;

	QMutexLocker lock(_mutex);
	data->cbk_id = purple_timeout_add(0, (GSourceFunc) PurpleChatSession::startCbk, data);
}

void PurpleChatSession::pause() {
}

void PurpleChatSession::resume() {
}

bool PurpleChatSession::stopCbk(void * data) {
	QMutexLocker lock(_mutex);
	PurpleChatSession * instance = ((misc_t *)data)->instance;
	purple_conversation_destroy((PurpleConversation *) instance->_convInfo.purple_conv_session);
	instance->_convInfo.purple_conv_session = NULL;

	purple_timeout_remove(((misc_t *)data)->cbk_id);
	delete ((misc_t *) data);

	return TRUE;
}

void PurpleChatSession::stop() {
	misc_t * data = new misc_t();
	data->instance = this;

	QMutexLocker lock(_mutex);
	data->cbk_id = purple_timeout_add(0, (GSourceFunc) PurpleChatSession::stopCbk, data);
}

void PurpleChatSession::createPurpleChat(GList * users, mConvInfo_t * convInfo, 
	EnumAccountType::AccountType accountType) {
	PurpleConversation * pConv = (PurpleConversation *) convInfo->purple_conv_session;
	PurpleConnection * pConnection = purple_conversation_get_gc(pConv);

	if (!pConnection) {
		LOG_FATAL("PurpleConnection pConnection is NULL");
	}

	char chatName[100];
	snprintf(chatName, sizeof(chatName), "Chat%d", convInfo->conv_id);

	if (convInfo->purple_conv_session) {
		purple_conversation_destroy((PurpleConversation *) convInfo->purple_conv_session);
		convInfo->purple_conv_session = NULL;
	}

	if (accountType == EnumAccountType::AccountTypeMSN) {
		serv_chat_create(pConnection, chatName, users);
	} else {
		GHashTable * components = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

		g_hash_table_replace(components, g_strdup("room"), g_strdup(chatName));

		if (accountType == EnumAccountType::AccountTypeYahoo) {
			g_hash_table_replace(components, g_strdup("topic"), g_strdup("Join my conference..."));
			g_hash_table_replace(components, g_strdup("type"), g_strdup("Conference"));
		} else if ((accountType == EnumAccountType::AccountTypeAIM)
			|| (accountType == EnumAccountType::AccountTypeICQ)) {
			g_hash_table_replace(components, g_strdup("exchange"), g_strdup("16"));
		}

		convInfo->pending_invit = users;

		serv_join_chat(pConnection, components);
		g_hash_table_destroy(components);
	}

	pConv = purple_find_conversation_with_account(
		PURPLE_CONV_TYPE_CHAT, chatName, pConnection->account);
	if (!pConv) {
		LOG_FATAL("chat doesn't exist");
	}
	convInfo->purple_conv_session = pConv;
	pConv->ui_data = convInfo;
}
