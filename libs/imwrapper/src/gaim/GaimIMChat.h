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

//#include "GaimIMFactory.h"

#include <imwrapper/IMAccount.h>
#include <imwrapper/IMChat.h>
#include <imwrapper/IMChatSession.h>

typedef struct	mConvInfo_s
{
	void		*conv_session;
	int			conv_id;
	void		*gaim_conv_session;
}				mConvInfo_t;

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
	void createSession();
	void closeSession(IMChatSession & chatSession);
	void sendMessage(IMChatSession & chatSession, const std::string & message);
	void addContact(IMChatSession & chatSession, const std::string & contactId);
	void removeContact(IMChatSession & chatSession, const std::string & contactId);
	bool equalsTo(std::string login, EnumIMProtocol::IMProtocol protocol);

	mConvInfo_t *mCreateSession();

private:

	/* ************ MANAGE CHAT SESSION LIST ************ */
	static std::list<mConvInfo_t *> _GaimChatSessionList;
	typedef std::list<mConvInfo_t *>::iterator GaimChatSessionIterator;
	void AddChatSessionInList(mConvInfo_t *conv);
	void RemoveChatSessionFromList(int convId);
	bool IsChatSessionInList(int convId);
	static mConvInfo_t *FindChatStructById(int convId);
	/* ************************************************** */
};

#endif	//GAIMIMCHAT_H
