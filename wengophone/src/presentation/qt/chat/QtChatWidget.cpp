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
#include "chatwidgetmanager.h"
#include "QtEmoticonsWidget.h"
#include "QtEmoticonsManager.h"

#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>

#include <model/profile/UserProfile.h>

#include <util/Logger.h>

#include <cutil/global.h>

#include <qtutil/Object.h>
#include <qtutil/QtWengoStyleLabel.h>
#include <qtutil/WidgetFactory.h>

const int ChatWidget::STOPPED_TYPING_DELAY = 1000;
const int ChatWidget::NOT_TYPING_DELAY = 1000;


ChatWidget::ChatWidget (CChatHandler & cChatHandler, int sessionId, QWidget * parent, Qt::WFlags f) :
QWidget(parent, f), _cChatHandler(cChatHandler){

	_qtEmoticonManager = new QtEmoticonsManager(this);
	_emoticonsWidget = new EmoticonsWidget(_qtEmoticonManager,this,Qt::Popup);

	_stoppedTypingTimerId = -1;
	_notTypingTimerId = -1;
	_isTyping = false;
	_sessionId = sessionId;

	/* Defaults fonts and colors */
	#if defined(OS_MACOSX)
	_nickFont = QFont();
	#else
	_nickFont = QFont("Helvetica", 12);
	#endif
	_nickTextColor = "'#000000'"; // Black
	_nickBgColor = "'#B4C8FF'";
	_nickBgColorAlt = "'#B0FFB3'";
	_nickName = "Wengo";

	_ui.setupUi(this);

	createActionFrame();
	setupSendButton();

	ChatWidgetManager * cwm = new ChatWidgetManager(this,_ui.chatEdit);

	connect (cwm,SIGNAL(enterPressed()),this, SLOT (enterPressed()));
	connect (cwm,SIGNAL(deletePressed()),this,SLOT (deletePressed()));
	connect (_fontLabel,SIGNAL(clicked()), this, SLOT (chooseFont()));
	connect (_ui.chatHistory,SIGNAL(anchorClicked(const QUrl &)),this,SLOT(urlClicked(const QUrl & )));
	connect (_emoticonsLabel,SIGNAL(clicked()),this,SLOT(chooseEmoticon()));
	connect (_ui.sendButton,SIGNAL(clicked()),this,SLOT(enterPressed()));

	connect (this,SIGNAL(contactAddedEventSignal()),this,SLOT(contactAddedEventSlot()));
	connect (this,SIGNAL(contactRemovedEventSignal()),this,SLOT(contactRemovedEventSlot()));

	_ui.chatHistory->setHtml ("<qt type=detail>");

	connect(_emoticonsWidget,SIGNAL(emoticonClicked(QtEmoticon)),this,SLOT(emoticonSelected(QtEmoticon)));
	connect(_emoticonsWidget,SIGNAL(closed()),_ui.chatEdit,SLOT(setFocus()));
	connect(_ui.chatEdit,SIGNAL(textChanged ()),SLOT(chatEditChanged()));

	QGridLayout * frameLayout = new QGridLayout(_ui.contactListFrame);
	_ui.contactListFrame->setVisible(false);
	_scrollArea = new QScrollArea(_ui.contactListFrame);
	frameLayout->addWidget(_scrollArea);

	_contactViewport = new QWidget(_scrollArea);
	_scrollArea->setWidget(_contactViewport);
	new QHBoxLayout(_contactViewport);
	_contactViewport->layout()->setMargin(0);
	_scrollArea->setWidgetResizable(true);

}

ChatWidget::~ChatWidget(){
	_imChatSession->close();
}

void ChatWidget::chatEditChanged(){
	if ( ! _isTyping ){
		_imChatSession->changeTypingState(IMChat::TypingStateTyping);
		_isTyping = true;
	}

	if ( _notTypingTimerId != -1 )
	{
		killTimer(_notTypingTimerId);
		_notTypingTimerId = -1;
	}

	if ( _stoppedTypingTimerId == -1 )
	{
		_stoppedTypingTimerId = startTimer(STOPPED_TYPING_DELAY);
	}
	else
	{
		killTimer(_stoppedTypingTimerId);
		_stoppedTypingTimerId = startTimer(STOPPED_TYPING_DELAY);
	}
}

void ChatWidget::timerEvent ( QTimerEvent * event ){
	if ( event->timerId() == _stoppedTypingTimerId ){
		killTimer(_stoppedTypingTimerId);
		_stoppedTypingTimerId = -1;
		_imChatSession->changeTypingState(IMChat::TypingStateStopTyping);
		_notTypingTimerId = startTimer(NOT_TYPING_DELAY);
		_isTyping = false;
	}
	if ( event->timerId() == _notTypingTimerId){
		killTimer(_notTypingTimerId);
		_notTypingTimerId = -1;
		_imChatSession->changeTypingState(IMChat::TypingStateNotTyping);
		_isTyping = false;
	}
}

void  ChatWidget::setNickName(const QString & nickname){
	_nickName = nickname;
}

const QString & ChatWidget::nickName(){
	return _nickName;
}

void ChatWidget::setNickBgColor(const QString &color){
	_nickBgColor = color;
}

void ChatWidget::setNickTextColor(const QString  &color){
	_nickTextColor = color;
}

void ChatWidget::setNickFont(QFont &font){
	_nickFont = font;
}

const QFont & ChatWidget::nickFont(){
	return _nickFont;
}

const QString & ChatWidget::nickBgColor(){
	return _nickBgColor;
}

const QString & ChatWidget::nickTextColor(){
	return _nickTextColor;
}

void ChatWidget::addToHistory(const QString & senderName,const QString & str){
	QTextCursor curs(_ui.chatHistory->document());
	curs.movePosition(QTextCursor::End);
	_ui.chatHistory->setTextCursor(curs);

	QString header;

	if (senderName == _nickName) {
		header = QString("<table border=0 width=100% cellspacing=0 "
		"cellpadding=0><tr><td BGCOLOR=%1> <font color=%2> %3 </font></td><td BGCOLOR=%4 align=right>"
		"<font color=%5> %6 </font></td></tr></table>").
		arg(_nickBgColor).
		arg(_nickTextColor).
		arg(senderName).
		arg(_nickBgColor).
		arg(_nickTextColor).
		arg(QTime::currentTime().toString());

	} else {
		header = QString("<table border=0 width=100% cellspacing=0 "
		"cellpadding=0><tr><td BGCOLOR=%1> <font color=%2> %3 </font></td><td BGCOLOR=%4 align=right>"
		"<font color=%5> %6 </font></td></tr></table>").
		arg(_nickBgColorAlt).
		arg(_nickTextColor).
		arg(senderName).
		arg(_nickBgColorAlt).
		arg(_nickTextColor).
		arg(QTime::currentTime().toString());

	}


	QTextDocument tmp;
	tmp.setHtml (str);

	_ui.chatHistory->insertHtml (header);
	_ui.chatHistory->insertHtml (text2Emoticon(replaceUrls(tmp.toPlainText(),str + "<P></P>")));
	_ui.chatHistory->ensureCursorVisible();
}
	/* SLOTS */
void ChatWidget::urlClicked(const QUrl & link){
	_ui.chatHistory->setSource(QUrl(""));
	QStringList args;
	args << link.toString();
	QProcess::startDetached ("explorer.exe",args );
	_ui.chatHistory->ensureCursorVisible();
}

void ChatWidget::enterPressed(){
	if ( _notTypingTimerId != -1 )
	{
		killTimer(_notTypingTimerId);
		_notTypingTimerId = -1;
	}

	if ( _stoppedTypingTimerId != -1 ){
		killTimer(_stoppedTypingTimerId);
		_stoppedTypingTimerId = -1;
	}

	_imChatSession->changeTypingState(IMChat::TypingStateNotTyping);

	// Drop empty message
	if (_ui.chatEdit->toPlainText().isEmpty())
		return;

	addToHistory(_nickName,_ui.chatEdit->toHtml());

	QString tmp = emoticon2Text(_ui.chatEdit->toHtml());

	// bad and ugly hack
	QTextEdit textEditTmp(NULL);
	textEditTmp.setHtml(tmp);

	newMessage(_imChatSession, prepareMessageForSending(textEditTmp.toPlainText()));

	_ui.chatEdit->clear();
	_ui.chatEdit->setFocus();

}

void ChatWidget::deletePressed() {
	QTextCursor cursor = _ui.chatEdit->textCursor();
	if (!cursor.hasSelection ()){
		cursor.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1 );
		cursor.removeSelectedText();
	}
	else {
		cursor.removeSelectedText();
	}
	_ui.chatEdit->setTextCursor(cursor);
}

QString ChatWidget::prepareMessageForSending(const QString & message) {
	QString result;
	// Replace URLs with HTML based URLs
	result = replaceTextURLs(message);
	// Insert font tag
	result = insertFontTag(result);
	return result;
}

QString ChatWidget::replaceTextURLs(const QString & message) {
	QString result = message;
	QRegExp regExp("(http://|https://|ftp://)(\\S*) ", Qt::CaseInsensitive);
	result.replace(regExp, "<a href=\"\\1\\2\">\\1\\2</a>");
	return result;
}

QString ChatWidget::insertFontTag(const QString & message) {
	QString result = "<font color=\"%1\"><font face=\"%2\">";
	result = result.arg("#000000");
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

void ChatWidget::chooseFont(){
	bool ok;
	_currentFont = QFontDialog::getFont(&ok,_currentFont,this);
	_ui.chatEdit->setCurrentFont(_currentFont);
	_ui.chatEdit->setFocus();
}

const QString ChatWidget::emoticon2Text(const QString &htmlstr){
	return _qtEmoticonManager->emoticons2Text(htmlstr,getProtocol());
}

const QString ChatWidget::text2Emoticon(const QString &htmlstr){
	return _qtEmoticonManager->text2Emoticon(htmlstr,getProtocol());
}

QString ChatWidget::getProtocol() const {
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

const QString ChatWidget::replaceUrls(const QString & str, const QString & htmlstr){
	int beginPos = 0;
	QString tmp=htmlstr;
	int endPos;
	int repCount=0;

	QStringList urls;
	QStringList reps;
	while(1)
	{
		beginPos = str.indexOf( QRegExp("(http://|https://|ftp://)",Qt::CaseInsensitive),beginPos);

		if ( beginPos == -1)
			break;
		for ( endPos = beginPos; endPos<str.size();endPos++)
		{
			if ( (str[endPos]==' ') || (str[endPos]=='\r') || (str[endPos]=='\n') )
				break;
		}
		QString url = str.mid(beginPos,endPos - beginPos );
		urls << url;
		QString r = QString("$$_$$*_WENGOSTRUTILS_|KB|%1").arg(repCount);
		reps << r;
		repCount++;
		tmp.replace(url,r);
		beginPos = endPos;

	}
	for (int i = 0; i < reps.size(); i++)
	{
		QString url = QString("<a href='"+urls[i]+"'>"+urls[i]+"</a>");
		tmp.replace(reps[i],url);
	}
	return tmp;
}

void ChatWidget::chooseEmoticon(){
	QPoint p = _ui.actionFrame->pos();
	p.setY(p.y() + _ui.actionFrame->rect().bottom());
	_emoticonsWidget->move(mapToGlobal(p));
	_emoticonsWidget->setWindowOpacity(0.95);
	_emoticonsWidget->show();
}

void ChatWidget::emoticonSelected(QtEmoticon emoticon){
	_ui.chatEdit->insertHtml(emoticon.getHtml());
	_ui.chatEdit->ensureCursorVisible();
}

void ChatWidget::setIMChatSession(IMChatSession * imChatSession){
	_imChatSession = imChatSession;
	_emoticonsWidget->initButtons(getProtocol());

	_imChatSession->contactAddedEvent +=
		boost::bind(&ChatWidget::contactAddedEventHandler,this,_1,_2);

	_imChatSession->contactRemovedEvent +=
		boost::bind(&ChatWidget::contactRemovedEventHandler,this,_1,_2);

	_imChatSession->changeTypingState(IMChat::TypingStateNotTyping);

}

// called from the model's thread
void ChatWidget::contactAddedEventHandler(IMChatSession &, const IMContact &){
	contactAddedEventSignal();
}

// called from the model's thread
void ChatWidget::contactRemovedEventHandler(IMChatSession &, const IMContact &){
	contactRemovedEventSignal();
}

// Qt thread
void ChatWidget::contactAddedEventSlot(){
	updateContactListLabel();
}

// Qt thread
void ChatWidget::contactRemovedEventSlot(){
	updateContactListLabel();
}

void ChatWidget::updateContactListLabel(){
	QMutexLocker locker(&_mutex);

	IMContactSet imContactSet = _imChatSession->getIMContactSet();
	QStringList contactStringList;

	IMContactSet::iterator iter;

	for (iter = imContactSet.begin(); iter != imContactSet.end(); iter++){
		contactStringList << QString::fromStdString( (*iter).getContactId() );
	}

	if ( contactStringList.size() > 1 ){
		QString str = QString (tr("Chat with : ")) + contactStringList.join("; ");
		_ui.contactListLabel->setText(str);
	}
}


bool ChatWidget::canDoMultiChat(){
	return _imChatSession->canDoMultiChat();
}

void ChatWidget::inviteContact(){
	QtChatRoomInviteDlg	dlg(*_imChatSession,
		_cChatHandler.getCUserProfile().getCContactList().getContactList(), this);
	dlg.exec();
	if ( dlg.result() == QDialog::Accepted ){
		QtChatRoomInviteDlg::SelectedContact selectedContact = dlg.getSelectedContact();
		QtChatRoomInviteDlg::SelectedContact::iterator iter;

		for (iter = selectedContact.begin(); iter != selectedContact.end(); iter++){
			addContactToContactListFrame( (*iter) );
		}
		contactAdded();
	}
}

void ChatWidget::setRemoteTypingState(const IMChatSession & sender,const IMChat::TypingState state){
	IMContact from = * sender.getIMContactSet().begin();
	QString remoteName = QString::fromUtf8(from.getContactId().c_str());
	switch (state){
		case IMChat::TypingStateNotTyping:
			_ui.typingStateLabel->setText("");
			break;

		case IMChat::TypingStateTyping:
			_ui.typingStateLabel->setText(remoteName + tr(" is typing"));
			break;

		case IMChat::TypingStateStopTyping:
			_ui.typingStateLabel->setText("");
			break;
		default:
			break;
	}
}

void ChatWidget::openContactListFrame(){
	QSize size = _ui.contactListFrame->minimumSize();
	size.setHeight(96);
	_ui.contactListFrame->setMinimumSize(size);
	size = _ui.contactListFrame->maximumSize();
	size.setHeight(96);
	_ui.contactListFrame->setMaximumSize(size);
	_ui.contactListFrame->setVisible(true);
	_ui.contactListFrame->setFrameShape(QFrame::NoFrame);
	_ui.contactListFrame->setFrameShadow(QFrame::Plain);
	_ui.contactListFrame->setAutoFillBackground(true);

	_scrollArea->setFrameShape(QFrame::NoFrame);
	_scrollArea->setFrameShadow(QFrame::Plain);
	_scrollArea->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );

}

void ChatWidget::addContactToContactListFrame(const Contact & contact){
	newContact(contact);
}

void ChatWidget::createActionFrame(){
	QHBoxLayout * layout = new QHBoxLayout(_ui.actionFrame);
	_emoticonsLabel = new QtWengoStyleLabel(_ui.actionFrame);

	_fontLabel = new QtWengoStyleLabel(_ui.actionFrame);


	_emoticonsLabel->setPixmaps(
						QPixmap(":/pics/chat/chat_emoticon_button.png"),
						QPixmap(":/pics/profilebar/bar_separator.png"),
						QPixmap(":/pics/profilebar/bar_fill.png"), // Fill

						QPixmap(":/pics/chat/chat_emoticon_button_on.png"),
						QPixmap(":/pics/profilebar/bar_separator.png"),
						QPixmap(":/pics/profilebar/bar_on_fill.png")  // Fill
						);

	_emoticonsLabel->setMaximumSize(QSize(120,65));
	_emoticonsLabel->setMinimumSize(QSize(120,65));

	_fontLabel->setPixmaps(
						QPixmap(),
						QPixmap(":/pics/profilebar/bar_end.png"),  //
						QPixmap(":/pics/profilebar/bar_fill.png"), // Fill
						QPixmap(),
						QPixmap(":/pics/profilebar/bar_on_end.png"),
						QPixmap(":/pics/profilebar/bar_on_fill.png")  // Fill
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

void ChatWidget::setupSendButton(){
	QPixmap pix = _ui.sendButton->icon().pixmap(50,50);
	QPainter painter ( &pix );
	painter.setPen(Qt::white);
	painter.drawText(pix.rect(),Qt::AlignCenter,tr("send"));
	painter.end();
	_ui.sendButton->setIcon(QIcon(pix));
}

void ChatWidget::setVisible ( bool visible ){
	QWidget::setVisible(visible);
	if ( visible )
		_ui.chatEdit->setFocus();
}

