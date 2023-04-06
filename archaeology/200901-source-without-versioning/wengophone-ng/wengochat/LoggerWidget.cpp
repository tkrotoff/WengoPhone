/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include "LoggerWidget.h"

#include "WengoChat.h"

#include "ui_LoggerWidget.h"

#include <util/SafeConnect.h>

#include <cutil/global.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <QtGui/QtGui>

LoggerWidget::LoggerWidget(QWidget * parent, WengoChat * wengoChat)
	: QWidget(parent) {

	_ui = new Ui::LoggerWidget();
	_ui->setupUi(this);

#ifdef OS_MACOSX
	_ui->log->setFontPointSize(9.0);
#endif

	/*
	FIXME does not exist anymore in Logger.h API because of crashes
	Logger::logger.messageAddedEvent.connect(this,
		boost::bind(&LoggerWidget::messageAddedEventHandler, this, _1),
		wengoChat->getIPostEvent());
	*/
}

LoggerWidget::~LoggerWidget() {
	OWSAFE_DELETE(_ui);
}

void LoggerWidget::messageAddedEventHandler(std::string message) {
	QTextCursor curs(_ui->log->document());
	curs.movePosition(QTextCursor::End);
	_ui->log->setTextCursor(curs);

	_ui->log->insertPlainText(QString::fromStdString(message + "\n"));

	_ui->log->ensureCursorVisible();
}
