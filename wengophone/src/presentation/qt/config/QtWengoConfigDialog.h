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

#ifndef QTWENGOCONFIGDIALOG_H
#define QTWENGOCONFIGDIALOG_H

#include <util/NonCopyable.h>
#include <util/List.h>

#include <QObject>

class CWengoPhone;

class QtISettings;

class QDialog;
class QWidget;
namespace Ui { class WengoConfigDialog; }

/**
 * Main configuration window.
 *
 * @author Tanguy Krotoff
 */
class QtWengoConfigDialog : public QObject, NonCopyable {
	Q_OBJECT
public:

	QtWengoConfigDialog(CWengoPhone & cWengoPhone, QWidget * parent);

	~QtWengoConfigDialog();

public Q_SLOTS:

	void save();

	void show();

	void showGeneralPage();

	void showLanguagePage();

	void showAccountsPage();

	void showPrivacyPage();

	void showAudioPage();

	void showVideoPage();

	void showNotificationsPage();

	void showCallForwardPage();

	void showAdvancedPage();

private Q_SLOTS:

	void itemActivated();

private:

	Ui::WengoConfigDialog * _ui;

	QDialog * _configDialog;

	typedef List<QtISettings *> SettingsList;

	SettingsList _settingsList;
};

#endif	//QTWENGOCONFIGDIALOG_H
