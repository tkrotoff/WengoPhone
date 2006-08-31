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

#include "QtChatWidget.h"
#include "QtChatTabWidget.h"
//#include "QtChatActionBarWidget.h"
#include "QtChatEditEventManager.h"
#include "QtChatEditActionBarWidget.h"
#include "QtChatEditWidget.h"
#include "QtChatHistoryWidget.h"
#include "QtChatUtils.h"
#include "QtChatUserFrame.h"
#include "chatroom/QtChatRoomInviteDlg.h"
#include "contactlist/QtChatContactList.h"
#include "emoticons/QtEmoticonsWidget.h"

#include <coipmanager/CoIpManager.h>
#include <filesessionmanager/SendFileSession.h>

#include <model/WengoPhone.h>
#include <model/profile/UserProfile.h>

#include <control/CWengoPhone.h>
#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>

#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/contactlist/QtContactList.h>
#include <presentation/qt/contactlist/QtContactListManager.h>

#include <imwrapper/Account.h>
#include <imwrapper/EnumIMProtocol.h>
#include <imwrapper/IMContactSet.h>

#include <util/File.h>
#include <util/Logger.h>
#include <cutil/global.h>

#include <QtGui/QtGui>

#include <string>

static const int CHAT_NOT_TYPING_DELAY = 1000;
static const int CHAT_STOPPED_TYPING_DELAY = 1000;
static const QString CHAT_USER_BACKGOUND_COLOR = "#F0EFFF";
static const QString CHAT_USER_FORGROUND_COLOR = "#000000";

QtChatWidget::QtChatWidget(CChatHandler & cChatHandler,
	QtWengoPhone * qtWengoPhone, int sessionId, QWidget * parent, QtChatTabWidget * qtChatTabWidget) :
	QWidget(parent), _cChatHandler(cChatHandler),_qtWengoPhone(qtWengoPhone) {

	//Default nickname for testing purpose
	_nickName = "Wengo";
	_stoppedTypingTimerId = -1;
	_notTypingTimerId = -1;
	_isTyping = false;
	_sessionId = sessionId;
	_lastBackGroundColor = QColor("#D0FFE6");
	_isContactListFrameOpened = false;
	_isUserPictureFrameOpened = false;
	
	_bold = false;
	_italic = false;
	_underline = false;

	//setup ui
	_ui.setupUi(this);
	QPixmap pix = _ui.sendButton->icon().pixmap(50,50);
	QPainter painter(&pix);
	painter.setPen(Qt::white);
	painter.drawText(pix.rect(),Qt::AlignCenter,tr("send"));
	painter.end();
	_ui.sendButton->setIcon(QIcon(pix));
	////

	//creates sub widgets
	_emoticonsWidget = new EmoticonsWidget(this, Qt::Popup);
	_editActionBar = new QtChatEditActionBarWidget(this);
	_chatEdit = new QtChatEditWidget(this);
	_chatHistory = new QtChatHistoryWidget(this);
	_qtChatUserFrame = new QtChatUserFrame(this);
	_qtChatContactList = new QtChatContactList(this);
	////

	//create QtChatEditEventManager
	QtChatEditEventManager * cwm = new QtChatEditEventManager(this, _chatEdit);
	////

	//add sub widgets
	_ui.editActionStackedWidget->addWidget(_editActionBar);
	_ui.editActionStackedWidget->setCurrentWidget(_editActionBar);
	_ui.editStackedWidget->addWidget(_chatEdit);
	_ui.editStackedWidget->setCurrentWidget(_chatEdit);
	_ui.historyStackedWidget->addWidget(_chatHistory);
	_ui.historyStackedWidget->setCurrentWidget(_chatHistory);
	////

	//signal connection
	connect (cwm, SIGNAL(enterPressed(Qt::KeyboardModifiers)), SLOT(enterPressed(Qt::KeyboardModifiers)));
	connect (cwm, SIGNAL(deletePressed()), SLOT(deletePressed()));
	connect (cwm, SIGNAL(ctrlTabPressed()), qtChatTabWidget, SLOT(ctrlTabPressedSlot()));
	connect (_ui.sendButton, SIGNAL(clicked()), SLOT(sendButtonClicked()));
	connect (_editActionBar, SIGNAL(fontLabelClicked()), SLOT(changeFont()));
	connect (_editActionBar, SIGNAL(emoticonsLabelClicked()), SLOT(chooseEmoticon()));
	connect (_editActionBar, SIGNAL(fontColorLabelClicked()), SLOT(changeFontColor()));
	connect (_editActionBar, SIGNAL(boldLabelClicked()), SLOT(boldClickedSlot()));
	connect (_editActionBar, SIGNAL(italicLabelClicked()), SLOT(italicClickedSlot()));
	connect (_editActionBar, SIGNAL(underlineLabelClicked()), SLOT(underlineClickedSlot()));
	connect(_emoticonsWidget, SIGNAL(emoticonClicked(QtEmoticon)), SLOT(emoticonSelected(QtEmoticon)));
	connect(_emoticonsWidget, SIGNAL(closed()), _chatEdit, SLOT(setFocus()));
	connect(_chatEdit, SIGNAL(textChanged()), SLOT(chatEditTextChanged()));
	connect(_chatEdit, SIGNAL(fileDragged(const QString &)), SLOT(fileDraggedSlot(const QString &)));
	connect (this, SIGNAL(contactAddedEventSignal(IMChatSession &, const IMContact &)),
		SLOT(contactAddedEventSlot(IMChatSession &, const IMContact &)));
	connect (this, SIGNAL(contactRemovedEventSignal(IMChatSession &, const IMContact &)),
		SLOT(contactRemovedEventSlot(IMChatSession &, const IMContact &)));
	connect(_ui.historyFoldButton, SIGNAL(clicked()), SLOT(historyFoldButtonClicked()));
	connect(_ui.editFoldButton, SIGNAL(clicked()), SLOT(editFoldButtonClicked()));
	////

	addUserPictureFrame();
	addContactListFrame();
	updateUserFrame();
	_cChatHandler.getCUserProfile().getUserProfile().profileChangedEvent +=
		boost::bind(&QtChatWidget::updateUserFrame, this);
}

QtChatWidget::~QtChatWidget() {
	_imChatSession->close();
}

void QtChatWidget::changeFont() {
	bool ok;
	QFont font = QFontDialog::getFont(&ok, _chatEdit->currentFont(), this);
	if (ok) {
		_currentFont = font;
		_chatEdit->setCurrentFont(font);
	}
	_chatEdit->setFocus();
}

void QtChatWidget::changeFontColor() {
	bool ok;
	QRgb color = QColorDialog::getRgba(_chatEdit->textColor().rgba(), &ok, this);
	if (ok) {
		_currentColor = QColor(color);
		_chatEdit->setTextColor(_currentColor);
	}
	_chatEdit->setFocus();
}

void QtChatWidget::boldClickedSlot() {
	_bold = !_bold;
	if (_bold) {
		_chatEdit->setFontWeight(QFont::Bold);
	} else {
		_chatEdit->setFontWeight(QFont::Normal);
	}
}

void QtChatWidget::italicClickedSlot() {
	_italic = !_italic;
	_chatEdit->setFontItalic(_italic);
}

void QtChatWidget::underlineClickedSlot() {
	_underline = !_underline;
	_chatEdit->setFontUnderline(_underline);
}

void QtChatWidget::chooseEmoticon() {
	QPoint p = _ui.editActionStackedWidget->pos();
	p.setY(p.y() + _ui.editActionStackedWidget->rect().bottom());
	_emoticonsWidget->move(mapToGlobal(p));
	_emoticonsWidget->setWindowOpacity(0.95);
	_emoticonsWidget->show();
}

void QtChatWidget::setVisible(bool visible) {
	QWidget::setVisible(visible);
	if (visible) {
		_chatEdit->setFocus();
	}
}

void QtChatWidget::showInviteDialog() {
	if (canDoMultiChat()) {
		QtChatRoomInviteDlg dlg(*_imChatSession, 
			_cChatHandler.getCUserProfile().getCContactList(), this);
		dlg.exec();
	}
}

void QtChatWidget::contactAddedEventHandler(IMChatSession & sender, const IMContact & imContact) {
	contactAddedEventSignal(sender, imContact);
}

void QtChatWidget::contactRemovedEventHandler(IMChatSession & sender, const IMContact & imContact) {
	contactRemovedEventSignal(sender, imContact);
}

void QtChatWidget::contactAddedEventSlot(IMChatSession & sender, const IMContact & imContact) {
	/*
	QMutexLocker locker(&_mutex);

	QtContactList * qtContactList = _qtWengoPhone->getContactList();
	CContactList & cContactList = qtContactList->getCContactList();

	std::string contactId = cContactList.findContactThatOwns(imContact);
	ContactProfile profile = cContactList.getContactProfile(contactId);
	std::string data = profile.getIcon().getData();
	LOG_DEBUG("\n\n\n\n contactid: " + contactId + "\ndata: " + data + "\n\n\n\n");

	QPixmap pixmap;
	pixmap.loadFromData((uchar *)data.c_str(), data.size());
	_qtChatContactList->addContact(QString::fromStdString(contactId), pixmap,
		QString::fromStdString(profile.getDisplayName()));
	*/

	//LOG_DEBUG("\n\n\n\n contactAddedEventSlot \n\n\n\n");
	updateChatContactList();
}

void QtChatWidget::contactRemovedEventSlot(IMChatSession & sender, const IMContact & imContact) {
	/*
	QMutexLocker locker(&_mutex);

	QtContactList * qtContactList = _qtWengoPhone->getContactList();
	CContactList & cContactList = qtContactList->getCContactList();
	std::string contactId = cContactList.findContactThatOwns(imContact);
	_qtChatContactList->removeContact(QString::fromStdString(contactId));
	*/
	//LOG_DEBUG("\n\n\n\n contactRemovedEventSlot \n\n\n\n");
	updateChatContactList();
}

void QtChatWidget::sendButtonClicked() {
	enterPressed();
}

bool QtChatWidget::hasUserColor(const QString & nickName) const {
	return (_userColorHash.find(nickName) != _userColorHash.end());
}

void QtChatWidget::emoticonSelected(QtEmoticon emoticon) {
	_chatEdit->insertHtml(emoticon.getHtml());
	_chatEdit->ensureCursorVisible();
}

QString QtChatWidget::getNewBackgroundColor() const {
	_lastBackGroundColor.setRed(_lastBackGroundColor.red() + 20);
	return QString("%1").arg(_lastBackGroundColor.name());
}

void QtChatWidget::historyFoldButtonClicked() {
	if (_isContactListFrameOpened) {
		removeContactListFrame();
	} else {
		addContactListFrame();
	}
}

void QtChatWidget::editFoldButtonClicked() {
	if (_isUserPictureFrameOpened) {
		removeUserPictureFrame();
	} else {
		addUserPictureFrame();
	}
}

void QtChatWidget::enterPressed(Qt::KeyboardModifiers modifier) {
	//CTRL+ENTER = new line
	if (modifier == Qt::ControlModifier) {
		_chatEdit->append(QString::null);
		////
	} else {
		sendMessage();
	}
}

void QtChatWidget::addUserPictureFrame() {
	QGridLayout * glayout = dynamic_cast<QGridLayout *>(_ui.EditFrame->layout());
	_qtChatUserFrame->setVisible(true);
	_qtChatUserFrame->setMinimumSize(65, 65);
	glayout->addWidget(_qtChatUserFrame, 0, 2);

	_isUserPictureFrameOpened = true;
	_ui.editFoldButton->setIcon(QIcon(":pics/group_close.png"));
}

void QtChatWidget::removeUserPictureFrame() {
	QGridLayout * glayout = dynamic_cast<QGridLayout *>(_ui.EditFrame->layout());
	_qtChatUserFrame->setVisible(false);
	_qtChatUserFrame->setMinimumSize(0, 0);
	glayout->removeWidget(_qtChatUserFrame);

	_isUserPictureFrameOpened = false;
	_ui.editFoldButton->setIcon(QIcon(":pics/group_close_reverse.png"));
}

void QtChatWidget::addContactListFrame() {
	QGridLayout * glayout = dynamic_cast<QGridLayout *>(_ui.HistoryFrame->layout());
	_qtChatContactList->setVisible(true);
	_qtChatContactList->setMinimumSize(65, 0);
	//_qtChatContactList->setMaximumSize(65, 0);
	glayout->addWidget(_qtChatContactList, 0, 2);

	_isContactListFrameOpened = true;
	_ui.historyFoldButton->setIcon(QIcon(":pics/group_close.png"));
}

void QtChatWidget::removeContactListFrame() {
	QGridLayout * glayout = dynamic_cast<QGridLayout *>(_ui.HistoryFrame->layout());
	_qtChatContactList->setVisible(false);
	_qtChatContactList->setMinimumSize(0, 0);
	glayout->removeWidget(_qtChatContactList);

	_isContactListFrameOpened = false;
	_ui.historyFoldButton->setIcon(QIcon(":pics/group_close_reverse.png"));
}

void QtChatWidget::updateUserFrame() {
	QPixmap pixmap;
	std::string myData = _cChatHandler.getCUserProfile().getUserProfile().getIcon().getData();
	pixmap.loadFromData((uchar *)myData.c_str(), myData.size());
	_qtChatUserFrame->setPixmap(pixmap);
}

void QtChatWidget::addToHistory(const QString & senderName, const QString & str) {
	QTextCursor curs(_chatHistory->document());
	curs.movePosition(QTextCursor::End);
	_chatHistory->setTextCursor(curs);

	if (!hasUserColor(senderName)) {
		_userColorHash[senderName] = getNewBackgroundColor();
	}

	// insert "header" code html
	QString header = QtChatUtils::getUserHeader(getUserColor(senderName),CHAT_USER_FORGROUND_COLOR, senderName);
	_chatHistory->insertHtml(header);
	////

	// insert message html code
	QString table = QString("<table border=\"0\" width=\"98%\" cellspacing=\"0\" cellpadding=\"5\">") +
		QString("<tr><td>") + 
		QtChatUtils::decodeMessage(_imChatSession->getIMChat().getIMAccount().getProtocol(), str) +
		QString("</td></tr></table>");
	_chatHistory->insertHtml(table);
	////

	_chatHistory->ensureCursorVisible();
}

void QtChatWidget::sendMessage() {

	//manage typing timer
	stopNotTypingTimer();
	stopStoppedTypingTimer();
	////

	//Not typing anymore
	_imChatSession->changeTypingState(IMChat::TypingStateNotTyping);

	//Drop empty message
	if (_chatEdit->toPlainText().isEmpty()) {
		return;
	}

	//add the text to QtChatHistoryWidget
	addToHistory(_nickName, _chatEdit->toHtml());
	////

	std::string message(QtChatUtils::encodeMessage(
		_chatEdit->currentFont(),
		_imChatSession->getIMChat().getIMAccount().getProtocol(),
		_chatEdit->toHtml()).toUtf8().constData()
	);
	_imChatSession->sendMessage(message);
	////

	_chatEdit->clear();
	_chatEdit->setCurrentFont(_currentFont);
	_chatEdit->setTextColor(_currentColor);
	_chatEdit->setFontItalic(_italic);
	_chatEdit->setFontUnderline(_underline);
	if (_bold) {
		_chatEdit->setFontWeight(QFont::Bold);
	} else {
		_chatEdit->setFontWeight(QFont::Normal);
	}
	_chatEdit->setFocus();
}

void QtChatWidget::setIMChatSession(IMChatSession * imChatSession) {
	
	LOG_DEBUG("\n\n\n\n setIMChatSession \n\n\n\n");
	
	std::string tmpNickName = imChatSession->getIMChat().getIMAccount().getLogin();
	QString nickName = QString::fromUtf8(tmpNickName.c_str());
	_userColorHash[nickName] = CHAT_USER_BACKGOUND_COLOR;

	_imChatSession = imChatSession;
	_emoticonsWidget->initButtons(QtChatUtils::getProtocol(_imChatSession->getIMChat().getIMAccount().getProtocol()));

	_imChatSession->contactAddedEvent +=
		boost::bind(&QtChatWidget::contactAddedEventHandler, this, _1, _2);
	_imChatSession->contactRemovedEvent +=
		boost::bind(&QtChatWidget::contactRemovedEventHandler, this, _1, _2);
	_imChatSession->changeTypingState(IMChat::TypingStateNotTyping);
	
	//TODO: remove this call
	updateChatContactList();
}

void QtChatWidget::chatEditTextChanged() {
	// change typing state to Typing
	if (!_isTyping) {
		_imChatSession->changeTypingState(IMChat::TypingStateTyping);
		_isTyping = true;
	}
	////

	// manage timers
	stopNotTypingTimer();
	if (_stoppedTypingTimerId == -1) {
		_stoppedTypingTimerId = startTimer(CHAT_STOPPED_TYPING_DELAY);
	} else {
		killTimer(_stoppedTypingTimerId);
		_stoppedTypingTimerId = startTimer(CHAT_STOPPED_TYPING_DELAY);
	}
	////
}

void QtChatWidget::setRemoteTypingState(const IMChatSession & sender,const IMChat::TypingState state) {
	IMContact from = * sender.getIMContactSet().begin();
	QString remoteName = QString::fromUtf8(from.getContactId().c_str());
	switch (state) {
		case IMChat::TypingStateNotTyping:
			_ui.typingStateLabel->setText(QString::null);
			break;
		case IMChat::TypingStateTyping:
			_ui.typingStateLabel->setText(remoteName + tr(" is typing"));
			break;
		case IMChat::TypingStateStopTyping:
			_ui.typingStateLabel->setText(QString::null);
			break;
		default:
			break;
	}
}

void QtChatWidget::timerEvent(QTimerEvent * event) {
	if (event->timerId() == _stoppedTypingTimerId) {
		killTimer(_stoppedTypingTimerId);
		_stoppedTypingTimerId = -1;
		_imChatSession->changeTypingState(IMChat::TypingStateStopTyping);
		_notTypingTimerId = startTimer(CHAT_NOT_TYPING_DELAY);
		_isTyping = false;
	}

	if (event->timerId() == _notTypingTimerId) {
		killTimer(_notTypingTimerId);
		_notTypingTimerId = -1;
		_imChatSession->changeTypingState(IMChat::TypingStateNotTyping);
		_isTyping = false;
	}
}

void QtChatWidget::stopNotTypingTimer() {
	if (_notTypingTimerId != -1) {
		killTimer(_notTypingTimerId);
		_notTypingTimerId = -1;
	}
}

void QtChatWidget::stopStoppedTypingTimer() {
	if (_stoppedTypingTimerId != -1) {
		killTimer(_stoppedTypingTimerId);
		_stoppedTypingTimerId = -1;
	}
}

//Work around for Qt 4.1.2 bug, can be removed with Qt > 4.1.2
void QtChatWidget::deletePressed() {
	QTextCursor cursor = _chatEdit->textCursor();
	if (!cursor.hasSelection()) {
		cursor.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1 );
		cursor.removeSelectedText();
	} else {
		cursor.removeSelectedText();
	}
	_chatEdit->setTextCursor(cursor);
}
////

void QtChatWidget::updateChatContactList() {
	QMutexLocker locker(&_mutex);

	QtContactList * qtContactList = _qtWengoPhone->getContactList();
	CContactList & cContactList = qtContactList->getCContactList();

	IMContactSet imContactSet = _imChatSession->getIMContactSet();
	IMContactSet::iterator it;
	for (it = imContactSet.begin(); it != imContactSet.end(); it++) {

		std::string contactId = cContactList.findContactThatOwns(*it);
		ContactProfile profile = cContactList.getContactProfile(contactId);

		std::string data = profile.getIcon().getData();
		QPixmap pixmap;
		pixmap.loadFromData((uchar *)data.c_str(), data.size());
		_qtChatContactList->addContact(QString::fromStdString(contactId), pixmap,
			QString::fromStdString(profile.getDisplayName()));
	}
}

void QtChatWidget::fileDraggedSlot(const QString & filename) {
	QtContactList * qtContactList = _qtWengoPhone->getContactList();
	CContactList & cContactList = qtContactList->getCContactList();
	SendFileSession * fileSession =
		_cChatHandler.getCWengoPhone().getWengoPhone().getCoIpManager()->getFileSessionManager().createSendFileSession();
	IMContactSet imContactSet = _imChatSession->getIMContactSet();
	for (IMContactSet::const_iterator it = imContactSet.begin();
		it != imContactSet.end();
		++it) {
		std::string contactId = cContactList.findContactThatOwns(*it);
		fileSession->addContact(contactId);
	}

	fileSession->addFile(File(filename.toStdString()));
	fileSession->start();
}
