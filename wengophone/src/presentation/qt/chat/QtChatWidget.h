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

#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QtGui>

#include "QtEmoticonsWidget.h"
#include "widgetseeker.h"
#include <WidgetFactory.h>

class ChatWidget : public QWidget //, Ui::ChatWidget
{
    Q_OBJECT
    
public:
    
    ChatWidget(QWidget * parent =0, Qt::WFlags f = 0);
    
    void            setNickBgColor(const QString &color);
    void            setNickTextColor(const QString &color);
    void            setNickFont(QFont &font);
    void            setNickName(const QString & nickname);
    const QString & nickName();
    const QFont&    nickFont();
    const QString&   nickBgColor();
    const QString&   nickTextColor();
    void             addToHistory(const QString & senderName,const QString & str);
protected:
    
    QWidget *       _widget;
    WidgetSeeker    _seeker;

    QPushButton  *   _fontButton;
    QTextBrowser *   _chatHistory;
    QTextEdit    *   _chatEdit;
    QPushButton  *   _emoticonsButton;
    QPushButton  *   _sendButton;

    
    
    QFont            _currentFont;
    QFont            _nickFont;
    QString          _nickBgColor;
    QString          _nickTextColor;
    QString          _nickName;
    
    EmoticonsWidget *_emoticonsWidget;
    const QString    replaceUrls(const QString & str, const QString & htmlstr);
public Q_SLOTS:
    void enterPressed();
    void chooseFont();
    void chooseEmoticon();
    void emoticonSelected(const QString & emoticonName);
    void urlClicked(const QUrl & link);
Q_SIGNALS:
    void newMessage(const QString & msg);
};

#endif
