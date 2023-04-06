/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#ifndef CONVERSATIONSIPSTATE_H
#define CONVERSATIONSIPSTATE_H

#include "SipState.h"

class CallLog;
class PhonePageWidget;
class Sound;

/**
 * Conversation state of the state machine.
 *
 * The two users are in an audio communication using SIP.
 *
 * Design Pattern State.
 *
 * @author Tanguy Krotoff
 */
class ConversationSipState : public SipState {
	Q_OBJECT
public:

	ConversationSipState(AudioCall & audioCall, CallLog & callLog);

	virtual ~ConversationSipState();

	virtual void handle(int callId, const CallStateInfo * info) const;

private slots:

	/**
	 * The user clicked on the close button in order to finish
	 * the current conversation.
	 */
	void closeCall();

private:

	ConversationSipState();
	ConversationSipState(const ConversationSipState &);
	ConversationSipState & operator=(const ConversationSipState &);

	CallLog & _callLog;

	static const QString SOUND_OCCUPIED;

	Sound * _soundOccupied;
};

#endif	//CONVERSATIONSIPSTATE_H
