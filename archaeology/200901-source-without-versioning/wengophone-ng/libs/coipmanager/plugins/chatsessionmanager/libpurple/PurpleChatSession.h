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

#ifndef OWPURPLECHATSESSION_H
#define OWPURPLECHATSESSION_H

#include "PurpleChatSessionManager.h"

#include <coipmanager/chatsessionmanager/IChatSessionPlugin.h>

extern "C" {
#include <glib.h>
}

typedef struct mConvInfo_s {
	void * conv_session;
	int conv_id;
	void * purple_conv_session;
	GList * pending_invit;
} mConvInfo_t;

/**
 * IChatSession LibPurple implementation.
 *
 * @author Philippe Bernery
 * @author Julien Bossart
 */
class PurpleChatSession : public IChatSessionPlugin {
	//PurpleChatSessionManager is friend with PurpleChatSession
	//so that PurpleChatSessionManager can emit signals from ChatSession
	friend class PurpleChatSessionManager;
public:

	PurpleChatSession(CoIpManager & coIpManager);

	virtual ~PurpleChatSession();

	virtual void sendMessage(const std::string & message);

	virtual void setTypingState(EnumChatTypingState::ChatTypingState state);

	virtual void start();

	virtual void pause();

	virtual void resume();

	virtual void stop();

	virtual void addIMContact(const IMContact & imContact);

	virtual void removeIMContact(const IMContact & imContact);

	virtual void setIMContactList(const IMContactList & imContactList);

	mConvInfo_t & getConvInfo() { return _convInfo; }

	static bool addIMContactCbk(void * data);
	static bool removeIMContactCbk(void * data);
	static bool setTypingStateCbk(void * data);
	static bool sendMessageCbk(void * data);
	static bool startCbk(void * data);
	static bool stopCbk(void * data);

private:

	static void createPurpleChat(GList * users, mConvInfo_t * convInfo,
		EnumAccountType::AccountType accountType);

	static QMutex * _mutex;
	
	mConvInfo_t _convInfo;
};

#endif	//OWPURPLECHATSESSION_H
