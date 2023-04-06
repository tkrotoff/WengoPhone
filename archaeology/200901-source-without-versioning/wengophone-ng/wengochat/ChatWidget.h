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

#ifndef OWCHATWIDGET_H
#define OWCHATWIDGET_H

#include <coipmanager_base/contact/Contact.h>
#include <coipmanager/chatsessionmanager/EnumChatStatusMessage.h>
#include <coipmanager/chatsessionmanager/IChatSession.h>

#include <QtGui/QWidget>

#include <string>

class KeyPressEventFilter;
class TChatSession;
class WengoChat;

namespace Ui { class ChatWidget; }

/**
 *
 * @author Philippe Bernery
 */
class ChatWidget : public QWidget {
	Q_OBJECT
public:

	ChatWidget(WengoChat * wengoChat, TChatSession * session);

	~ChatWidget();

private Q_SLOTS:

	void sendClickedSlot();

	bool keyPressed(QEvent * event);

	void messageAddedSlot();

	void statusMessageReceivedSlot(EnumChatStatusMessage::ChatStatusMessage status, std::string message);

	void typingStateChangedSlot(Contact contact, EnumChatTypingState::ChatTypingState state);

private:

	TChatSession * _session;

	Ui::ChatWidget * _ui;

	KeyPressEventFilter * _keyEventFilter;

	WengoChat * _wengoChat;
};

#endif	//OWCHATWIDGET_H
