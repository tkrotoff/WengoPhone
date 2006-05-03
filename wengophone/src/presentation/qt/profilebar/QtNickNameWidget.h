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

#ifndef QTNICKNAMEWIDGET_H
#define QTNICKNAMEWIDGET_H

#include <QtGui>
#include "QtClickableLabel.h"

class CWengoPhone;
class UserProfile;
class IMAccount;

class QtNickNameWidget : public QWidget
{
	Q_OBJECT

public:

	QtNickNameWidget (UserProfile & userProfile, CWengoPhone & cWengoPhone, QWidget * parent = 0, Qt::WFlags f = 0);

protected:

	QtClickableLabel * _msnLabel;

	QtClickableLabel * _yahooLabel;

	QtClickableLabel * _wengoLabel;

	QtClickableLabel * _aimLabel;

	QtClickableLabel * _jabberLabel;

	QtClickableLabel * _avatarLabel;

	QLineEdit * _nickNameEdit;

	QGridLayout * _protocolLayout;

	QGridLayout * _widgetLayout;

	QMenu * _msnIMAccountMenu;

	QMenu * _yahooIMAccountMenu;

	QMenu * _wengoIMAccountMenu;

	QMenu * _aimIMAccountMenu;

	QMenu * _jabberIMAccountMenu;

	UserProfile & _userProfile;

	CWengoPhone & _cWengoPhone;

	void showMsnMenu();

	void showYahooMenu();

	void showWengoMenu();

	void showAimMenu();

	void showJabberMenu();

	/**
	 * Initializes the widgets.
	 *
	 * Full the widgets with information container in UserProfile.
	 */
	void init();

public Q_SLOTS:

	void msnClicked();

	void yahooClicked();

	void wengoClicked();

	void aimClicked();

	void jabberClicked();

	void avatarClicked();

	void nicknameChanged();

	void connected(IMAccount * pImAccount);

	void disconnected(IMAccount * pImAccount);
};

#endif
