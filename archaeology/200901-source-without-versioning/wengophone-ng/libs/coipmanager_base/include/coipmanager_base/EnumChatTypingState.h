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

#ifndef OWENUMCHATTYPINGSTATE_H
#define OWENUMCHATTYPINGSTATE_H

#include <coipmanager_base/coipmanagerbasedll.h>

#include <util/NonCopyable.h>

#include <string>

/**
 * Chat typing state.
 *
 * SipWrapper ONLY handles telephony and video!!!
 * TODO SipWrapper deals with the SIP protocol not with the SIP/SIMPLE protocol
 * chatMessageReceivedSignal() and others have to be removed from SipWrapper
 * SipWrapper is done for handling video and audio (SIP protocol)
 * not chat, presence, typing blabla (SIP/SIMPLE protocol), VNC...
 * This has to be separated: SipAudioVideoWrapper, SipChatWrapper, SipPresenceWrapper,
 * SipVNCWrapper...
 *
 * Gives infos during a chat session:
 * "peer is typing", "peer stopped typing"...
 *
 * @author Tanguy Krotoff
 */
class COIPMANAGER_BASE_API EnumChatTypingState : NonCopyable {
public:

	enum ChatTypingState {

		/** The contact is not typing. */
		ChatTypingStateNotTyping,

		/** The contact is typing. */
		ChatTypingStateTyping,

		/** The contact stopped typing. */
		ChatTypingStateStopTyping
	};

	/**
	 * Gets a human readable string of a ChatTypingState.
	 */
	static std::string toString(ChatTypingState state);
};

#endif	//OWENUMCHATTYPINGSTATE_H
