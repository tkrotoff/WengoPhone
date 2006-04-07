/*
* WengoPhone, a voice over Internet phone
* Copyright (C) 2004-2006  Wengo
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

#include "QtChatTabWidget.h"

QtChatTabWidget::QtChatTabWidget(QWidget * parent) : QTabWidget (parent){

	_currentColor = Qt::black;
	_timerId = startTimer(500);
}


void QtChatTabWidget::setBlinkingTab(int index){
	_blinkingTabIndex.append(index);
}

void QtChatTabWidget::stopBlinkingTab(int index){
	BlinkingTabIndex::iterator iter;

	for (iter = _blinkingTabIndex.begin(); iter != _blinkingTabIndex.end(); iter++){
		if ( (*iter) ==  index ){
			_blinkingTabIndex.erase(iter);
			tabBar()->setTabTextColor((*iter),Qt::black);
			return;
		}
	}
}

bool QtChatTabWidget::isBlinkingTab(int index){

	BlinkingTabIndex::iterator iter;
	for (iter = _blinkingTabIndex.begin(); iter != _blinkingTabIndex.end(); iter++){
		if ( (*iter) ==  index ){
			return true;
		}
	}
	return false;
}

void QtChatTabWidget::timerEvent ( QTimerEvent * event ){

	BlinkingTabIndex::iterator iter;

	if ( _currentColor == Qt::black )
		_currentColor = Qt::red;
	else
		_currentColor = Qt::black;

	for (iter = _blinkingTabIndex.begin(); iter != _blinkingTabIndex.end(); iter++){
		tabBar()->setTabTextColor((*iter),_currentColor);
	}
}
