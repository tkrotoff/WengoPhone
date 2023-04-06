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

#ifndef OWQTCHATHISTORYWIDGET_H
#define OWQTCHATHISTORYWIDGET_H

#include <imwrapper/EnumIMProtocol.h>

#include <QtGui/QtGui>

class QPoint;
class QAction;
class QWidget;

static const QString CHAT_USER_BACKGOUND_COLOR = "#F0EFFF";
static const QString CHAT_USER_FOREGROUND_COLOR = "#000000";
static const QString CHAT_HISTORY_FOREGROUND_COLOR = "#000000";//"#505050";

/**
 * Implements the chat text zone with a custom context menu.
 *
 * @author Nicolas Couturier
 * @author Mathieu Stute
 * @author Xavier Desjardins
 */
class QtChatHistoryWidget : public QTextBrowser {
	Q_OBJECT
public:

	QtChatHistoryWidget(QWidget * parent);
	
	~QtChatHistoryWidget();
	
	void insertMessage(const QString & senderName, const QString & str, const QString & date = "");
	
	void insertHeader(const QString & senderName, const QString & date);
	
	void insertStatusMessage(const QString & statusMessage);
	
	void setUserName(const QString & username);
	
	void setProtocol(EnumIMProtocol::IMProtocol newProtocol) {
		_protocol = newProtocol;
	}

	void saveHistoryAsHtml();

private Q_SLOTS:

	/**
	 * An URL has been clicked we must open a web browser
	 *
	 * @param link the clicked url
	 */
	void urlClicked(const QUrl & link);
	
private:

	typedef QHash<QString, QString> UserColorHash;
	
	QString getUserColor(const QString & nickName) const {
		return _userColorHash[nickName];
	}
	
	bool hasUserColor(const QString & nickname) const;
	
	QString getNewBackgroundColor() const;
	
	UserColorHash _userColorHash;
	
	mutable QColor _lastBackGroundColor;
	
	EnumIMProtocol::IMProtocol _protocol;
};

#endif	//OWQTCHATHISTORYWIDGET_H
