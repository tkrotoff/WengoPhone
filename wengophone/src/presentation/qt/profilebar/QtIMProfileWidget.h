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

#ifndef OWQTIMPROFILEWIDGET_H
#define OWQTIMPROFILEWIDGET_H

#include <imwrapper/EnumIMProtocol.h>

#include <util/Trackable.h>

#include <QtCore/QObject>

class CWengoPhone;
class CUserProfile;
class IMAccount;
class ToolTipLineEdit;

class QMenu;
class QWidget;
namespace Ui { class IMProfileWidget; }

/**
 * Shows a summary of the user profile + its IM accounts.
 *
 * Inside the profile bar at the top of the main window.
 *
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class QtIMProfileWidget : public QObject, public Trackable {
	Q_OBJECT
public:

	QtIMProfileWidget(CUserProfile & userProfile, CWengoPhone & cWengoPhone, QWidget * parent);

	~QtIMProfileWidget();

	QWidget * getWidget() const;

public Q_SLOTS:

	void connected(IMAccount * pImAccount);

	void disconnected(IMAccount * pImAccount, bool connectionError, const QString & reason);

	void connectionProgress(IMAccount * pImAccount, int currentStep, int totalSteps, const QString & infoMessage);

private Q_SLOTS:

	void msnClicked();

	void yahooClicked();

	void wengoClicked();

	void aimClicked();

	void jabberClicked();

	void editProfileClicked();

	void changeAvatarClicked();

	void aliasTextChanged();

private:

	/**
	 * @see UserProfile::profileChangedEvent
	 */
	void profileChangedEventHandler();

	void updateAvatar();

	/**
	 * Internal code factorization.
	 */
	void showIMMenu(EnumIMProtocol::IMProtocol imProtocol);

	/**
	 * Internal code factorization.
	 */
	void imButtonClicked(EnumIMProtocol::IMProtocol imProtocol);

	void showImAccountManager();

	void init();

	Ui::IMProfileWidget * _ui;

	QWidget * _imProfileWidget;

	CUserProfile & _cUserProfile;

	CWengoPhone & _cWengoPhone;
};

#endif	//OWQTIMPROFILEWIDGET_H
