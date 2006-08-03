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

#ifndef OWQTCHATCONTACTWIDGET_H
#define OWQTCHATCONTACTWIDGET_H

#include <QtGui>

#include <model/profile/UserProfile.h>

#include <imwrapper/IMContact.h>
#include <imwrapper/IMChatSession.h>
#include <imwrapper/IMChat.h>
#include <control/chat/CChatHandler.h>

/**
 *
 * @ingroup presentation
 * @author Mr K.
 */
class QtChatContactWidget : public QLabel
{
	Q_OBJECT

public:

	QtChatContactWidget(IMChatSession * imChatSession, CChatHandler & cChatHandler,
						QWidget * parent = 0, Qt::WFlags f = 0 );

	int getId();

public Q_SLOTS:

	void updateDisplay();

private:

	IMChatSession * _imChatSession;

	CChatHandler & _cChatHandler;

	QPixmap _display;

	void drawBorder(QPainter * painter, QRect rect, const QColor & color);

	void createGroupPixmap(int width);

	void fillDisplay(QList <QPixmap> pixList);

	void setDisplayName(QString displayName, QPixmap * pixmap);

	QPixmap createContactPixmap(const QString & displayName,QPixmap pixmap );

	QMutex _mutex;

};

#endif //OWQTCHATCONTACTWIDGET_H
