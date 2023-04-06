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

#include "ChatPageWidget.h"
#include "EmoticonsWidget.h"

#include "MyWidgetFactory.h"
#include "sip/SipPrimitives.h"
#include "sip/SipAddress.h"
#include "Softphone.h"
#include "login/User.h"
#include "contact/Contact.h"
#include "util/EventFilter.h"
#include "SessionWindow.h"

#include <qwidget.h>
#include <qsplitter.h>
#include <qdatetime.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qvaluelist.h>
#include <qwidgetfactory.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qfontdialog.h>
#include <qfont.h>
#include <qcolordialog.h>
#include <qcolor.h>
#include <qdialog.h>
#include <qdir.h>
#include <qcombobox.h>
#include <qaction.h>
#include <qkeysequence.h>
#include <qpoint.h>

#include <iostream>
using namespace std;

const int ChatPageWidget::MAX_MESSAGE_LENGTH = 2000;

static const QString MESSAGE_NEWLINE = "\n     ";

ChatPageWidget::ChatPageWidget(QWidget * parent)
	: QObject(parent) {
	_searchPath = "emoticons";
	_chatPageWidget = MyWidgetFactory::create("ChatPageWidgetForm.ui", this, parent);

	//ChatHistoryText
	_chatHistoryText = (QTextEdit *) _chatPageWidget->child("chatHistoryText", "QTextEdit");
	connect(_chatHistoryText, SIGNAL(doubleClicked(int, int)),
		this, SLOT(doubleClicked(int, int)));

	//MessageText
	_messageText = (QTextEdit *) _chatPageWidget->child("messageText", "QTextEdit");
	connect(_messageText, SIGNAL(textChanged()),
			this, SLOT(messageTextChanged()));
	_messageText->setFocus();

	_emoticonSetChooser = (QComboBox*)_chatPageWidget->child("emoticonSetChooser", "QComboBox");
	_emoticonSetChooser->hide();
	updateIconsetChooser();

	//Return key press filter
	ReturnKeyEventFilter * returnKeyEventFilter = new ReturnKeyEventFilter(this, SLOT(sendMessage()));
	_messageText->installEventFilter(returnKeyEventFilter);

	//QSplitter
	QSplitter * splitter = (QSplitter *) _chatPageWidget->child("splitter", "QSplitter");
	QValueList<int> sizes = splitter->sizes();
	sizes[0] = 350;
	sizes[1] = 50;
	splitter->setSizes(sizes);

	//Send button
	QPushButton * sendButton = (QPushButton *) _chatPageWidget->child("sendButton", "QPushButton");
	connect(sendButton, SIGNAL(clicked()),
			this, SLOT(sendMessage()));


	_emoticonsWidget = new EmoticonsWidget(this);
	_emoticonsWidget->setIconset("default");
	_iconset = _emoticonsWidget->getIconset();
	_emoticonsWidget->update();
/*
	connect(_emoticonSetChooser, SIGNAL(activated ( const QString & )), _emoticonsWidget, SLOT(setIconset(const QString&)));
	connect(_emoticonSetChooser, SIGNAL(activated ( const QString & )), SLOT(setIconset(const QString&)));
*/

	QMimeSourceFactory * m = _emoticonsWidget->getMimeSourceFactory();
	_messageText->setMimeSourceFactory(m);
	_chatHistoryText->setMimeSourceFactory(m);

	boldButton = (QPushButton*)_chatPageWidget->child("boldButton", "QPushButton");
	italicButton = (QPushButton*)_chatPageWidget->child("italicButton", "QPushButton");
	underlineButton = (QPushButton*)_chatPageWidget->child("underlineButton", "QPushButton");
	smileysButton = (QPushButton*)_chatPageWidget->child("smileysButton", "QPushButton");
	fontButton = (QPushButton*)_chatPageWidget->child("fontButton", "QPushButton");
	fontColorButton = (QPushButton*)_chatPageWidget->child("fontColorButton", "QPushButton");
	connect(boldButton, SIGNAL(clicked()), SLOT(setBold()));
	connect(italicButton, SIGNAL(clicked()), SLOT(setItalic()));
	connect(underlineButton, SIGNAL(clicked()), SLOT(setUnderline()));
	connect(fontButton, SIGNAL(clicked()), SLOT(setFont()));
	connect(fontColorButton, SIGNAL(clicked()), SLOT(setFontColor()));
	connect(smileysButton, SIGNAL(clicked()), SLOT(showEmoticons()));
}

void ChatPageWidget::appendMessageToHistory(const QString & message) {
	assert(_contact != NULL && "Contact cannot be null");
	appendMessageToHistory(message, "<b>" + _contact->toString() + "</b>", QString::null);
}

void ChatPageWidget::appendMessageToHistory(const QString & message, const QString & author, const QString & fontColor) const {
	static const QString space("<br>");

	QString temp = message;
	QPtrListIterator<Icon> it = _iconset.iterator();
	Icon *icon;
	while ( (icon = it.current()) != 0 ) {
		++it;
		temp.replace(icon->regExp(), "<img src=" + icon->name() + ">");
	}

	//QString text("<font color=grey>" + author + " (" + QTime::currentTime().toString() + ")</font>");
	QString text("<table border=0 width=100% cellspacing=0 cellpadding=0><tr><td><font color=#9ebde2>" + author
			+ "</font></td><td align=right><font color=#9ebde2>" + QTime::currentTime().toString()
			+ "</font></td></tr></table>");

	QString tmp(QString::fromUtf8(temp));
	//Replace \n by <br>
	//tmp.replace("\n", space);

	//Adds a space column to the message, equivalent to a <blockquote></blockquote>
	//text += "<table border=0 cellspacing=0 cellpadding=0><tr><td></td><td>" + tmp + "</td></tr><tr><td></td></tr></table>";
	text += "<table border=0 cellspacing=0 cellpadding=0><tr><td></td><td>" + tmp + "</td></tr></table>";

	text.replace("\n", "");

	/*Does not work properly since Qt adds a new line when using QTextEdit::append()
	_chatHistoryText->append(text);*/
	_chatHistoryText->setText(_chatHistoryText->text() + text);
	_chatHistoryText->scrollToBottom();
}

void ChatPageWidget::doubleClicked(int para, int pos) {
	QString line = _chatHistoryText->text(para);

	if (line.contains("http://") ||
		line.contains("www.") ||
		line.contains("ftp://")) {

		cout << line << endl;
	}
}

void ChatPageWidget::appendLocalMessageToHistory(const QString & message, const QString & author, const QString & fontColor) const {
	static const QString space("<br>");

	//QString text("<font color=grey>" + author + " (" + QTime::currentTime().toString() + ")</font>");
	QString text("<table border=0 width=100% cellspacing=0 cellpadding=0><tr><td><font color=#9ebde2>" + author
			+ "</font></td><td align=right><font color=#9ebde2>" + QTime::currentTime().toString()
			+ "</font></td></tr></table>");

	QString tmp(QString::fromUtf8(message));
	//Replace \n by <br>
	//tmp.replace("\n", space);

	//Adds a space column to the message, equivalent to a <blockquote></blockquote>
	//text += "<table border=0 cellspacing=0 cellpadding=0><tr><td></td><td>" + tmp + "</td></tr><tr><td></td></tr></table>";
	text += "<table border=0 cellspacing=0 cellpadding=0><tr><td></td><td>" + tmp + "</td></tr></table>";

	text.replace("\n", "");

	/*Does not work properly since Qt adds a new line when using QTextEdit::append()
	_chatHistoryText->append(text);*/
	_chatHistoryText->setText(_chatHistoryText->text() + text);
	_chatHistoryText->scrollToBottom();
}


void ChatPageWidget::setContact(const Contact & contact) {
	_contact = &contact;
}

void ChatPageWidget::sendMessage() const {
	assert(_contact != NULL && "Contact cannot be null");

	QString message(_messageText->text());
	if (!message.isEmpty()) {

		//Appends the message to the history
		appendLocalMessageToHistory(message.utf8(), Softphone::getInstance().getUser().getLogin(), "grey");

		QPtrListIterator<Icon> it = _iconset.iterator();
		Icon *icon;
		while ( (icon = it.current()) != 0 ) {
			++it;
			message.replace("<img src=" + icon->name() + " >", EmoticonsWidget::getFirstShortcut(icon->regExp()) );
		}
		
		QString wengophone = _contact->getWengoPhone();
		if(wengophone != "") {
			SipPrimitives::sendMessage(SipAddress::fromPhoneNumber(wengophone), message.utf8());
		}
	}
	
	//Resets the message
	_messageText->clear();
}

void ChatPageWidget::messageTextChanged() {
	int para, index;
	_messageText->getCursorPosition(&para, &index);
	QString message = _messageText->text();

	QPtrListIterator<Icon> it = _iconset.iterator();
	Icon *icon;
	while ( (icon = it.current()) != 0 ) {
		++it;
		message.replace(icon->regExp(), "<img src=" + icon->name() + ">");
	}

	if( message != _messageText->text() ) {
		_messageText->disconnect(SIGNAL(textChanged()));
		_messageText->setText(message);
		connect(_messageText, SIGNAL(textChanged()),
			this, SLOT(messageTextChanged()));
		_messageText->setCursorPosition(para, index);
	}

	if (_messageText->length() > MAX_MESSAGE_LENGTH && _messageText->isVisible()) {
		_messageText->doKeyboardAction(QTextEdit::ActionBackspace);
	}
}

void ChatPageWidget::reset() {
	_chatHistoryText->clear();
	_messageText->clear();
}

void ChatPageWidget::setFocus() {
	_messageText->setFocus();
}

void ChatPageWidget::setBold() {
	_messageText->setBold(!_messageText->bold());
}

void ChatPageWidget::setItalic() {
	_messageText->setItalic(!_messageText->italic());
}

void ChatPageWidget::setUnderline() {
	_messageText->setUnderline(!_messageText->underline());
}

void ChatPageWidget::setFont() {
	bool ok;
	QFont font;
	font = QFontDialog::getFont ( &ok, getWidget());
	if(ok) {
		_messageText->setCurrentFont(font);
		boldButton->setOn(font.bold());
		italicButton->setOn(font.italic());
		underlineButton->setOn(font.underline());
	}
}

void ChatPageWidget::setFontColor() {
	QColor color;
	color =  QColorDialog::getColor( Qt::black, getWidget());
	_messageText->setColor(color);
}

void ChatPageWidget::showEmoticons() {
	QWidget * sessionWindow = (QWidget*)(parent());
	QPoint pos = sessionWindow->pos();

	_emoticonsWidget->getWidget()->move(pos.x() + sessionWindow->width() + 8,
			pos.y() + sessionWindow->height() - _emoticonsWidget->getWidget()->height());
	_emoticonsWidget->getWidget()->show();
	_emoticonsWidget->getWidget()->raise();
}

void ChatPageWidget::hideEmoticons() {
	_emoticonsWidget->getWidget()->hide();
}


void ChatPageWidget::setSearchPath(QString searchPath) {
	_searchPath = searchPath;
	updateIconsetChooser();
}

void ChatPageWidget::updateIconsetChooser() {
	QDir dir(_searchPath);
	QStringList entries = dir.entryList();
	for ( QStringList::Iterator it = entries.begin(); it != entries.end(); ++it ) {
		if( ((*it)!=".") && ((*it)!="..")) {
			_emoticonSetChooser->insertItem(*it);
		}
	}
}

void ChatPageWidget::insertImage(QString name) {
	_messageText->insert(name);
	giveFocusToTextEdit();
}

void ChatPageWidget::setIconset(const QString & name) {
	_iconset.clear();
	_iconset.removeFromFactory();
	if(_iconset.load("emoticons/"+name)) {
		_iconset.addToFactory();
	}
}

void ChatPageWidget::giveFocusToTextEdit() {
	_chatPageWidget->setActiveWindow();
	_messageText->setFocus();
}

void ChatPageWidget::updateMimeSourceFactory() {
}

void ChatPageWidget::scrollHistoryToBottom() {
	_chatHistoryText->scrollToBottom();	
}

