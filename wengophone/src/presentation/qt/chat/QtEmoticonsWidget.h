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
#include "QtEmoticon.h"
#include "QtEmoticonButton.h"

class EmoticonsWidget : public QWidget //, Ui::EmoticonsWidget
{
    Q_OBJECT
    
public:
	enum EmoticonsWidgetStat {Window,Popup};
	
    EmoticonsWidget(QWidget * parent = 0, Qt::WFlags f= 0);
	
    void loadConfig(const QString & path);
	
	QVector<QtEmoticon> getEmoticonsVector() {return _emoticonsVector;};
	
protected:

	QWidget * _widget;
	
	WidgetSeeker _seeker;
	
	QStringList _iconName;
	
	EmoticonsWidgetStat _stat;

	int _buttonX;

	int _buttonY;

	QGridLayout * _layout;

	QVector<QtEmoticon> _emoticonsVector;
	
	void addButton(QtEmoticon emoticon);
	
	virtual void closeEvent ( QCloseEvent * event );
public Q_SLOTS:

	void changeStat(); 

	void buttonClicked(QtEmoticon emoticon);

Q_SIGNALS:

	void emoticonClicked(QtEmoticon emoticon);
	
	void closed();

	
};


#endif