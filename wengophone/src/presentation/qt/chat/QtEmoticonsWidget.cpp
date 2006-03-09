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
#include "QtEmoticonButton.h"

#include <WidgetFactory.h>
#include <QtXml>
#include <Logger.h>

EmoticonsWidget::EmoticonsWidget(QWidget * parent, Qt::WFlags f) : QWidget(parent,f){
    
    _layout = new QGridLayout(this);
    _layout->setMargin(0);
    loadConfig("emoticons/icondef.xml");

	// Default stat is popup
	_stat=Popup;

//    QPushButton * exitButton = _seeker.getPushButton(_widget, "emoticonCloseButton");
    
//    connect(exitButton,SIGNAL(clicked()),this,SLOT(changeStat()));

}

void EmoticonsWidget::buttonClicked(QtEmoticon emoticon)
{
	if (_stat == Popup)
		close();
	emoticonClicked(emoticon);
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

void EmoticonsWidget::loadConfig(const QString & path)
{
	//regexp = QString("(\\b(%1))|((%2)\\b)").arg(regexp).arg(regexp);
	_buttonX = 0;
	_buttonY = 0;
	
	QFile file(path);
	QDomDocument doc("wengoIcons");
	
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();
	
	QDomElement docElem = doc.documentElement();

	QDomNode n = docElem.firstChild();
	
	QtEmoticon emoticon;
	
	QStringList textList;

	while(!n.isNull()) {
        QDomElement e = n.toElement(); 
        if(!e.isNull()) {
			if (e.tagName() == "icon")
			{
				textList.clear();
				QDomNode n1 = e.firstChild();
				while ( ! n1.isNull() )
				{
					QDomElement e1 = n1.toElement();
					if ( !e1.isNull()){
						if (e1.tagName() == "text"){
							textList << e1.text();
						}
						if (e1.tagName() == "object"){
							emoticon.setPath("emoticons/"+e1.text());
							emoticon.setPixmap(QPixmap("emoticons/"+e1.text()));
						}
						if (e1.tagName() == "regexp"){
							emoticon.setRegExp(e1.text());
						}
						if (e1.tagName() == "button"){
							emoticon.setButtonPixmap(QPixmap("emoticons/"+e1.text()));
						}
					}
					n1 = n1.nextSibling();
				}
			}
        }
		emoticon.setText(textList);
		_emoticonsVector.append(emoticon);
		addButton(emoticon);
        n = n.nextSibling();
    }
}

void EmoticonsWidget::addButton(QtEmoticon emoticon){
	
	if ( _buttonX == 3 ){
		_buttonX=0;
		_buttonY+=1;
	}
	QtEmoticonButton * button = new QtEmoticonButton(this);
	button->setEmoticon(emoticon);
	button->setMaximumSize(emoticon.getButtonPixmap().size());
	button->setMinimumSize(emoticon.getButtonPixmap().size());
	_layout->addWidget( button, _buttonY, _buttonX);
	connect (button,SIGNAL(buttonClicked(QtEmoticon )),this,SLOT(buttonClicked(QtEmoticon )));
	_buttonX++;
}

void EmoticonsWidget::closeEvent ( QCloseEvent * event )
{
	closed();
	event->accept();
}

