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

#include "QtChatWidget.h"
#include "chatwidgetmanager.h"
#include "QtEmoticonsWidget.h"

#include <util/Logger.h>

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>

#include <model/profile/UserProfile.h>

#include <qtutil/QtWengoStyleLabel.h>



ChatWidget::ChatWidget (CChatHandler & cChatHandler, int sessionId, QWidget * parent, Qt::WFlags f) :
QWidget(parent, f), _cChatHandler(cChatHandler){

    _widget =WidgetFactory::create(":/forms/chat/ChatRoomWidget.ui", this);
    QGridLayout * layout = new QGridLayout();
    layout->addWidget(_widget);
    layout->setMargin(0);
    setLayout(layout);

	_stoppedTypingDelay=1000;
	_notTypingDelay=1000;
	_stoppedTypingTimerId = -1;
	_notTypingTimerId = -1;
	_isTyping = false;
	_sessionId = sessionId;

    /* Defaults fonts and colors */
    _nickFont = QFont("Helvetica", 12);
    _nickTextColor = "'#000000'"; // Black
    _nickBgColor = "'#B4C8FF'";
	_nickBgColorAlt = "'#B0FFB3'";
    _nickName = "Wengo";

    _chatHistory = Object::findChild<QTextBrowser *>(_widget,"chatHistory");
    _sendButton = Object::findChild<QPushButton *>(_widget,"sendButton");
    _chatEdit = Object::findChild<QTextEdit *>(_widget,"chatEdit");
	_contactListFrame = Object::findChild<QFrame *>(_widget,"contactListFrame");
	_typingStateLabel = Object::findChild<QLabel *>(_widget,"typingStateLabel");
	_actionFrame = Object::findChild<QFrame *>(_widget,"actionFrame");
    _contactListLabel = Object::findChild<QLabel *>(_widget,"contactListLabel");

	createActionFrame();
    setupSendButton();

    ChatWidgetManager * cwm = new ChatWidgetManager(this,_chatEdit);

    connect (cwm,SIGNAL(enterPressed()),this, SLOT (enterPressed()));
    connect (_fontLabel,SIGNAL(clicked()), this, SLOT (chooseFont()));
    connect (_chatHistory,SIGNAL(anchorClicked(const QUrl &)),this,SLOT(urlClicked(const QUrl & )));
    connect (_emoticonsLabel,SIGNAL(clicked()),this,SLOT(chooseEmoticon()));
    connect (_sendButton,SIGNAL(clicked()),this,SLOT(enterPressed()));

    connect (this,SIGNAL(contactAddedEventSignal()),this,SLOT(contactAddedEventSlot()));
    connect (this,SIGNAL(contactRemovedEventSignal()),this,SLOT(contactRemovedEventSlot()));

    _chatHistory->setHtml ("<qt type=detail>");

    _emoticonsWidget = new EmoticonsWidget(this,Qt::Popup);

    connect(_emoticonsWidget,SIGNAL(emoticonClicked(QtEmoticon)),this,SLOT(emoticonSelected(QtEmoticon)));
	connect(_emoticonsWidget,SIGNAL(closed()),_chatEdit,SLOT(setFocus()));
	connect(_chatEdit,SIGNAL(textChanged ()),SLOT(chatEditChanged()));

	QGridLayout * frameLayout = new QGridLayout(_contactListFrame);
	_contactListFrame->setVisible(false);
	_scrollArea = new QScrollArea(_contactListFrame);
	frameLayout->addWidget(_scrollArea);

	_contactViewport = new QWidget(_scrollArea);
	_scrollArea->setWidget(_contactViewport);
	new QHBoxLayout(_contactViewport);
	_contactViewport->layout()->setMargin(0);
	_scrollArea->setWidgetResizable(true);
}

ChatWidget::~ChatWidget(){

	_imChatSession->contactAddedEvent -=
        boost::bind(&ChatWidget::contactAddedEventHandler,this,_1,_2);

    _imChatSession->contactRemovedEvent -=
        boost::bind(&ChatWidget::contactRemovedEventHandler,this,_1,_2);

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
		_stoppedTypingTimerId = startTimer(_stoppedTypingDelay);
	}
	else
	{
		killTimer(_stoppedTypingTimerId);
		_stoppedTypingTimerId = startTimer(_stoppedTypingDelay);
	}
}

void ChatWidget::timerEvent ( QTimerEvent * event ){
	if ( event->timerId() == _stoppedTypingTimerId ){
		killTimer(_stoppedTypingTimerId);
		_stoppedTypingTimerId = -1;
		_imChatSession->changeTypingState(IMChat::TypingStateStopTyping);
		_notTypingTimerId = startTimer(_notTypingDelay);
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

const QFont&    ChatWidget::nickFont(){
    return _nickFont;
}

const QString &   ChatWidget::nickBgColor(){
    return _nickBgColor;
}

const QString &   ChatWidget::nickTextColor(){
    return _nickTextColor;
}

void ChatWidget::addToHistory(const QString & senderName,const QString & str){
	QString bgColor;
    QTextCursor curs(_chatHistory->document());
    curs.movePosition(QTextCursor::End);

	if (senderName == _nickName)
		bgColor = _nickBgColor;
	else
		bgColor = _nickBgColorAlt;

    QString header = QString("<table border=0 width=100% cellspacing=0 "
    "cellpadding=0><tr><td BGCOLOR=%1> <font color=%2> %3 </font></td><td BGCOLOR=%4 align=right>"
    "<font color=%5> %6 </font></td></tr></table>").
    arg(_nickBgColorAlt).
    arg(_nickTextColor).
    arg(senderName).
	arg(_nickBgColorAlt).
    arg(_nickTextColor).
    arg(QTime::currentTime().toString());
	_chatHistory->insertHtml (header);
    _chatHistory->insertHtml (text2Emoticon(str));
    _chatHistory->ensureCursorVisible();
}
    /* SLOTS */
void ChatWidget::urlClicked(const QUrl & link){
    _chatHistory->setSource(QUrl(""));
    QStringList args;
    args << link.toString();
    QProcess::startDetached ("explorer.exe",args );
    _chatHistory->ensureCursorVisible();
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
	if (_chatEdit->toPlainText().isEmpty())
		return;

    QTextCursor curs(_chatHistory->document());
    curs.movePosition(QTextCursor::End);

    QString text= QString("<table border=0 width=100% cellspacing=0 "
    "cellpadding=0><tr><td BGCOLOR=%1> <font color=%2> %3 </font></td><td BGCOLOR=%4 align=right>"
    "<font color=%5> %6 </font></td></tr></table>").
    arg(_nickBgColor).
    arg(_nickTextColor).
    arg(_nickName).
    arg(_nickBgColor).
    arg(_nickTextColor).
    arg(QTime::currentTime().toString());

//    QString text = generateHtmlHeader("#dced80","#b9db01","#3d7c00",_nickName);

    _chatHistory->setTextCursor(curs);
    _chatHistory->insertHtml (text);
    _chatHistory->insertHtml (text2Emoticon(replaceUrls(_chatEdit->toPlainText(),_chatEdit->toHtml() + "<P></P>")));
    _chatHistory->ensureCursorVisible();

    newMessage(_imChatSession, prepareMessageForSending(_chatEdit->toPlainText()));

    _chatEdit->clear();
    _chatEdit->setFocus();

}

QString ChatWidget::prepareMessageForSending(const QString & message) {
	QString result;

	// Replace URLs with HTML based URLs
	result = replaceTextURLs(message);
	////

	// Insert font tag
	result = insertFontTag(result);
	////

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
    _chatEdit->setCurrentFont(_currentFont);
    _chatEdit->setFocus();
}

const QString ChatWidget::Emoticon2Text(const QString &htmlstr){
	QVector<QtEmoticon> emoticons = _emoticonsWidget->getEmoticonsVector();
	QString tmp = htmlstr;

	for (int i = 0; i < emoticons.size(); i++)
	{
		tmp.replace(QRegExp(emoticons[i].getHtmlRegExp(),Qt::CaseInsensitive),emoticons[i].getDefaultText());
	}
	return tmp;
}

const QString ChatWidget::text2Emoticon(const QString &htmlstr){
	QVector<QtEmoticon> emoticons = _emoticonsWidget->getEmoticonsVector();
	QString tmp = htmlstr;

	for (int i = 0; i < emoticons.size(); i++)
	{
		tmp.replace(QRegExp(emoticons[i].getRegExp(),Qt::CaseInsensitive),emoticons[i].getHtml());
	}
	return tmp;
}

const QString  ChatWidget::replaceUrls(const QString & str, const QString & htmlstr){
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
	QPoint p = _actionFrame->pos();
	p.setY(p.y() + _actionFrame->rect().bottom());
	_emoticonsWidget->move(mapToGlobal(p));
	_emoticonsWidget->setWindowOpacity(0.95);
    _emoticonsWidget->show();
}

void ChatWidget::emoticonSelected(QtEmoticon emoticon){
	_chatEdit->insertHtml(emoticon.getHtml());
	_chatEdit->ensureCursorVisible();
}

void ChatWidget::setIMChatSession(IMChatSession * imChatSession){
	_imChatSession = imChatSession;

	_imChatSession->contactAddedEvent +=
        boost::bind(&ChatWidget::contactAddedEventHandler,this,_1,_2);

    _imChatSession->contactRemovedEvent +=
        boost::bind(&ChatWidget::contactRemovedEventHandler,this,_1,_2);

	_imChatSession->changeTypingState(IMChat::TypingStateNotTyping);

}

// called from the model's thread
void ChatWidget::contactAddedEventHandler(IMChatSession & sender, const IMContact & imContact){
    contactAddedEventSignal();
}

// called from the model's thread
void ChatWidget::contactRemovedEventHandler(IMChatSession & sender, const IMContact & imContact){
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
        _contactListLabel->setText(str);
    }

}


bool ChatWidget::canDoMultiChat(){
	return _imChatSession->canDoMultiChat();
}

void ChatWidget::inviteContact(){
	QtChatRoomInviteDlg	dlg(*_imChatSession, _cChatHandler.getUserProfile().getContactList(),this);
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
			_typingStateLabel->setText("");
			break;

		case IMChat::TypingStateTyping:
			_typingStateLabel->setText(remoteName + tr(" is typing"));
			break;

		case IMChat::TypingStateStopTyping:
			_typingStateLabel->setText("");
			break;
		default:
			break;
	}
}

void ChatWidget::openContactListFrame(){
	QSize size = _contactListFrame->minimumSize();
	size.setHeight(96);
	_contactListFrame->setMinimumSize(size);
	size = _contactListFrame->maximumSize();
	size.setHeight(96);
	_contactListFrame->setMaximumSize(size);
	_contactListFrame->setVisible(true);
	_contactListFrame->setFrameShape(QFrame::NoFrame);
	_contactListFrame->setFrameShadow(QFrame::Plain);
	_contactListFrame->setAutoFillBackground(true);

	_scrollArea->setFrameShape(QFrame::NoFrame);
	_scrollArea->setFrameShadow(QFrame::Plain);
	_scrollArea->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );

}

void ChatWidget::addContactToContactListFrame(const Contact & contact){
	newContact(contact);
}

void ChatWidget::createActionFrame(){
    QGridLayout * layout = new QGridLayout(_actionFrame);
   	_emoticonsLabel = new QtWengoStyleLabel(_actionFrame);

	_fontLabel = new QtWengoStyleLabel(_actionFrame);


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

    _emoticonsLabel->setText(tr("emoticons   "));
	_emoticonsLabel->setTextColor(Qt::white);
	_emoticonsLabel->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

/*
    _emoticonsLabel->setBackgroundColor(palette().color(QPalette::Base));
    _fontLabel->setBackgroundColor(palette().color(QPalette::Base));
*/
	_fontLabel->setText(tr("    fonts"));
	_fontLabel->setTextColor(Qt::white);
    _fontLabel->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	layout->addWidget(_emoticonsLabel,0,0);
	layout->addWidget(_fontLabel,0,1);
	layout->setMargin(0);
	layout->setSpacing(0);

}
QString ChatWidget::generateHtmlHeader(const QString & bgColor,
                                       const QString & barColor,
                                       const QString & stringColor,
                                       const QString & nickName){

    QString html = ("<table border=0 width=100% cellspacing=0 cellpading=0 height=30>");
    html+=QString("<tr>");
    html+=QString("<td bgcolor='%1'>").arg(bgColor);
    html+=QString("<font color = '%1'>").arg(stringColor);
    html+=QString("<b>%1</b></font>").arg(nickName);
    html+=QString("</td>");
    html+=QString("<td bgcolor='%1' align=right>").arg(bgColor);
    html+=QString("<font color = '%1'>").arg(stringColor);
    html+=QString("<b>%1</b></font>").arg(QTime::currentTime().toString());
    html+=QString("</td>");
    html+=QString("</tr>");
    html+=QString("<tr>");
    html+=QString("<td bgcolor='%1' height=4>").arg(barColor);
    html+=QString("</td>");
    html+=QString("</tr>");
    html+=QString("</table>");
    return html;

}
void ChatWidget::setupSendButton(){
    QPixmap pix = _sendButton->icon().pixmap(50,50);
    QPainter painter ( &pix );
    painter.setPen(Qt::white);
    painter.drawText(pix.rect(),Qt::AlignCenter,tr("send"));
    painter.end();
    _sendButton->setIcon(QIcon(pix));
}

void ChatWidget::setVisible ( bool visible ){
    QWidget::setVisible(visible);
    if ( visible )
        _chatEdit->setFocus();
}

