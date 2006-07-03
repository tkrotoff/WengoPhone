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
#include "QtEmoticonsManager.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/Logger.h>

#include <cutil/global.h>

#include <qtutil/WidgetFactory.h>

#include <QtXml>


EmoticonsWidget::EmoticonsWidget(QtEmoticonsManager * qtEmoticonsManager, QWidget * parent, Qt::WFlags f) : QWidget(parent,f){
	_layout = NULL;
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	_qtEmoticonsManager = qtEmoticonsManager;
	_qtEmoticonsManager->loadFromFile(QString::fromStdString(config.getResourcesDir() + "emoticons/icondef2.xml"));
	_stat=Popup;
	_buttonX = 0;
	_buttonY = 0;
}

void EmoticonsWidget::buttonClicked(QtEmoticon emoticon) {
	if (_stat == Popup) {
		close();
	}
	emoticonClicked(emoticon);
}

void EmoticonsWidget::changeStat() {
	if (_stat == Popup) {
		close();
		setWindowFlags(Qt::Window);
		_stat = Window;
		show();
	} else {
		close();
		setWindowFlags(Qt::Popup);
		_stat=Popup;
	}
}

void EmoticonsWidget::initButtons(const QString & protocol) {
	QtEmoticonsManager::QtEmoticonsList emoticonsList;
	QtEmoticonsManager::QtEmoticonsList::iterator it;
	if (_layout) {
		delete _layout;
	}
	_layout = new QGridLayout(this);
	_layout->setMargin(0);
	_buttonX=0;
	_buttonY=0;
	emoticonsList = _qtEmoticonsManager->getQtEmoticonsList(protocol);
	for (it=emoticonsList.begin();it!=emoticonsList.end();it++) {
		addButton((*it));
	}
}

void EmoticonsWidget::addButton(QtEmoticon emoticon) {
	if ( _buttonX == 10 ) {
		_buttonX=0;
		_buttonY+=1;
	}
	QtEmoticonButton * button = new QtEmoticonButton();
	button->setEmoticon(emoticon);
	QSize buttonSize = emoticon.getButtonPixmap().size();
	#if defined(OS_MACOSX)
	QSize macosxHackSize(6, 6);
	buttonSize += macosxHackSize;
	#endif
	button->setMaximumSize(buttonSize);
	button->setMinimumSize(buttonSize);
	_layout->addWidget(button, _buttonY, _buttonX);
	connect (button, SIGNAL(buttonClicked(QtEmoticon)), this, SLOT(buttonClicked(QtEmoticon)));
	_buttonX++;
}

void EmoticonsWidget::closeEvent (QCloseEvent * event) {
	closed();
	event->accept();
}

