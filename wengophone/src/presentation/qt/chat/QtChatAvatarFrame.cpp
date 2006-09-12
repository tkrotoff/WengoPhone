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

#include "QtChatAvatarFrame.h"
#include "QtChatAvatarWidget.h"

#include <QtGui/QtGui>

#include <stdio.h>

QtChatAvatarFrame::QtChatAvatarFrame(QWidget * parent)
	: QWidget(parent) {

	_ui.setupUi(this);
	setMinimumWidth(70);
	QGridLayout * glayout = dynamic_cast<QGridLayout *>(_ui.remoteAvatarFrame->layout());
	glayout->setSpacing(0);
	glayout->setMargin(0);
}

void QtChatAvatarFrame::setUserPixmap(QPixmap pixmap) {

	QPixmap background = QPixmap(":pics/avatar_background.png");

	if (!pixmap.isNull()) {
		QPainter painter(&background);
		painter.drawPixmap(5, 5, pixmap.scaled(60, 60, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
		painter.end();
		_ui.userAvatar->setPixmap(background);
	} else {
		_ui.userAvatar->setPixmap(background);
	}
}

void QtChatAvatarFrame::addRemoteContact(const QString & id, const QString & displayName, const QString & contactId, QPixmap avatar) {
	QtChatAvatarWidget * avatarWidget = new QtChatAvatarWidget(
		_ui.remoteAvatarFrame, id, avatar, displayName, contactId, QtChatAvatarWidget::BIG);
	_widgetList.append(avatarWidget);
	QGridLayout * glayout = dynamic_cast<QGridLayout *>(_ui.remoteAvatarFrame->layout());
	glayout->addWidget(avatarWidget, glayout->count(), 0);

	_ui.remoteAvatarFrame->setMinimumSize(0, glayout->count() * 70);
}

void QtChatAvatarFrame::removeRemoteContact(const QString & contactId) {

	for(int i = 0; i < _widgetList.size(); i++) {
		if(_widgetList[i]->getContactId() == contactId) {
			QGridLayout * glayout = dynamic_cast<QGridLayout *>(_ui.remoteAvatarFrame->layout());
			glayout->removeWidget(_widgetList[i]);
			glayout->update();
			delete _widgetList[i];
			_widgetList.removeAt(i);
		}
	}
}
