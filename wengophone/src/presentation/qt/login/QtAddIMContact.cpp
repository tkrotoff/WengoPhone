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

#include "QtAddIMContact.h"

#include <imwrapper/IMAccount.h>

#include "ui_AddIMContactDialog.h"

using namespace std;

QtAddIMContact::QtAddIMContact(EnumIMProtocol::IMProtocol protocol, set<IMAccount *> imAccounts, QWidget * parent)
: QDialog(parent) {
	_imAccounts = imAccounts;
	
	_ui = new Ui::AddIMContactDialog();
	_ui->setupUi(this);

	init();
}

void QtAddIMContact::init() {
	unsigned j = 0;
	for (set<IMAccount *>::const_iterator it = _imAccounts.begin();
		it != _imAccounts.end();
		++it, ++j) {
		QTreeWidgetItem * item = new QTreeWidgetItem(_ui->accountsTree);
		// Check the first element
		if (j == 0) {
			item->setCheckState(0, Qt::Checked);
		} else {
			item->setCheckState(0, Qt::Unchecked);
		}
		item->setText(1, QString::fromStdString((*it)->getLogin()));
	}
}

QString QtAddIMContact::contactId() const {
	return _ui->contactIdEdit->text();
}

set<IMAccount *> QtAddIMContact::selectedIMAccount() const {
	set<IMAccount *> result;

	for (set<IMAccount *>::const_iterator it = _imAccounts.begin();
		it != _imAccounts.end();
		++it) {
		QList<QTreeWidgetItem *> list =
			_ui->accountsTree->findItems(QString::fromStdString((*it)->getLogin()), Qt::MatchExactly, 1);
		if (list.size() > 0) {
			// There should be only one item
			QTreeWidgetItem * item = list[0];
			if (item->checkState(0) == Qt::Checked) {
				result.insert(*it);
			}
		}
	}

	return result;
}
