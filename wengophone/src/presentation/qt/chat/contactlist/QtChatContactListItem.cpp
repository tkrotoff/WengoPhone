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

#include "QtChatContactListItem.h"

#include "ui_ChatContactListItem.h"

#include <QtGui/QtGui>

#include <util/Logger.h>

QtChatContactListItem::QtChatContactListItem(
	QWidget * parent, QPixmap picture, const QString & nickname,
	PictureMode pmode/*, NicknameMode nmode*/)
	: QWidget(parent), _pictureMode(pmode) {

	_ui = new Ui::ChatContactListItem();
	_ui->setupUi(this);
	setupPixmap(picture);
	setupNickname(nickname);
}

void QtChatContactListItem::setupPixmap(QPixmap pixmap) {

	QPixmap background = QPixmap(":/pics/fond_avatar.png");
	QPainter painter(& background);

	if (!pixmap.isNull()) {

		switch (_pictureMode) {
			case BIG:
				painter.drawPixmap(5, 5, pixmap.scaled(96, 96, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
				break;
			case MEDIUM:
				painter.drawPixmap(5, 5, pixmap.scaled(48, 48, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
				break;
			case SMALL:
				painter.drawPixmap(5, 5, pixmap.scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
				break;
			case TINY:
				painter.drawPixmap(5, 5, pixmap.scaled(12, 12, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
				break;
			default:
				LOG_WARN("unknown picture mode: " + String::fromNumber(_pictureMode));
		}
	}

	painter.end();
	_ui->pictureLabel->setPixmap(background);
}

void QtChatContactListItem::setupNickname(const QString & nickname) {
	//TODO: limit string length
	_ui->nicknameLabel->setText(nickname);
}
