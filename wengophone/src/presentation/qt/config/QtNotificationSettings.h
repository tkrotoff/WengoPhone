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

#ifndef QTNOTIFICATIONSETTINGS_H
#define QTNOTIFICATIONSETTINGS_H

#include "QtISettings.h"

#include <QtCore/QObject>

class QWidget;
class QString;
namespace Ui { class NotificationSettings; }

#include <string>

/**
 * Loads and display the notification settings configuration page.
 *
 * @author Tanguy Krotoff
 */
class QtNotificationSettings : public QObject, public QtISettings {
	Q_OBJECT
public:

	QtNotificationSettings(QWidget * parent);

	virtual ~QtNotificationSettings();

	QWidget * getWidget() const {
		return _notificationSettingsWidget;
	}

	QString getName() const;

	void saveConfig();

private Q_SLOTS:

	void soundComboBoxActivated(const QString & text);

	void browseSounds();

	void playSound();

	void clearSound();

private:

	void readConfig();

	void setSounds(const QString & soundFile);

	Ui::NotificationSettings * _ui;

	QWidget * _notificationSettingsWidget;

	std::string _incomingCallSoundFile;
	std::string _callClosedSoundFile;
	std::string _incomingChatSoundFile;
	std::string _imAccountConnectedSoundFile;
	std::string _imAccountDisconnectedSoundFile;
	std::string _contactOnlineSoundFile;
};

#endif	//QTNOTIFICATIONSETTINGS_H
