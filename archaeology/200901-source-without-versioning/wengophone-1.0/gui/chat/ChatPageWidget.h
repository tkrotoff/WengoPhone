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

#ifndef CHATPAGEWIDGET_H
#define CHATPAGEWIDGET_H

#include <iconset.h>
#include <qobject.h>

class EmoticonsWidget;

class SessionWindow;
class Contact;
class QWidget;
class QTextEdit;
class QPushButton;
class QDialog;
class QComboBox;


/**
 * Chat widget component of the SessionWindow.
 *
 * ChatPageWidget is a tab/page inside the SessionWindow.
 * Includes a text edit for the chat message and the chat history
 * and a button to send the message.
 *
 * @see ChatPageWidgetForm.ui
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class ChatPageWidget : public QObject {
	Q_OBJECT
public:

	/**
	 * Then SessionWindow can construct a ChatPageWidget since
	 * the constructor is private.
	 */
	friend class SessionWindow;

	/**
	 * Gets the low-level gui widget of this class.
	 *
	 * @return the low-level widget
	 */
	QWidget * getWidget() const {
		return _chatPageWidget;
	}

	/**
	 * Sets the Contact associated with the chat conversation.
	 *
	 * The Contact is the receiver of the chat messages.
	 *
	 * @param contact Contact associated with the conversation
	 */
	void setContact(const Contact & contact);

	/**
	 * Appends a message to the history of messages.
	 *
	 * @param message message to add to the history
	 */
	void appendMessageToHistory(const QString & message);

	/**
	 * Sets the focus on this gui component.
	 */
	void setFocus();

	/**
	 * Resets to default the widget.
	 */
	void reset();

	/**
	 * Insert an emoticon.
	 * @param the regexp for the emoticon
	 */
	void insertImage(QString name);

	/**
	 * Give the focus to the message text edit
	 */
	void giveFocusToTextEdit();
	
	/**
	 * Scroll the history to bottom
	 */
	void scrollHistoryToBottom();


private slots:

	/**
	 * Sends the chat message via SIP
	 */
	void sendMessage() const;

	/**
	 * The message inside the widget has been changed.
	 */
	void messageTextChanged();

	void setBold();
	void setItalic();
	void setUnderline();
	void setFont();
	void setFontColor();
	void showEmoticons();
	void hideEmoticons();
	void setIconset(const QString&);

	void doubleClicked(int para, int pos);

private:

	/**
	 * Constructs the ChatPageWidget.
	 *
	 * @param parent parent widget
	 */
	ChatPageWidget(QWidget * parent);

	ChatPageWidget(const ChatPageWidget &);
	ChatPageWidget & operator=(const ChatPageWidget &);

	void appendMessageToHistory(const QString & message, const QString & author, const QString & fontColor) const;
	void appendLocalMessageToHistory(const QString & message, const QString & author, const QString & fontColor) const;

	void setSearchPath(QString path);
	void updateIconsetChooser();
	void updateMimeSourceFactory();

	/**
	 * Low-level widget of this class.
	 */
	QWidget * _chatPageWidget;

	/**
	 * Widget that stores the chat message.
	 */
	QTextEdit * _messageText;

	/**
	 * Widget that stores the chat history.
	 */
	QTextEdit * _chatHistoryText;

	/**
	 * Contact associated with the chat conversation.
	 */
	const Contact * _contact;

	static const int MAX_MESSAGE_LENGTH;

	/**
	 * ToolButtons
	 */
	QPushButton * boldButton;
	QPushButton * italicButton;
	QPushButton * underlineButton;
	QPushButton * fontButton;
	QPushButton * fontColorButton;
	QPushButton * smileysButton;

	EmoticonsWidget *_emoticonsWidget;
	QComboBox * _emoticonSetChooser;
	QString _searchPath;
	Iconset _iconset;
};

#endif  //CHATPAGEWIDGET_H
