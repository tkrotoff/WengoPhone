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

#include "QtChatHandler.h"
#include <presentation/qt/toaster/QtToaster.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <control/chat/CChatHandler.h>
#include <control/CWengoPhone.h>
#include <util/Logger.h>

#include <QWidget>

QtChatHandler::QtChatHandler(CChatHandler & cChatHandler)
	: QObjectThreadSafe(NULL),
	_cChatHandler(cChatHandler) {

	_qtChatWidget = NULL;
}

QtChatHandler::~QtChatHandler() {
}

void QtChatHandler::newIMChatSessionCreatedEventHandler(IMChatSession & imChatSession) {
	typedef PostEvent1<void (IMChatSession & imChatSession), IMChatSession &> MyPostEvent;
	MyPostEvent * event =
		new MyPostEvent(boost::bind(&QtChatHandler::newIMChatSessionCreatedEventHandlerThreadSafe, this, _1), imChatSession);
	postEvent(event);
}

void QtChatHandler::newIMChatSessionCreatedEventHandlerThreadSafe(IMChatSession & imChatSession) {

    Config & config = ConfigManager::getInstance().getCurrentConfig();

	if (!_qtChatWidget)
	{
		_qtChatWidget =  new ChatWindow(_cChatHandler, imChatSession);
	}
	else
	{
		_qtChatWidget->addChatSession(&imChatSession);
	}

    showToaster(imChatSession);
}

void QtChatHandler::createSession(IMAccount & imAccount, IMContactSet & imContactSet) {
	_cChatHandler.createSession(imAccount, imContactSet);
}

void QtChatHandler::updatePresentation() {

}

void QtChatHandler::updatePresentationThreadSafe() {

}

void QtChatHandler::initThreadSafe() {
	_qtChatWidget = NULL;
}
void QtChatHandler::showToaster(IMChatSession & imChatSession) {

    Config & config = ConfigManager::getInstance().getCurrentConfig();

    if (_qtChatWidget->isVisible())
        return;
	if (imChatSession.isUserCreated())
		return;
    // Shows toaster for incoming incoming chats ?
    if (!config.getNotificationShowToasterOnIncomingCall())
        return;

    QPixmap result;

    ContactList & contactList = _cChatHandler.getUserProfile().getContactList();

	QPixmap background = QPixmap(":/pics/fond_avatar.png");

	QtToaster  * toaster = new QtToaster();
	toaster->setTitle(tr("New chat session"));
	if (imChatSession.getIMContactSet().size() > 0) {
		QString message;
		for (IMContactSet::const_iterator it = imChatSession.getIMContactSet().begin();
			it != imChatSession.getIMContactSet().end();
			++it) {
			if (it != imChatSession.getIMContactSet().begin()) {
				message += ", ";
			}
			message += QString::fromStdString((*it).getContactId());

			Contact * contact = contactList.findContactThatOwns((*it));
			Picture picture = contact->getIcon();
            std::string data = picture.getData();
            if ( !data.empty()) {
                result.loadFromData((uchar *) data.c_str(), data.size());
            }
		}
		toaster->setMessage(message);
	}
	toaster->hideButton(2); toaster->hideButton(3);
    if (!result.isNull()) {

        QPainter pixpainter(& background);
        pixpainter.drawPixmap(5, 5, result.scaled(60, 60));
        pixpainter.end();
    }
	else {
	    result = QPixmap(":pics/toaster/chat.png");

        QPainter pixpainter(& background);
        pixpainter.drawPixmap(5, 5, result.scaled(60, 60));
        pixpainter.end();
	}
    toaster->setPixmap(background);
    toaster->setButton1Pixmap(QPixmap(":pics/toaster/chat.png"));
	connect(toaster,SIGNAL(button1Clicked()),_qtChatWidget,SLOT(show()));
	toaster->showToaster();
}
