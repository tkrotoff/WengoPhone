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

#ifndef OWPHAPICHATWRAPPER_H
#define OWPHAPICHATWRAPPER_H

#include <phapiwrapperdll.h>

#include <sipwrapper/SipChatWrapper.h>

/**
 * Chat part of PhApiChatWrapper.
 *
 * @author Philippe Bernery
 */
class PHAPIWRAPPER_API PhApiChatWrapper : public SipChatWrapper {
public:

	PhApiChatWrapper();

	~PhApiChatWrapper();

	virtual void sendMessage(int lineId, const std::string &contactId, const std::string &msg);

	virtual void sendTypingState(int lineId, const std::string &contactId, EnumChatTypingState::ChatTypingState typingState);

};

#endif //OWPHAPICHATWRAPPER_H
