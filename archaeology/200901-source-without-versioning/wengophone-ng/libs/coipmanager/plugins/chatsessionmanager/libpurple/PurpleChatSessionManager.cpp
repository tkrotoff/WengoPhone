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

#include "PurpleChatSessionManager.h"

#include "PurpleChatSession.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <purplewrapper/PurpleEnumIMProtocol.h>
#include <purplewrapper/PurpleWrapper.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

IChatSessionManagerPlugin * coip_chatsession_plugin_init(CoIpManager & coIpManager) {
	return new PurpleChatSessionManager(coIpManager);
}

extern "C" {
#include <libpurple/util.h>
}

static PurpleConversationUiOps conv_wg_ops = {
	PurpleChatSessionManager::createConversationCbk,
	PurpleChatSessionManager::destroyConversationCbk,		/* destroy_conversation */
	PurpleChatSessionManager::writeChatCbk,				/* write_chat */
	PurpleChatSessionManager::writeIMCbk,					/* write_im */
	PurpleChatSessionManager::writeConvCbk,				/* write_conv */
	PurpleChatSessionManager::chatAddUsersCbk,			/* chat_add_users */
	PurpleChatSessionManager::chatRenameUserCbk,			/* chat_rename_user */
	PurpleChatSessionManager::chatRemoveUsersCbk,			/* chat_remove_users */
	PurpleChatSessionManager::chatUpdateUserCbk,			/* chat_update_user */
	PurpleChatSessionManager::presentConvCbk,				/* present */
	PurpleChatSessionManager::hasFocusCbk,				/* has_focus */
	PurpleChatSessionManager::customSmileyAddCbk,			/* custom_smiley_add */
	PurpleChatSessionManager::customSmileyWriteCbk,		/* custom_smiley_write */
	PurpleChatSessionManager::customSmileyCloseCbk,		/* custom_smiley_close */
	PurpleChatSessionManager::sendConfirm,				/* send_confirm */
};

PurpleChatSessionManager * PurpleChatSessionManager::_instance = NULL;

PurpleChatSessionManager::PurpleChatSessionManager(CoIpManager & coIpManager)
	: IChatSessionManagerPlugin(coIpManager) {

	PurpleChatSessionManager::_instance = this;

	_supportedAccountType.push_back(EnumAccountType::AccountTypeAIM);
	_supportedAccountType.push_back(EnumAccountType::AccountTypeGTalk);
	_supportedAccountType.push_back(EnumAccountType::AccountTypeICQ);
	_supportedAccountType.push_back(EnumAccountType::AccountTypeJabber);
	_supportedAccountType.push_back(EnumAccountType::AccountTypeMSN);
	_supportedAccountType.push_back(EnumAccountType::AccountTypeYahoo);

	PurpleWrapper::getInstance().setConversationCallbacks(&conv_wg_ops);
}

PurpleChatSessionManager::~PurpleChatSessionManager() {
	// _purpleChatSessionList is cleaned by ChatSessionManager

	PurpleChatSessionManager::_instance = NULL;
}

void PurpleChatSessionManager::initialize() {
	initChatEvent();
}

IChatSessionPlugin * PurpleChatSessionManager::createIChatSessionPlugin() {
	return new PurpleChatSession(_coIpManager);
}

bool PurpleChatSessionManager::canCreateISession(const Account & account, const ContactList & contactList) const {
	bool result = false;

	EnumAccountType::AccountType accountType = account.getAccountType();
	std::vector<EnumAccountType::AccountType>::const_iterator acIt =
		std::find(_supportedAccountType.begin(), _supportedAccountType.end(), accountType);
	if (acIt != _supportedAccountType.end()) {
		for (ContactList::const_iterator it = contactList.begin();
			it != contactList.end();
			it++) {
			IMContactList imContactList = (*it).getIMContactList();
			if (IMContactListHelper::hasIMContactOfProtocol(imContactList, accountType)) {
				result = true;
			} else {
				result = false;
				break;
			}
		}
	}

	return result;	
}

IMContactList PurpleChatSessionManager::getValidIMContacts(const Account & account, const ContactList & contactList) const {
	IMContactList result;

	for (ContactList::const_iterator it = contactList.begin();
		it != contactList.end();
		it++) {
		IMContactList imContactList = (*it).getIMContactList();

		// Look for an IMContact associated with the given Account
		IMContactList imContactListAsso = IMContactListHelper::getCopyOfAssociatedIMContacts(imContactList, account.getUUID());
		if (!imContactListAsso.empty()) {
			result.push_back(*imContactListAsso.begin());
			break;
		}

		// Look for an IMContact of the same protocol of the given Account
		IMContactList imContactListProto = IMContactListHelper::getCopyOfIMContactsOfProtocol(imContactList, account.getAccountType());
		if (!imContactListProto.empty()) {
			result.push_back(*imContactListProto.begin());
			break;
		}
	}

	return result;	
}

void PurpleChatSessionManager::initChatEvent() {
	void * handle = PurpleWrapper::getInstance().getHandle();

	purple_signal_connect(purple_conversations_get_handle(), "buddy-typing",
		handle, PURPLE_CALLBACK(PurpleChatSessionManager::updateBuddyTypingCbk), NULL);

	purple_signal_connect(purple_conversations_get_handle(), "buddy-typing-stopped",
		handle, PURPLE_CALLBACK(PurpleChatSessionManager::updateBuddyTypingCbk), NULL);

	purple_signal_connect(purple_conversations_get_handle(), "chat-invited",
		handle, PURPLE_CALLBACK(PurpleChatSessionManager::chatInviteRequestCbk), NULL);

	purple_signal_connect(purple_conversations_get_handle(), "chat-joined",
		handle, PURPLE_CALLBACK(PurpleChatSessionManager::chatJoinedCbk), NULL);

	purple_signal_connect(purple_conversations_get_handle(), "chat-left",
		handle, PURPLE_CALLBACK(PurpleChatSessionManager::chatJoinedCbk), NULL);

	purple_signal_connect(purple_conversations_get_handle(), "received-im-msg",
		handle, PURPLE_CALLBACK(PurpleChatSessionManager::receivedImMsgCbk), NULL);
}

int PurpleChatSessionManager::getPurpleConversationId(const char * name) {
	int id = 0;
	char * str_id = (char *) name;

	if (name == NULL) {
		return id;
	}

	while (str_id && *str_id && (*str_id > '9' || *str_id < '0')) {
		str_id++;
	}

	if (*str_id == '\0') {
		return id;
	}

	id = strtol(str_id, (char **) NULL, 10);
	return id;
}

void PurpleChatSessionManager::createIncomingConversationCbk(PurpleConversation * conv) {
	LOG_DEBUG("create incoming conversation");
	PurpleChatSessionManager::createConversation(conv, false);
}

void PurpleChatSessionManager::createConversationCbk(PurpleConversation * conv) {
	LOG_DEBUG("create conversation");
	PurpleChatSessionManager::createConversation(conv, true);
}

void PurpleChatSessionManager::createConversation(PurpleConversation * conv, bool userCreated) {
	// User created IChatSession are managed in PurpleChatSession::start
	if (!userCreated) {
		PurpleChatSession * chatSession = (PurpleChatSession *) _instance->createIChatSessionPlugin();
		chatSession->getConvInfo().purple_conv_session = conv;
		conv->ui_data = &(chatSession->getConvInfo());
		_instance->newIChatSessionPluginCreatedSignal(chatSession);
	}
}

void PurpleChatSessionManager::destroyConversationCbk(PurpleConversation * conv) {
	LOG_DEBUG("destroy conversation. wgconv : purple_wgconv_destroy()");
	// TODO: free (mConvInfo_t *) conv->ui_data;
}

void PurpleChatSessionManager::writeChatCbk(PurpleConversation * conv,
	const char * who, const char * message, PurpleMessageFlags flags, time_t mtime) {

	LOG_DEBUG("write chat");
	purple_conversation_write(conv, who, message, flags, mtime);
}

void PurpleChatSessionManager::writeIMCbk(PurpleConversation * conv,
	const char * who, const char * message, PurpleMessageFlags flags, time_t mtime) {

	LOG_DEBUG("write instant message");
	purple_conversation_write(conv, who, message, flags, mtime);
}

void PurpleChatSessionManager::writeConvCbk(PurpleConversation * conv,
	const char * name, const char * alias, const char * message,
	PurpleMessageFlags flags, time_t mtime) {

	LOG_DEBUG("write conversation");
	if ((flags & PURPLE_MESSAGE_RECV)) {
		// Prevent from double message during a chat session
		if (strcmp(purple_account_get_username(conv->account), name) == 0) {
			return;
		}

		// Looks for the IMContact in Contact list
		PurpleAccount *gAccount = purple_conversation_get_account(conv);
		EnumAccountType::AccountType accountType = PurpleIMPrcl::getEnumIMProtocol(gAccount);
		IMContact imContact(accountType, std::string(name));
		ContactList contactList = _instance->_coIpManager.getUserProfileManager().getCopyOfUserProfile().getContactList();
		ContactListHelper::getIMContact(contactList, imContact);
		////

		// TODO: A session could already exists
		if (conv->ui_data == NULL) {
			LOG_DEBUG("no Session exists for this LibPurple conversation. Creating one");
			Account *account = findAccount(gAccount);
			if (account) {
				PurpleChatSession * newChatSession = (PurpleChatSession *) _instance->createIChatSessionPlugin();
				newChatSession->setAccount(*account);
				OWSAFE_DELETE(account);
				newChatSession->addIMContactLocally(imContact);
				newChatSession->getConvInfo().purple_conv_session = conv;
				conv->ui_data = &(newChatSession->getConvInfo());
				_instance->newIChatSessionPluginCreatedSignal(newChatSession);
			} else {
				LOG_FATAL("Cannot find Account");
			}
		}

		PurpleChatSession * chatSession = (PurpleChatSession *) ((mConvInfo_t *) conv->ui_data)->conv_session;
		chatSession->messageReceivedSignal(imContact, std::string(message));
	}
}

void PurpleChatSessionManager::chatAddUsersCbk(PurpleConversation * conv,
	GList * users, gboolean new_arrivals) {

	LOG_DEBUG("add users");

	PurpleChatSession * chatSession = (PurpleChatSession *) ((mConvInfo_t *) conv->ui_data)->conv_session;
	PurpleAccount * gAccount = purple_conversation_get_account(conv);
	EnumAccountType::AccountType accountType = PurpleIMPrcl::getEnumIMProtocol(gAccount);

	for (GList * l = users; l != NULL; l = l->next) {
		if (strcmp(purple_account_get_username(gAccount), (char *) l->data) == 0) { //TODO: pbernery: added == 0. To be tested
			IMContact imContact(accountType, std::string((char *) l->data));

			if (IMContactListHelper::has(chatSession->getIMContactList(), imContact)) {
				LOG_ERROR("IMContact for " + imContact.getContactId() + " already in this IChatSession");
			} else {
				ContactList contactList = _instance->_coIpManager.getUserProfileManager().getCopyOfUserProfile().getContactList();
				ContactListHelper::getIMContact(contactList, imContact);
				chatSession->addIMContact(imContact);
			}
		}
	}
}

void PurpleChatSessionManager::chatRenameUserCbk(PurpleConversation * conv,
	const char * old_name, const char * new_name, const char * new_alias) {

	LOG_DEBUG("rename user");
}

void PurpleChatSessionManager::chatRemoveUsersCbk(PurpleConversation * conv, GList * users) {
	LOG_DEBUG("remove user");

	PurpleChatSession * chatSession = (PurpleChatSession *) ((mConvInfo_t *) conv->ui_data)->conv_session;
	PurpleAccount * gAccount = purple_conversation_get_account(conv);
	EnumAccountType::AccountType accountType = PurpleIMPrcl::getEnumIMProtocol(gAccount);

	for (GList * l = users; l != NULL; l = l->next) {
		if (strcmp(purple_account_get_username(gAccount), (char *) l->data) == 0) { //TODO: pbernery: added == 0. To be tested
			IMContact imContact(accountType, std::string((char *) l->data));
			chatSession->removeIMContact(imContact);
		}
	}
}

void PurpleChatSessionManager::chatUpdateUserCbk(PurpleConversation * conv, const char * user) {
	LOG_DEBUG("update user");
}

void PurpleChatSessionManager::presentConvCbk(PurpleConversation * conv) {
	LOG_DEBUG("present conversation");
}

gboolean PurpleChatSessionManager::hasFocusCbk(PurpleConversation * conv) {
	LOG_DEBUG("has focus");
	return TRUE;
}

gboolean PurpleChatSessionManager::customSmileyAddCbk(PurpleConversation * conv,
	const char * smile, gboolean remote) {

	LOG_DEBUG("add custom smiley");
	return TRUE;
}

void PurpleChatSessionManager::customSmileyWriteCbk(PurpleConversation * conv,
	const char * smile, const guchar * data, gsize size) {

	LOG_DEBUG("write custom smiley");
}

void PurpleChatSessionManager::customSmileyCloseCbk(PurpleConversation * conv, const char * smile) {
	LOG_DEBUG("close custom smiley");
}

void PurpleChatSessionManager::sendConfirm(PurpleConversation * conv, const char * message) {
	LOG_DEBUG("send confirm");
}

void PurpleChatSessionManager::chatJoinedCbk(PurpleConversation * conv) {
	LOG_DEBUG("chat joined");

	mConvInfo_t * mConv = (mConvInfo_t *) conv->ui_data;
	if (mConv->pending_invit) {
		for (GList * l = mConv->pending_invit; l != NULL; l = l->next) {
			serv_chat_invite(purple_conversation_get_gc(conv),
				purple_conv_chat_get_id(PURPLE_CONV_CHAT(conv)),
				"Join my conference...", (char *)l->data);
		}
	}
}

int PurpleChatSessionManager::chatInviteRequestCbk(PurpleAccount * account,
	const char * who, const char * message, void * data) {

	LOG_DEBUG("chat invite request");

	return 1;
}

void PurpleChatSessionManager::updateBuddyTypingCbk(PurpleAccount * account, const char * who) {
	PurpleConversation * gConv = NULL;

	LOG_DEBUG("update buddy typing");

	gConv = purple_find_conversation_with_account(PURPLE_CONV_TYPE_IM, who, account);
	if (!gConv) {
		return;
	}

	EnumChatTypingState::ChatTypingState state;
	switch (purple_conv_im_get_typing_state(PURPLE_CONV_IM(gConv))) {
	case PURPLE_TYPING:
		state = EnumChatTypingState::ChatTypingStateTyping;
		break;
	case PURPLE_TYPED:
		state = EnumChatTypingState::ChatTypingStateStopTyping;
		break;
	default:
		state = EnumChatTypingState::ChatTypingStateNotTyping;
		break;
	}

	PurpleChatSession * chatSession = (PurpleChatSession *) ((mConvInfo_t *) gConv->ui_data)->conv_session;
	PurpleAccount * gAccount = purple_conversation_get_account(gConv);
	EnumAccountType::AccountType accountType = PurpleIMPrcl::getEnumIMProtocol(gAccount);

	IMContact contact(accountType, purple_conversation_get_name(gConv));
	ContactList contactList = _instance->_coIpManager.getUserProfileManager().getCopyOfUserProfile().getContactList();
	ContactListHelper::getIMContact(contactList, contact);
	chatSession->typingStateChangedSignal(contact, state);
}

void PurpleChatSessionManager::receivedImMsgCbk(PurpleAccount * account, char * sender,
	char * message, PurpleConversation * conv, int flags) {

	LOG_DEBUG("received instant message");

	if (conv != NULL) {
		return;
	}

	purple_conversation_new(PURPLE_CONV_TYPE_IM, account, sender);
}

Account * PurpleChatSessionManager::findAccount(PurpleAccount * gAccount) {
	Account *result = NULL;
	std::string login = PurpleWrapper::cleanLogin(gAccount);
	AccountList accountList = _instance->_coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();

	EnumAccountType::AccountType accountType = PurpleIMPrcl::getEnumIMProtocol(gAccount);
	result = AccountListHelper::getCopyOfAccount(accountList, login, accountType);
	
	if (!result && (accountType == EnumAccountType::AccountTypeJabber)) {
		// Try with GTalk protocol.
		result = AccountListHelper::getCopyOfAccount(accountList, login, EnumAccountType::AccountTypeGTalk);
	}

	return result;
}
