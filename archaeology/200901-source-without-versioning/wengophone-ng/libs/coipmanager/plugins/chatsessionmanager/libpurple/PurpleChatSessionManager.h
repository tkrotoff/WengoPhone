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

#ifndef OWPURPLECHATSESSIONMANAGER_H
#define OWPURPLECHATSESSIONMANAGER_H

#include <coipmanager/chatsessionmanager/IChatSessionManagerPlugin.h>

#include <vector>

extern "C" {
#include <libpurple/connection.h>
#include <libpurple/conversation.h>
}

class PurpleChatSession;

/**
 * LibPurple implementation of Chat Session Manager.
 *
 * @author Philippe Bernery
 * @author Julien Bossart
 */
class PurpleChatSessionManager : public IChatSessionManagerPlugin {
public:

	PurpleChatSessionManager(CoIpManager & coIpManager);

	virtual ~PurpleChatSessionManager();

	virtual void initialize();

	virtual IChatSessionPlugin * createIChatSessionPlugin();

	virtual bool canCreateISession(const Account & account, const ContactList & contactList) const;

	virtual IMContactList getValidIMContacts(const Account & account, const ContactList & contactList) const;

	/**
	 * @name Purple conversation callbacks
	 * @{
	 */

	static void createIncomingConversationCbk(PurpleConversation * conv);
	static void createConversationCbk(PurpleConversation * conv);
	static void destroyConversationCbk(PurpleConversation * conv);
	static void writeChatCbk(PurpleConversation * conv,
		const char * who, const char * message, PurpleMessageFlags flags, time_t mtime);
	static void writeIMCbk(PurpleConversation * conv,
		const char * who, const char * message, PurpleMessageFlags flags, time_t mtime);
	static void writeConvCbk(PurpleConversation * conv,
		const char * name, const char * alias, const char * message,
		PurpleMessageFlags flags, time_t mtime);
	static void chatAddUsersCbk(PurpleConversation * conv,
		GList * users, gboolean new_arrivals);
	static void chatRenameUserCbk(PurpleConversation * conv,
		const char * old_name, const char * new_name, const char * new_alias);
	static void chatRemoveUsersCbk(PurpleConversation * conv, GList * users);
	static void chatUpdateUserCbk(PurpleConversation * conv, const char * user);
	static void presentConvCbk(PurpleConversation * conv);
	static gboolean hasFocusCbk(PurpleConversation * conv);
	static gboolean customSmileyAddCbk(PurpleConversation * conv,
		const char * smile, gboolean remote);
	static void customSmileyWriteCbk(PurpleConversation * conv,
		const char * smile, const guchar * data, gsize size);
	static void customSmileyCloseCbk(PurpleConversation * conv, const char * smile);
	static void sendConfirm(PurpleConversation * conv, const char * message);

	static void chatJoinedCbk(PurpleConversation * conv);
	static int chatInviteRequestCbk(PurpleAccount * account,
		const char * who, const char * message, void * data);
	static void updateBuddyTypingCbk(PurpleAccount * account, const char * who);
	static void receivedImMsgCbk(PurpleAccount * account, char * sender, char * message,
		PurpleConversation * conv, int flags);

	/**
	 * @}
	 */

private:

	static int getPurpleConversationId(const char * name);

	static void createConversation(PurpleConversation * conv, bool userCreated);

	static Account * findAccount(PurpleAccount * account);

	void initChatEvent();

	static PurpleChatSessionManager * _instance;
};

#endif	//OWPURPLECHATSESSIONMANAGER_H
