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

#ifndef EMOTICONSWIDGET_H
#define EMOTICONSWIDGET_H

#include <QtGui>

#include "widgetseeker.h"

class EmoticonsWidget : public QWidget //, Ui::EmoticonsWidget
{
    Q_OBJECT
    
public:
	enum EmoticonsWidgetStat {Window,Popup};
    EmoticonsWidget(QWidget * parent = 0, Qt::WFlags f= 0);
    
protected:

    void    *   setupGui();

    QWidget * _widget;
    WidgetSeeker _seeker;
    QStringList _iconName;
    EmoticonsWidgetStat _stat;
    
	QPushButton * emoticonButton1;
	QPushButton * emoticonButton2;
	QPushButton * emoticonButton3;
	QPushButton * emoticonButton4;
	QPushButton * emoticonButton5;
	QPushButton * emoticonButton6;
	QPushButton * emoticonButton7;
	QPushButton * emoticonButton8;
	QPushButton * emoticonButton9;
	QPushButton * emoticonButton10;
	QPushButton * emoticonButton11;
    
public Q_SLOTS:
	void emoticon1Clicked();
	void emoticon2Clicked();
	void emoticon3Clicked();
	void emoticon4Clicked();
	void emoticon5Clicked();
	void emoticon6Clicked();
	void emoticon7Clicked();
	void emoticon8Clicked();
	void emoticon9Clicked();
	void emoticon10Clicked();
	void emoticon11Clicked();
    void changeStat(); 
Q_SIGNALS:
    void emoticonClicked(const QString & emoticonName);
    
};


#endif