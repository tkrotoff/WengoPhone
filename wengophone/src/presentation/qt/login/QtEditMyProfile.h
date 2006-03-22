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

#ifndef EDITMYPROFILE_H
#define EDITMYPROFILE_H

#include <QtGui>

#include <util/Logger.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>


class QtEditMyProfile : public QDialog
{
	Q_OBJECT

public:

	QtEditMyProfile (QWidget * parent =0, Qt::WFlags f=0);

protected:

	void init();

	void changeGroupBoxStat(QGroupBox * box, bool stat);

	void readFromConfig();

	void writeToConfig();

	void hideAccountWidgets();

public Q_SLOTS:
	void saveClicked();
	void cancelClicked();
	void imAccountAdded();
protected:

	QWidget * _widget;

	QGridLayout * layout;

	QLineEdit * _wengoNickName;

	QPushButton * _changePassword;

	QLineEdit * _firstName;

	QLineEdit * _lastName;

	QDateEdit * _birthDate;

	QLineEdit * _city;

	QComboBox * _gender;

	QComboBox * _country;

	QComboBox * _state;

	QList<QLabel *> _imAccountsPic;

	QList<QLineEdit *> _imAccountLineEdit;

	QList<QString> _imAccountsPicPath;

	QPushButton * _addIMAccount;

	QPushButton * _modifyIMAccount;

	QLineEdit * _cellphone;

	QLineEdit * _wengoPhone;

	QLineEdit * _homePhone;

	QLineEdit * _workPhone;

	QLineEdit * _email;

	QLineEdit * _blog;

	QLineEdit * _web;

	QLabel * _avatar;

	QString  _avatarPath;

	QComboBox * _changeAvatarButton;

	QPushButton * _saveChange;

	QPushButton * _cancelChange;


	QMap<QString,QVariant> _data;



};

#endif
