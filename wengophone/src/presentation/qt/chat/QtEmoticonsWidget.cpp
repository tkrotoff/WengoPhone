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

#include "QtEmoticonsWidget.h"
#include <WidgetFactory.h>


EmoticonsWidget::EmoticonsWidget(QWidget * parent, Qt::WFlags f) : QWidget(parent,f){
    
    _widget =WidgetFactory::create(":/forms/chat/emoticonswidget.ui", this);
    QGridLayout * layout = new QGridLayout();
    layout->addWidget(_widget);
    layout->setMargin(0);
    setLayout(layout);
    
	// Default stat is popup
	_stat=Popup;

    QPushButton * exitButton = _seeker.getPushButton(_widget, "emoticonCloseButton");
    
    connect(exitButton,SIGNAL(clicked()),this,SLOT(changeStat()));
    
    _iconName << "face-wink48.png";
    _iconName << "face-angel48.png";
    _iconName << "face-crying48.png";
    _iconName << "face-devil-grin48.png";
    _iconName << "face-glasses48.png";
    _iconName << "face-kiss48.png";
    _iconName << "face-plain48.png";
    _iconName << "face-sad48.png";
    _iconName << "face-smile48.png";
    _iconName << "face-smile-big48.png";
    _iconName << "face-surprise48.png";
    
    
    emoticonButton1 = _seeker.getPushButton(_widget,"emoticonButton1");
    emoticonButton2 = _seeker.getPushButton(_widget,"emoticonButton2");
    emoticonButton3 = _seeker.getPushButton(_widget,"emoticonButton3");
    emoticonButton4 = _seeker.getPushButton(_widget,"emoticonButton4");
    emoticonButton5 = _seeker.getPushButton(_widget,"emoticonButton5");
    emoticonButton6 = _seeker.getPushButton(_widget,"emoticonButton6");
    emoticonButton7 = _seeker.getPushButton(_widget,"emoticonButton7");
    emoticonButton8 = _seeker.getPushButton(_widget,"emoticonButton8");
    emoticonButton9 = _seeker.getPushButton(_widget,"emoticonButton9");
    emoticonButton10 = _seeker.getPushButton(_widget,"emoticonButton10");
    emoticonButton11 = _seeker.getPushButton(_widget,"emoticonButton11");
    
    connect(emoticonButton1,SIGNAL(clicked()),this,SLOT(emoticon1Clicked()));
    connect(emoticonButton2,SIGNAL(clicked()),this,SLOT(emoticon2Clicked()));
    connect(emoticonButton3,SIGNAL(clicked()),this,SLOT(emoticon3Clicked()));
    connect(emoticonButton4,SIGNAL(clicked()),this,SLOT(emoticon4Clicked()));
    connect(emoticonButton5,SIGNAL(clicked()),this,SLOT(emoticon5Clicked()));
    connect(emoticonButton6,SIGNAL(clicked()),this,SLOT(emoticon6Clicked()));
    connect(emoticonButton7,SIGNAL(clicked()),this,SLOT(emoticon7Clicked()));
    connect(emoticonButton8,SIGNAL(clicked()),this,SLOT(emoticon8Clicked()));
    connect(emoticonButton9,SIGNAL(clicked()),this,SLOT(emoticon9Clicked()));
    connect(emoticonButton10,SIGNAL(clicked()),this,SLOT(emoticon10Clicked()));
    connect(emoticonButton11,SIGNAL(clicked()),this,SLOT(emoticon11Clicked()));
    
}
void EmoticonsWidget::emoticon1Clicked()
{
	emoticonClicked(_iconName[0]);
	if (_stat == Popup)
		close();
}
void EmoticonsWidget::emoticon2Clicked()
{
	emoticonClicked(_iconName[1]);
	if (_stat == Popup)
		close();
}
void EmoticonsWidget::emoticon3Clicked()
{
    emoticonClicked(_iconName[2]);
	if (_stat == Popup)
		close();
}
void EmoticonsWidget::emoticon4Clicked()
{
    emoticonClicked(_iconName[3]);
	if (_stat == Popup)
		close();
}
void EmoticonsWidget::emoticon5Clicked()
{
    emoticonClicked(_iconName[4]);
	if (_stat == Popup)
		close();
}
void EmoticonsWidget::emoticon6Clicked()
{
    emoticonClicked(_iconName[5]);
	if (_stat == Popup)
		close();
}
void EmoticonsWidget::emoticon7Clicked()
{
    emoticonClicked(_iconName[6]);
	if (_stat == Popup)
		close();
}
void EmoticonsWidget::emoticon8Clicked()
{
    emoticonClicked(_iconName[7]);
	if (_stat == Popup)
		close();
}
void EmoticonsWidget::emoticon9Clicked()
{
    emoticonClicked(_iconName[8]);
	if (_stat == Popup)
		close();
}
void EmoticonsWidget::emoticon10Clicked()
{
    emoticonClicked(_iconName[9]);
	if (_stat == Popup)
		close();
}
void EmoticonsWidget::emoticon11Clicked()
{
    emoticonClicked(_iconName[10]);
	if (_stat == Popup)
		close();
}

void EmoticonsWidget::changeStat()
{
	if (_stat == Popup)
	{
		close();
		setWindowFlags(Qt::Window);
		_stat = Window;
		show();
	}
	else
	{
		close();
		setWindowFlags(Qt::Popup);
		_stat=Popup;
	}
}
