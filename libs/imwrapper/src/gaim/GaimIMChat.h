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

#ifndef GAIMIMCHAT_H
#define GAIMIMCHAT_H

#include <imwrapper/IMChat.h>

class IMAccount;
class IMChatSession;
class IMContactSet;

extern "C" {
#include <gaim/connection.h>
}

typedef struct mConvInfo_s
{
	void *conv_session;
	int conv_id;
	void *gaim_conv_session;
	GList *pending_invit;
} mConvInfo_t;

/**
 * Gaim IM chat.
 *
 * @ingroup model
 * @author Julien Bossart
 */
class GaimIMChat : public IMChat
{
	friend class GaimIMFactory;

public:

	GaimIMChat(IMAccount &account);

	mConvInfo_t *CreateChatSession(bool userCreated);

	virtual void createSession(IMContactSet & imContactList);
	virtual void closeSession(IMChatSession & chatSession);
	virtual void sendMessage(IMChatSession & chatSession, const std::string & message);
	virtual void changeTypingState(IMChatSession & chatSession, TypingState state);
	virtual void addContact(IMChatSession & chatSession, const std::string & contactId);
	virtual void removeContact(IMChatSession & chatSession, const std::string & contactId);

	bool equalsTo(std::string login, EnumIMProtocol::IMProtocol protocol);
	void AddChatSessionInList(mConvInfo_t *conv);
	static mConvInfo_t *FindChatStructById(int convId);

	void createGaimChat(GaimConnection *gGC, int id, GList *users);

private:

	/* ************ MANAGE CHAT SESSION LIST ************ */
	static std::list<mConvInfo_t *> _GaimChatSessionList;
	typedef std::list<mConvInfo_t *>::iterator GaimChatSessionIterator;
	void RemoveChatSessionFromList(int convId);
	bool IsChatSessionInList(int convId);
	/* ************************************************** */
};

#endif	//GAIMIMCHAT_H
