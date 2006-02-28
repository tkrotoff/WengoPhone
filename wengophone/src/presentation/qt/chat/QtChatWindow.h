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

#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QtGui>
#include <string>
#include <WidgetFactory.h>
#include "widgetseeker.h"
#include "QtChatWidget.h"
#include <imwrapper/IMContact.h>
#include <imwrapper/IMChatSession.h>
#include <QObjectThreadSafe.h>
// class IMChatSession;
// class IMContact;

class ChatWindow : public QObjectThreadSafe
{

    Q_OBJECT

public:
    
    ChatWindow(IMChatSession & imChatSession);
    
    
protected:
    QWidget     *   _widget; 
    WidgetSeeker    _seeker;
    ChatWidget  *   _chatWidget;
    QTabWidget  *   _tabWidget;
    
    IMChatSession & _imChatSession;
    QDialog			_dialog;
    
public Q_SLOTS:
    void    newMessage(const QString & msg);
private:
	void initThreadSafe();
    void messageReceivedEventHandler(IMChatSession & sender, const IMContact & from, const std::string & message);
    void messageReceivedEventHandlerThreadSafe(IMChatSession & sender, const IMContact & from, const std::string  message);
};

#endif
