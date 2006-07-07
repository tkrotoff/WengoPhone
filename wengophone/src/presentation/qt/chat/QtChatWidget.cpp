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

#include <imwrapper/IMContactSet.h>
#include <imwrapper/EnumIMProtocol.h>

#include "QtChatWidget.h"
#include "QtChatTabWidget.h"
#include "QtChatWidgetManager.h"
#include "QtEmoticonsWidget.h"
#include "QtEmoticonsManager.h"

#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>

#include <model/profile/UserProfile.h>

#include <util/WebBrowser.h>
#include <util/Logger.h>

#include <cutil/global.h>

#include <qtutil/QtWengoStyleLabel.h>

#include <util/Logger.h>
#include <util/WebBrowser.h>

#include <QtGui>

static const int CHAT_NOT_TYPING_DELAY=1000;
static const int CHAT_STOPPED_TYPING_DELAY=1000;
//static const QString CHAT_USER_BACKGOUND_COLOR = "#B4C8FF";
static const QString CHAT_USER_BACKGOUND_COLOR = "#F0EFFF";
static const QString CHAT_USER_FORGROUND_COLOR = "#000000";
static const QString CHAT_EMOTICONS_LABEL_OFF_BEGIN = ":/pics/chat/chat_emoticon_button.png";
static const QString CHAT_EMOTICONS_LABEL_OFF_END = ":/pics/profilebar/bar_separator.png";
static const QString CHAT_EMOTICONS_LABEL_OFF_FILL = ":/pics/profilebar/bar_fill.png";
static const QString CHAT_EMOTICONS_LABEL_ON_BEGIN = ":/pics/chat/chat_emoticon_button_on.png";
static const QString CHAT_EMOTICONS_LABEL_ON_END = ":/pics/profilebar/bar_separator.png";
static const QString CHAT_EMOTICONS_LABEL_ON_FILL = ":/pics/profilebar/bar_on_fill.png";
static const QString CHAT_FONT_LABEL_OFF_END = ":/pics/profilebar/bar_end.png";
static const QString CHAT_FONT_LABEL_OFF_FILL = ":/pics/profilebar/bar_fill.png";
static const QString CHAT_FONT_LABEL_ON_END = ":/pics/profilebar/bar_on_end.png";
static const QString CHAT_FONT_LABEL_ON_FILL = ":/pics/profilebar/bar_on_fill.png";

QtChatWidget::QtChatWidget(CChatHandler & cChatHandler, int sessionId, QWidget * parent, Qt::WFlags f) :
QWidget(parent, f), _cChatHandler(cChatHandler) {

	_qtEmoticonManager = new QtEmoticonsManager(this);
	_emoticonsWidget = new EmoticonsWidget(_qtEmoticonManager,this,Qt::Popup);

	_stoppedTypingTimerId = -1;
	_notTypingTimerId = -1;
	_isTyping = false;
	_sessionId = sessionId;
	_lastBackGroundColor = QColor("#D0FFE6");

	//Default nickname for testing purpose
	_nickName = "Wengo";
	_ui.setupUi(this);
	_ui.contactListLabel->setVisible(false);
	createActionFrame();
	setupSendButton();

	QtChatTabWidget * parentWidget = dynamic_cast<QtChatTabWidget *>(parent);

	QtChatWidgetManager * cwm = new QtChatWidgetManager(this,_ui.chatEdit);

	connect (cwm,SIGNAL(enterPressed(Qt::KeyboardModifiers)),this, SLOT (enterPressed(Qt::KeyboardModifiers)));
	connect (cwm,SIGNAL(deletePressed()),this,SLOT (deletePressed()));
	connect (cwm,SIGNAL(ctrlTabPressed()),parentWidget,SLOT(ctrlTabPressedSlot()));
	connect (_fontLabel,SIGNAL(clicked()), this, SLOT (chooseFont()));
	connect (_ui.chatHistory,SIGNAL(anchorClicked(const QUrl &)),this,SLOT(urlClicked(const QUrl & )));
	connect (_emoticonsLabel,SIGNAL(clicked()),this,SLOT(chooseEmoticon()));
	connect (_ui.sendButton,SIGNAL(clicked()),this,SLOT(sendButtonClicked()));

	connect (this,SIGNAL(contactAddedEventSignal()),this,SLOT(contactAddedEventSlot()));
	connect (this,SIGNAL(contactRemovedEventSignal()),this,SLOT(contactRemovedEventSlot()));

	_ui.chatHistory->setHtml ("<qt type=detail>");

	connect(_emoticonsWidget,SIGNAL(emoticonClicked(QtEmoticon)),this,SLOT(emoticonSelected(QtEmoticon)));
	connect(_emoticonsWidget,SIGNAL(closed()),_ui.chatEdit,SLOT(setFocus()));
	connect(_ui.chatEdit,SIGNAL(textChanged ()),SLOT(chatEditChanged()));

}

QtChatWidget::~QtChatWidget() {
	_imChatSession->close();
}

void QtChatWidget::chatEditChanged() {
	if (!_isTyping) {
		_imChatSession->changeTypingState(IMChat::TypingStateTyping);
		_isTyping = true;
	}

	if (_notTypingTimerId != -1) {
		killTimer(_notTypingTimerId);
		_notTypingTimerId = -1;
	}

	if (_stoppedTypingTimerId == -1) {
		_stoppedTypingTimerId = startTimer(CHAT_STOPPED_TYPING_DELAY);
	} else {
		killTimer(_stoppedTypingTimerId);
		_stoppedTypingTimerId = startTimer(CHAT_STOPPED_TYPING_DELAY);
	}
}

void QtChatWidget::timerEvent ( QTimerEvent * event) {
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

void  QtChatWidget::setNickName(const QString & nickname) {
	_nickName = nickname;
}

const QString & QtChatWidget::nickName() const {
	return _nickName;
}

void QtChatWidget::addToHistory(const QString & senderName,const QString & str) {
	QTextCursor curs(_ui.chatHistory->document());
	curs.movePosition(QTextCursor::End);
	_ui.chatHistory->setTextCursor(curs);

	QString header;
	if (hasQtChatContactInfo(senderName)) {
		QtChatContactInfo qtChatContactInfo = getQtChatContactInfo(senderName);
		header = qtChatContactInfo.getHeader();
	} else {
		IMContactSet imContactSet = _imChatSession->getIMContactSet();
		IMContactSet::iterator it = imContactSet.begin();
		QString nickName = QString::fromStdString((*it).getContactId());
		QtChatContactInfo qtChatContactInfo(getNewBackgroundColor(),CHAT_USER_FORGROUND_COLOR,senderName);
		_qtContactInfo[senderName] = qtChatContactInfo;
		header = qtChatContactInfo.getHeader();
	}
	QTextDocument tmp;
	tmp.setHtml(str);

	QString tmpStr = text2Emoticon(replaceUrls(tmp.toPlainText(),str));
	QString table = QString("<table border=0 width=98% cellspacing=0 cellpadding=5>")+
					QString("<tr><td>")+tmpStr+
					QString("</td></tr></table>");
	_ui.chatHistory->insertHtml(header);
	_ui.chatHistory->insertHtml(table);
	_ui.chatHistory->ensureCursorVisible();
}

void QtChatWidget::urlClicked(const QUrl & link) {
	_ui.chatHistory->setSource(QUrl(QString::null));
	WebBrowser::openUrl(link.toString().toStdString());
	_ui.chatHistory->ensureCursorVisible();
}

void QtChatWidget::sendButtonClicked() {
	enterPressed();
}

void QtChatWidget::enterPressed(Qt::KeyboardModifiers modifier) {

	if (modifier == Qt::ControlModifier) {
		_ui.chatEdit->append(QString::null);
		return;
	}
	if (_notTypingTimerId != -1) {
		killTimer(_notTypingTimerId);
		_notTypingTimerId = -1;
	}

	if (_stoppedTypingTimerId != -1) {
		killTimer(_stoppedTypingTimerId);
		_stoppedTypingTimerId = -1;
	}

	_imChatSession->changeTypingState(IMChat::TypingStateNotTyping);

	//Drop empty message
	if (_ui.chatEdit->toPlainText().isEmpty()) {
		return;
	}

	addToHistory(_nickName,_ui.chatEdit->toHtml());
	QString tmp = emoticon2Text(_ui.chatEdit->toHtml());

	//bad and ugly hack
	QTextDocument tmpDocument;
	tmpDocument.setHtml(tmp);


	newMessage(_imChatSession, prepareMessageForSending(tmpDocument.toPlainText()));
	_ui.chatEdit->clear();
	_ui.chatEdit->setFocus();
}

//Work around for Qt 4.1.2 bug
//can be removed with Qt > 4.1.2
void QtChatWidget::deletePressed() {
	QTextCursor cursor = _ui.chatEdit->textCursor();
	if (!cursor.hasSelection()) {
		cursor.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1 );
		cursor.removeSelectedText();
	} else {
		cursor.removeSelectedText();
	}
	_ui.chatEdit->setTextCursor(cursor);
}

QString QtChatWidget::prepareMessageForSending(const QString & message) {
	QString result;

	//Insert font tag
	result = insertFontTag(message);
	result = result.replace("\n","<br>");
	return result;
}

QString QtChatWidget::insertFontTag(const QString & message) {
	QString result = "<font color=\"%1\"><font face=\"%2\">";
	result = result.arg(CHAT_USER_FORGROUND_COLOR);
	result = result.arg(_currentFont.defaultFamily());

	if (_currentFont.bold()) {
		result += "<b>";
	}

	if (_currentFont.italic()) {
		result += "<i>";
	}

	if (_currentFont.underline()) {
		result += "<u>";
	}

	result += message;

	if (_currentFont.underline()) {
		result += "</u>";
	}

	if (_currentFont.italic()) {
		result += "</i>";
	}

	if (_currentFont.bold()) {
		result += "</b>";
	}
	result += "</font></font>";
	return result;
}

void QtChatWidget::chooseFont() {
	bool ok;
	_currentFont = QFontDialog::getFont(&ok,_currentFont,this);
	_ui.chatEdit->setCurrentFont(_currentFont);
	_ui.chatEdit->setFocus();
}

const QString QtChatWidget::emoticon2Text(const QString &htmlstr) {
	return _qtEmoticonManager->emoticons2Text(htmlstr,getProtocol());
}

const QString QtChatWidget::text2Emoticon(const QString &htmlstr) {
	return _qtEmoticonManager->text2Emoticon(htmlstr,getProtocol());
}

QString QtChatWidget::getProtocol() const {
	QString protocol;

	EnumIMProtocol::IMProtocol proto = _imChatSession->getIMChat().getIMAccount().getProtocol();
	switch (proto) {
	case EnumIMProtocol::IMProtocolMSN:
		protocol="msn";
		break;
	case EnumIMProtocol::IMProtocolYahoo:
		protocol="yahoo";
		break;
	case EnumIMProtocol::IMProtocolWengo:
		protocol="wengo";
		break;
	case EnumIMProtocol::IMProtocolJabber:
	case EnumIMProtocol::IMProtocolAIMICQ:
	case EnumIMProtocol::IMProtocolUnknown:
		protocol="default";
		break;
	default:
		protocol="default";
	}
	return protocol;
}

const QString QtChatWidget::replaceUrls(const QString & str, const QString & htmlstr) {
	int beginPos = 0;
	QString tmp=htmlstr;
	int endPos;
	int repCount=0;

	QStringList urls;
	QStringList reps;
	while (true) {
		beginPos = str.indexOf(QRegExp("(http://|https://|ftp://)", Qt::CaseInsensitive), beginPos);

		if (beginPos == -1) {
			break;
		}
		for (endPos = beginPos; endPos < str.size(); endPos++) {
			if ((str[endPos] == ' ') || (str[endPos] == '\r') || (str[endPos] == '\n')) {
				break;
			}
		}
		QString url = str.mid(beginPos, endPos - beginPos);
		urls << url;
		QString r = QString("$$_$$*_WENGOSTRUTILS_|KB|%1").arg(repCount);
		reps << r;
		repCount++;
		tmp.replace(url, r);
		beginPos = endPos;
	}
	for (int i = 0; i < reps.size(); i++) {
		QString url = QString("<a href='" + urls[i]+ "'>" + urls[i] + "</a>");
		tmp.replace(reps[i], url);
	}
	return tmp;
}

void QtChatWidget::chooseEmoticon() {
	QPoint p = _ui.actionFrame->pos();
	p.setY(p.y() + _ui.actionFrame->rect().bottom());
	_emoticonsWidget->move(mapToGlobal(p));
	_emoticonsWidget->setWindowOpacity(0.95);
	_emoticonsWidget->show();
}

void QtChatWidget::emoticonSelected(QtEmoticon emoticon) {
	_ui.chatEdit->insertHtml(emoticon.getHtml());
	_ui.chatEdit->ensureCursorVisible();
}

void QtChatWidget::setIMChatSession(IMChatSession * imChatSession) {
	std::string tmpNickName = imChatSession->getIMChat().getIMAccount().getLogin();
	QString nickName = QString::fromUtf8(tmpNickName.c_str());

	QtChatContactInfo  qtChatContactInfo(QtChatContactInfo(CHAT_USER_BACKGOUND_COLOR,
		CHAT_USER_FORGROUND_COLOR,nickName));

	_qtContactInfo[nickName] = qtChatContactInfo;

	_imChatSession = imChatSession;
	_emoticonsWidget->initButtons(getProtocol());

	_imChatSession->contactAddedEvent +=
		boost::bind(&QtChatWidget::contactAddedEventHandler, this, _1, _2);

	_imChatSession->contactRemovedEvent +=
		boost::bind(&QtChatWidget::contactRemovedEventHandler, this, _1, _2);

	_imChatSession->changeTypingState(IMChat::TypingStateNotTyping);
}

void QtChatWidget::contactAddedEventHandler(IMChatSession &, const IMContact &) {
	contactAddedEventSignal();
}

void QtChatWidget::contactRemovedEventHandler(IMChatSession &, const IMContact &) {
	contactRemovedEventSignal();
}

void QtChatWidget::contactAddedEventSlot() {
	updateContactListLabel();
}

void QtChatWidget::contactRemovedEventSlot() {
	updateContactListLabel();
}

void QtChatWidget::updateContactListLabel() {
	QMutexLocker locker(&_mutex);

	IMContactSet imContactSet = _imChatSession->getIMContactSet();
	QStringList contactStringList;

	IMContactSet::iterator it;

	for (it = imContactSet.begin(); it != imContactSet.end(); it++) {
		QString nickName = QString::fromStdString((*it).getContactId());
		contactStringList << nickName;
		if (!hasQtChatContactInfo(nickName)) {
			QtChatContactInfo qtChatContactInfo(getNewBackgroundColor(),CHAT_USER_FORGROUND_COLOR,nickName);
			_qtContactInfo[nickName] = qtChatContactInfo;
		}
	}
	_ui.contactListLabel->setToolTip(QString::null);
	if (contactStringList.size() > 2) {
		_ui.contactListLabel->show();
		QString str = QString(tr("Chat with: "));
		str += contactStringList[0] + "; ";
		str += contactStringList[1] + "...";
		_ui.contactListLabel->setText(str);
		str = contactStringList.join("<br>");
		_ui.contactListLabel->setToolTip(str);
	} else {
		if (contactStringList.size() > 1) {
			_ui.contactListLabel->show();
			QString str = QString(tr("Chat with: ")) + contactStringList.join("; ");
			_ui.contactListLabel->setText(str);
		}
	}
}

bool QtChatWidget::canDoMultiChat() {
	return _imChatSession->canDoMultiChat();
}

void QtChatWidget::inviteContact() {
	QtChatRoomInviteDlg dlg(*_imChatSession,
		_cChatHandler.getCUserProfile().getCContactList().getContactList(),this);
	dlg.exec();
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

void QtChatWidget::createActionFrame() {
	QHBoxLayout * layout = new QHBoxLayout(_ui.actionFrame);
	_emoticonsLabel = new QtWengoStyleLabel(_ui.actionFrame);

	_fontLabel = new QtWengoStyleLabel(_ui.actionFrame);

	_emoticonsLabel->setPixmaps(
					QPixmap(CHAT_EMOTICONS_LABEL_OFF_BEGIN),
					QPixmap(CHAT_EMOTICONS_LABEL_OFF_END),
					QPixmap(CHAT_EMOTICONS_LABEL_OFF_FILL),
					QPixmap(CHAT_EMOTICONS_LABEL_ON_BEGIN),
					QPixmap(CHAT_EMOTICONS_LABEL_ON_END),
					QPixmap(CHAT_EMOTICONS_LABEL_ON_FILL)
					);

	_emoticonsLabel->setMaximumSize(QSize(120,65));
	_emoticonsLabel->setMinimumSize(QSize(120,65));

	_fontLabel->setPixmaps(
				QPixmap(),
				QPixmap(CHAT_FONT_LABEL_OFF_END),
				QPixmap(CHAT_FONT_LABEL_OFF_FILL),
				QPixmap(),
				QPixmap(CHAT_FONT_LABEL_ON_END),
				QPixmap(CHAT_FONT_LABEL_ON_FILL)
				);

	_emoticonsLabel->setText(tr("emoticons  "));
	_emoticonsLabel->setTextColor(Qt::white);
	_emoticonsLabel->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

	_fontLabel->setText(tr("    fonts"));
	_fontLabel->setTextColor(Qt::white);
	_fontLabel->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	layout->addWidget(_emoticonsLabel);
	layout->addWidget(_fontLabel);
	layout->setMargin(0);
	layout->setSpacing(0);
}

void QtChatWidget::setupSendButton() {
	QPixmap pix = _ui.sendButton->icon().pixmap(50,50);
	QPainter painter (&pix);
	painter.setPen(Qt::white);
	painter.drawText(pix.rect(),Qt::AlignCenter,tr("send"));
	painter.end();
	_ui.sendButton->setIcon(QIcon(pix));
}

void QtChatWidget::setVisible (bool visible) {
	QWidget::setVisible(visible);
	if (visible) {
		_ui.chatEdit->setFocus();
	}
}

QtChatContactInfo QtChatWidget::getQtChatContactInfo(const QString & nickName) const {
	return _qtContactInfo[nickName];
}

bool QtChatWidget::hasQtChatContactInfo(const QString & nickName) const {
	if (_qtContactInfo.find(nickName) == _qtContactInfo.end()) {
		return false;
	}
	return true;
}

QString QtChatWidget::getNewBackgroundColor() const {
	_lastBackGroundColor.setRed(_lastBackGroundColor.red() + 20);
	return QString("%1").arg(_lastBackGroundColor.name());
}
