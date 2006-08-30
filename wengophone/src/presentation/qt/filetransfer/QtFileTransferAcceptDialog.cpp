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

#include "QtFileTransferAcceptDialog.h"

#include "ui_AcceptDialog.h"

#include <util/Macro.h>

QtFileTransferAcceptDialog::QtFileTransferAcceptDialog(QWidget * parent) : QDialog(parent) {
	_ui = new Ui::AcceptDialog();
	_ui->setupUi(this);
}

QtFileTransferAcceptDialog::~QtFileTransferAcceptDialog() {
	SAFE_DELETE(_ui);
}

void QtFileTransferAcceptDialog::setFileName(const std::string & fileName) {
	_ui->filenameLabel->setText(QString::fromStdString(fileName));
}

void QtFileTransferAcceptDialog::setContactName(const std::string & contactName) {
	_ui->contactLabel->setText(QString::fromStdString(contactName));
}