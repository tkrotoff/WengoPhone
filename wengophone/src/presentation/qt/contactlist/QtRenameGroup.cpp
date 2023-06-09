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

#include "QtRenameGroup.h"

#include "ui_RenameGroupDialog.h"

#include <QtGui/QtGui>

QtRenameGroup::QtRenameGroup(const QString & groupName, QWidget * parent)
	: QDialog(parent) {
	_ui = new Ui::RenameGroupDialog();
	_ui->setupUi(this);
	_ui->groupName->setText(groupName);
}

QtRenameGroup::~QtRenameGroup() {
	delete _ui;
}

QString QtRenameGroup::getGroupName() const {
	return _ui->groupName->text();
}

void QtRenameGroup::accept() {
	QString tmp;
	tmp = _ui->groupName->text().trimmed();
	if (tmp.isEmpty()) {
		QMessageBox::warning(this, tr("Bad group name"), tr("Group name cannot be empty"), 1, 0, 0);
		return;
	}
	QDialog::accept();
}
