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

#include <presentation/qt/chat/QtChatTheme.h>
#include <presentation/qt/chat/QtChatUtils.h>

#include <model/config/Config.h>
#include <model/config/ConfigManager.h>

#include <control/chat/CChatHandler.h>

#include <imwrapper/Account.h>

#include <util/Logger.h>
#include <util/WebBrowser.h>

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTime>

#include <QtGui/QFileDialog>

QtChatHistoryWidget::QtChatHistoryWidget(QWidget * parent) : QTextBrowser(parent) {
	setFrameStyle(QFrame::NoFrame | QFrame::Plain);
	setLineWrapMode(QTextEdit::WidgetWidth);
	connect (this, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(urlClicked(const QUrl &)));

	_theme = new QtChatTheme();
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string themeDir = config.getResourcesDir()
		+ "/chat/"
		+ config.getStringKeyValue("chat.theme")
		+ "/";
	_theme->load(QString::fromUtf8(themeDir.c_str()));

	QString css = _theme->getStyleSheet();
	document()->setDefaultStyleSheet(css);
}

QtChatHistoryWidget::~QtChatHistoryWidget() {
	delete _theme;
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

void QtChatHistoryWidget::insertMessage(const QString & contactId, const QString & senderName, const QString & message, const QTime & time) {
	QTextCursor curs(document());
	curs.movePosition(QTextCursor::End);
	setTextCursor(curs);

	// Prepare message
	QString htmlMessage = QtChatUtils::decodeMessage(_protocol, message);

	// Get html
	QtChatTheme::Position position;
	if (_lastSenderName == senderName) {
		position = QtChatTheme::Next;
	} else {
		if (!_lastSenderName.isEmpty()) {
			insertHtml(_theme->getFooter());
		}
		position = QtChatTheme::First;
		_lastSenderName = senderName;
	}

	QtChatTheme::Direction direction;
	if (contactId == "self") { // FIXME: do not duplicate "self"
		direction = QtChatTheme::Outgoing;
	} else {
		direction = QtChatTheme::Incoming;
	}

	QString html = _theme->getContent(direction, position);
	QtChatTheme::setKeywordValue(html, "senderId", contactId);
	QtChatTheme::setKeywordValue(html, "sender", senderName);
	QtChatTheme::setKeywordValue(html, "time", time.toString("HH:mm"));
	QtChatTheme::setKeywordValue(html, "message", htmlMessage);
	insertHtml(html);

	// Scroll to bottom
	ensureCursorVisible();
}

void QtChatHistoryWidget::insertStatusMessage(const QString & message, const QTime& time) {
	// Make sure we close any opened message group
	if (!_lastSenderName.isEmpty()) {
		insertHtml(_theme->getFooter());
		_lastSenderName = "";
	}
	QString html = _theme->getStatusMessage();
	QtChatTheme::setKeywordValue(html, "message", message);
	QtChatTheme::setKeywordValue(html, "time", time.toString("HH:mm"));
	insertHtml(html);
	ensureCursorVisible();
}

void QtChatHistoryWidget::setAvatarPixmap(const QString& name, const QPixmap& pixmap) {
	_avatarMap[name] = pixmap;
}

QVariant QtChatHistoryWidget::loadResource(int type, const QUrl& url) {
	if (type != QTextDocument::ImageResource || url.scheme() != "avatar") {
		return QTextBrowser::loadResource(type, url);
	}

	QString userId = url.path();
	QPixmap pixmap = _avatarMap[userId];

	if (!pixmap.isNull()) {
		QString sizeString = url.queryItemValue("size");
		bool ok;
		int size = sizeString.toInt(&ok);
		if (ok) {
			if (_scaledAvatarCache.contains(url)) {
				pixmap = _scaledAvatarCache[url];
			} else {
				pixmap = pixmap.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
				_scaledAvatarCache[url] = pixmap;
			}
		}
	}

	return QVariant(pixmap);
}
