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

#ifndef OWIMCONTACTDIALOG_H
#define OWIMCONTACTDIALOG_H

#include <coipmanager_base/EnumAccountType.h>

#include <QtGui/QDialog>

namespace Ui { class IMContactDialog; }

/**
 *
 * @author Philippe Bernery
 */
class IMContactDialog : public QDialog {
	Q_OBJECT
public:

	IMContactDialog(QWidget * parent);

	~IMContactDialog();

	void setContactId(const QString & contactId);
	QString contactId() const;

	void setAccountType(EnumAccountType::AccountType accountType);
	EnumAccountType::AccountType accountType() const;

private:

	void initializeWidgets();

	Ui::IMContactDialog * _ui;
};

#endif	//OWIMCONTACTDIALOG_H
