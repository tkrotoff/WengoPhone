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

#include "QtChatToaster.h"

#include "QtToaster.h"

#include "ui_QtChatToaster.h"

#include <QtGui>

QtChatToaster::QtChatToaster(QWidget * parent)
	: QObject(parent) {

	_chatToasterWidget = new QWidget(parent);

	_ui = new Ui::ChatToaster();
	_ui->setupUi(_chatToasterWidget);

	QPixmap pixmap(":pics/toaster/chat.png");
	_ui->chatButton->setMinimumSize(pixmap.size());
	_ui->chatButton->setMaximumSize(pixmap.size());
	_ui->chatButton->setIconSize(pixmap.size());
	_ui->chatButton->setFlat(true);
	_ui->chatButton->setIcon(QIcon(pixmap));

	connect(_ui->chatButton, SIGNAL(clicked()), SLOT(chatButtonSlot()));

	_toaster = new QtToaster(_chatToasterWidget, _ui->windowFrame);
}

QtChatToaster::~QtChatToaster() {
	delete _ui;
}

void QtChatToaster::setTitle(const QString & title) {
	_ui->titleLabel->setText(title);
}

void QtChatToaster::setMessage(const QString & message) {
	_ui->messageLabel->setText(message);
}

void QtChatToaster::setPixmap(const QPixmap & pixmap) {
	_ui->pixmapLabel->setPixmap(pixmap);
}

void QtChatToaster::show() {
	_toaster->show();
}

void QtChatToaster::close() {
	_toaster->close();
}

void QtChatToaster::chatButtonSlot() {
	chatButtonClicked();
	close();
}