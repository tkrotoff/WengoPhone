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

#ifndef QTCHATWIDGET_H
#define QTCHATWIDGET_H

#include <QObject>

#include <string>

class IMChatSession;
class IMContact;
class QListWidget;
class QWidget;
class QLineEdit;
class QPushButton;

/**
 *
 * @ingroup presentation
 * @author Philippe Bernery
 */
class QtChatWidget : public QObject {
	Q_OBJECT
public:

	QtChatWidget(IMChatSession & imChatSession);

	~QtChatWidget();

	QWidget * getWidget() const {
		return _chatWidget;
	}

private Q_SLOTS:

	void sendMessage();

private:

	void messageReceivedEventHandler(IMChatSession & sender, const IMContact & from, const std::string & message);

	void initThreadSafe();

	QWidget * _chatWidget;

	QLineEdit * _lineEdit;

	QListWidget * _listWidget;

	QPushButton * _sendButton;

	IMChatSession & _imChatSession;

};

#endif	//QTCHATWIDGET_H
