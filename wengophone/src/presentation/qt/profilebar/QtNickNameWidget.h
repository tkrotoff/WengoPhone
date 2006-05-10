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
#include <qtutil/QtClickableLabel.h>

class CWengoPhone;
class CUserProfile;
class IMAccount;
class ToolTipLineEdit;

/**
 *
 * @author Mr K
 * @author Mathieu Stute
 */
class QtNickNameWidget : public QWidget {
	Q_OBJECT
public:

	QtNickNameWidget(CUserProfile & userProfile, CWengoPhone & cWengoPhone, QWidget * parent = 0, Qt::WFlags f = 0);

public Q_SLOTS:

	void msnClicked();

	void yahooClicked();

	void wengoClicked();

	void aimClicked();

	void jabberClicked();

	void avatarRightClicked();

	void avatarClicked();

	void nicknameChanged();

	void textChanged ( const QString & text );

	void connected(IMAccount * pImAccount);

	void disconnected(IMAccount * pImAccount);

	void userProfileUpdated();

protected:

	void updateAvatar();

	void showMsnMenu();

	void showYahooMenu();

	void showWengoMenu();

	void showAimMenu();

	void showJabberMenu();

	void showImAccountManager();

	void init();

	QtClickableLabel * _msnLabel;

	QtClickableLabel * _yahooLabel;

	QtClickableLabel * _wengoLabel;

	QtClickableLabel * _aimLabel;

	QtClickableLabel * _jabberLabel;

	QtClickableLabel * _avatarLabel;

	ToolTipLineEdit * _nickNameEdit;

	QGridLayout * _protocolLayout;

	QGridLayout * _widgetLayout;

	QMenu * _msnIMAccountMenu;

	QMenu * _yahooIMAccountMenu;

	QMenu * _wengoIMAccountMenu;

	QMenu * _aimIMAccountMenu;

	QMenu * _jabberIMAccountMenu;

	CUserProfile & _cUserProfile;

	CWengoPhone & _cWengoPhone;

	static const double POPUPMENU_OPACITY;

	static const QString PICS_MSN_ON;
	static const QString PICS_MSN_OFF;

	static const QString PICS_YAHOO_ON;
	static const QString PICS_YAHOO_OFF;

	static const QString PICS_WENGO_ON;
	static const QString PICS_WENGO_OFF;

	static const QString PICS_AIM_ON;
	static const QString PICS_AIM_OFF;

	static const QString PICS_JABBER_ON;
	static const QString PICS_JABBER_OFF;
};

#endif
