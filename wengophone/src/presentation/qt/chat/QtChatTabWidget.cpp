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

#include <util/Logger.h>

QtChatTabWidget::QtChatTabWidget(QWidget * parent) : QTabWidget (parent) {
	_currentColor = Qt::black;
	_timerId = startTimer(500);
	_alphaPixmap = QPixmap(QSize(16,16));
	_alphaPixmap.fill(Qt::transparent);
}

void QtChatTabWidget::setBlinkingTab(int index) {
	_blinkingTabIndex.insert(index);
	_blinkingTabIcons[index]=tabBar()->tabIcon(index);
}

void QtChatTabWidget::stopBlinkingTab(int index) {
	tabBar()->setTabTextColor(index,Qt::black);

	if (_blinkingTabIcons.find(index) != _blinkingTabIcons.end()) {
		tabBar()->setTabIcon(index,_blinkingTabIcons[index]);
		_blinkingTabIcons.remove(index);
	}
	_blinkingTabIndex.remove(index);
}

bool QtChatTabWidget::isBlinkingTab(int index) {
	if (_blinkingTabIndex.contains(index))
		return true;
	return false;
}

void QtChatTabWidget::timerEvent (QTimerEvent * event) {
	if ( _currentColor == Qt::black ) {
		_currentColor = Qt::red;
	} else {
		_currentColor = Qt::black;
	}

	BlinkingTabIndex::const_iterator it;

	for (it = _blinkingTabIndex.begin(); it != _blinkingTabIndex.end(); it++) {
		tabBar()->setTabTextColor((*it),_currentColor);

		if (_currentColor == Qt::red) {
			tabBar()->setTabIcon((*it),QIcon(_alphaPixmap));
		} else {
			tabBar()->setTabIcon((*it),_blinkingTabIcons[(*it)]);
		}
	}
}

bool QtChatTabWidget::event(QEvent *event) {
	if (event->type() == QEvent::KeyPress){
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
		if (keyEvent->key() == Qt::Key_Tab) {
			if ( (keyEvent->modifiers() & Qt::ControlModifier) == Qt::ControlModifier) {
				ctrlTabPressed();
				keyEvent->accept();
				return true;
			}
		}
	}
	return QTabWidget::event(event);
}

void QtChatTabWidget::ctrlTabPressedSlot() {
	ctrlTabPressed();
}
