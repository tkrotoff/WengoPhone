/*
* WengoPhone, a voice over Internet phone
* Copyright (C) 2004-2005  Wengo
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

#ifndef QTWENGOCONFIGDIALOG_H
#define QTWENGOCONFIGDIALOG_H

#include <QtGui>

// TODO: Change include files to class
#include "QtNotificationSettings.h"
#include "QtGeneralSettings.h"
#include "QtAccountSettings.h"
#include "QtPrivacySettings.h"
#include "QtAudioSettings.h"
#include "QtVideoSettings.h"
#include "QtAdvancedSettings.h"
#include "QtCallForwardSettings.h"
#include "QtLanguagesSettings.h"


class QtWengoConfigDialog : public QDialog {
		Q_OBJECT

	public:
		QtWengoConfigDialog ( QWidget *parent = 0, Qt::WFlags f = 0 );

	public Q_SLOTS:

		void itemActivated ( );

		void accept();

	protected:
		QWidget * _widget;

		QTreeWidget * _treeWidget;

		QStackedWidget * _stackedWidget;

		QtNotificationSettings * _notificationWidget;

		QtGeneralSettings * _generalSettingsWidget;

		QtAccountSettings * _accountSettingsWidget;

		QtPrivacySettings * _privacySettingsWidget;

		QtAudioSettings * _audioSettingsWidget;

		QtVideoSettings * _videoSettingsWidget;

		QtAdvancedSettings * _advancedSettingsWidget;

		QtCallForwardSettings * _callForwardWidget;

		QtLanguagesSettings * _languagesWidget;
	};

#endif
