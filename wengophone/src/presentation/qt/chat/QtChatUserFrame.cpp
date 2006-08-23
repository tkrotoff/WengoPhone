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

#include "QtChatUserFrame.h"

#include "ui_UserFrame.h"

#include <QtGui/QtGui>

QtChatUserFrame::QtChatUserFrame(QWidget * parent)
	: QWidget(parent) {

	_ui = new Ui::UserFrame();
	_ui->setupUi(this);
}

void QtChatUserFrame::setPixmap(QPixmap pixmap) {

	QPixmap background = QPixmap(":/pics/fond_avatar.png");

	if (!pixmap.isNull()) {
		QPainter painter(& background);
		painter.drawPixmap(5, 5, pixmap.scaled(60, 60, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
		painter.end();
		_ui->avatarLabel->setPixmap(background);
	} else {
		_ui->avatarLabel->setPixmap(background);
	}
}
