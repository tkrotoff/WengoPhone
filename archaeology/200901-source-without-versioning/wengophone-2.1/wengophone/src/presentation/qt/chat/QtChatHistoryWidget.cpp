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

#include "QtChatHistoryWidget.h"

#include <presentation/qt/chat/QtChatUtils.h>

#include <model/config/Config.h>
#include <model/config/ConfigManager.h>

#include <control/chat/CChatHandler.h>

#include <imwrapper/Account.h>

#include <util/Logger.h>
#include <util/WebBrowser.h>

#include <QtGui/QtGui>

QtChatHistoryWidget::QtChatHistoryWidget(QWidget * parent) : QTextBrowser(parent) {

	_lastBackGroundColor = QColor("#D0FFE6");

	setFrameStyle(QFrame::NoFrame | QFrame::Plain);
	setLineWrapMode(QTextEdit::WidgetWidth);
	setWordWrapMode(QTextOption::WrapAnywhere);
	connect (this, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(urlClicked(const QUrl &)));
	setHtml("<qt type=detail>");
}

QtChatHistoryWidget::~QtChatHistoryWidget() {
}

void QtChatHistoryWidget::setUserName(const QString & username) {
	_userColorHash[username] = CHAT_USER_BACKGOUND_COLOR;
}

void QtChatHistoryWidget::saveHistoryAsHtml() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	QString contentToSave = toHtml();

	QString filePath = QFileDialog::getSaveFileName(this, tr("Save As"), 
		QString::fromStdString(config.getLastChatHistorySaveDir()), "HTML (*.htm *.html)");

	if(filePath.length() > 0) {

		if(!filePath.endsWith(QString(".htm"), Qt::CaseInsensitive) && 
			!filePath.endsWith(QString(".html"), Qt::CaseInsensitive)) {

			filePath.append(QString(".html"));
		}

		QFile fileToSave(filePath);
		config.set(Config::LAST_CHAT_HISTORY_SAVE_DIR_KEY, QFileInfo(fileToSave).absolutePath().toStdString());
		fileToSave.open(QIODevice::WriteOnly);
		fileToSave.write(contentToSave.toStdString().c_str(), (long long)contentToSave.length());
		fileToSave.close();
	}
}

void QtChatHistoryWidget::urlClicked(const QUrl & link) {
	setSource(QUrl(QString::null));
	WebBrowser::openUrl(link.toString().toStdString());
	ensureCursorVisible();
}

void QtChatHistoryWidget::insertMessage(const QString & senderName, const QString & str, const QString & date) {
	QTextCursor curs(document());
	curs.movePosition(QTextCursor::End);
	setTextCursor(curs);

	if (!hasUserColor(senderName)) {
		_userColorHash[senderName] = getNewBackgroundColor();
	}

	//insert header
	insertHeader(senderName, date);
	////

	// insert message html code	
	QString table = QString("<table border=\"0\" width=\"98%\" cellspacing=\"0\" cellpadding=\"5\">") +
		QString("<tr><td>") +
		QtChatUtils::decodeMessage(_protocol, str) +
		QString("</td></tr></table>");
	QTextBrowser::insertHtml(table);
	////

	ensureCursorVisible();
}

void QtChatHistoryWidget::insertHeader(const QString & senderName, const QString & date) {
	QString header;
	if( date.isEmpty() ) {
		// insert "header" code html
		header = QtChatUtils::getHeader(getUserColor(senderName), CHAT_USER_FOREGROUND_COLOR, senderName);
	} else {
		// insert "header" code html for history messages
		header = QtChatUtils::getDatedHeader(getUserColor(senderName), CHAT_HISTORY_FOREGROUND_COLOR, senderName, date);
	}
	QTextBrowser::insertHtml(header);
}

void QtChatHistoryWidget::insertStatusMessage(const QString & statusMessage) {
	QString message = QtChatUtils::getHeader(CHAT_USER_BACKGOUND_COLOR,
		"#888888", "<i>" + statusMessage + "</i>");
	QTextBrowser::insertHtml(message);
}

bool QtChatHistoryWidget::hasUserColor(const QString & nickName) const {
	return (_userColorHash.find(nickName) != _userColorHash.end());
}
	
QString QtChatHistoryWidget::getNewBackgroundColor() const {
	_lastBackGroundColor.setRed(_lastBackGroundColor.red() + 20);
	return QString("%1").arg(_lastBackGroundColor.name());
}
