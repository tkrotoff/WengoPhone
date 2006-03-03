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
#include "chatwidgetmanager.h"
#include "QtEmoticonsWidget.h"

ChatWidget::ChatWidget (QWidget * parent, Qt::WFlags f) : QWidget(parent, f)

{

    _widget =WidgetFactory::create(":/forms/chat/ChatWidget.ui", this);
    QGridLayout * layout = new QGridLayout();
    layout->addWidget(_widget);
    layout->setMargin(0);
    setLayout(layout);
    
    /* Defaults fonts and colors */
    _nickFont = QFont("Helvetica", 12);
    _nickTextColor = "'#000000'"; // Black
    _nickBgColor = "'#ffcc66'";
    _nickName = "Wengo";

    _fontButton  = _seeker.getPushButton(_widget,"fontButton");
    _chatHistory = _seeker.getTextBrowser(_widget,"chatHistory");
    _emoticonsButton = _seeker.getPushButton(_widget,"emoticonsButton");
    _sendButton = _seeker.getPushButton(_widget,"sendButton");
    _chatEdit = _seeker.getTextEdit(_widget,"chatEdit");
    
    ChatWidgetManager * cwm = new ChatWidgetManager(this,_chatEdit);
    
    connect (cwm,SIGNAL(enterPressed()),this, SLOT (enterPressed()));
    connect (_fontButton,SIGNAL(clicked()), this, SLOT (chooseFont()));
    connect (_chatHistory,SIGNAL(anchorClicked(const QUrl &)),this,SLOT(urlClicked(const QUrl & )));
    connect (_emoticonsButton,SIGNAL(clicked()),this,SLOT(chooseEmoticon()));
    connect (_sendButton,SIGNAL(clicked()),this,SLOT(enterPressed()));
    _chatHistory->setHtml ("<qt type=detail>");
    
    _emoticonsWidget = new EmoticonsWidget(this,Qt::Popup);
    
    connect(_emoticonsWidget,SIGNAL(emoticonClicked(const QString &)),this,SLOT(emoticonSelected(const QString&)));
}


void  ChatWidget::setNickName(const QString & nickname)
{
    _nickName = nickname;
}
const QString & ChatWidget::nickName()
{
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


void ChatWidget::addToHistory(const QString & senderName,const QString & str)
{

    QTextCursor curs(_chatHistory->document());
    curs.movePosition(QTextCursor::End);

    QString text= QString("<table border=0 width=100% cellspacing=0 "
    "cellpadding=0><tr><td BGCOLOR=%1> <font color=%2> %3 </font></td><td BGCOLOR=%4 align=right>"
    "<font color=%5> %6 </font></td></tr></table>").
    arg(_nickBgColor).
    arg(_nickTextColor).
    arg(senderName).
    arg(_nickBgColor).
    arg(_nickTextColor).
    arg(QTime::currentTime().toString());
    
    _chatHistory->insertHtml (text);
    _chatHistory->insertHtml (str);
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

void ChatWidget::enterPressed()
{
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
    
    _chatHistory->setTextCursor(curs);
    _chatHistory->insertHtml (text);
    _chatHistory->insertHtml (replaceUrls(_chatEdit->toPlainText(),_chatEdit->toHtml() + "<P></P>"));
    _chatHistory->ensureCursorVisible();
    
    newMessage(_imChatSession,_chatEdit->toHtml());
    
    _chatEdit->clear();
    _chatEdit->setFocus();
    
//    newMessage(replaceUrls(_chatEdit->toPlainText(),_chatEdit->toHtml() + "<P></P>"));
    
}

void ChatWidget::chooseFont()
{
    bool ok;
    _currentFont = QFontDialog::getFont(&ok,_currentFont,this);
    _chatEdit->setCurrentFont(_currentFont);
    _chatEdit->setFocus();
}

const QString  ChatWidget::replaceUrls(const QString & str, const QString & htmlstr)
{
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

void ChatWidget::chooseEmoticon()
{
	QPoint p = _emoticonsButton->pos();
	p.setY(p.y() + _emoticonsButton->rect().bottom());
	_emoticonsWidget->move(mapToGlobal(p));
	_emoticonsWidget->setWindowOpacity(0.85);	
    _emoticonsWidget->show();
}

void ChatWidget::emoticonSelected(const QString & emoticonName)
{
    QString path = QString("emoticons/") + emoticonName;
    QString image = QString("<img src='%1' />").arg(path);
    _chatEdit->insertHtml (image);
    _chatEdit->ensureCursorVisible();
}

void ChatWidget::setIMChatSession(IMChatSession * imChatSession)
{
	_imChatSession = imChatSession;
}
