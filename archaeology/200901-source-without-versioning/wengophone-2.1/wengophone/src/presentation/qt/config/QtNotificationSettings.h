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

#ifndef QTNOTIFICATIONSETTINGS_H
#define QTNOTIFICATIONSETTINGS_H

#include "QtISettings.h"

#include <QtGui/QWidget>

#include <string.h>

namespace Ui {
	class NotificationSettings;
	class NotificationSettingsItem;
}

/**
 * An item in the "list" of notification events
 *
 * @author Aurelien Gateau
 */
class QtNotificationSettingsItem : public QWidget {
	Q_OBJECT
public:
	QtNotificationSettingsItem(const std::string& key, const std::string& soundFile, const QString& caption);
	~QtNotificationSettingsItem();
	void init(QWidget*);
	void saveConfig();

private Q_SLOTS:
	void browseSounds();
	void playSound();
	void updatePlayButton();

private:
	Ui::NotificationSettingsItem* _ui;
	std::string _configKey;
	QString _soundFile;
	QString _caption;
};

/**
 * Loads and display the notification settings configuration page.
 *
 * @author Tanguy Krotoff
 * @author Aurelien Gateau
 */
class QtNotificationSettings : public QWidget, public QtISettings {
	Q_OBJECT
public:

	QtNotificationSettings(QWidget * parent);

	virtual ~QtNotificationSettings();

	virtual QString getName() const;

	virtual QString getTitle() const;

	virtual QString getIconName() const;

	virtual void saveConfig();

	virtual QWidget *getWidget() const {
		return (QWidget*)this;
	}

private:

	virtual void readConfig();

	void setSounds(const QString & soundFile);

	Ui::NotificationSettings* _ui;

	typedef QList<QtNotificationSettingsItem*> QtNotificationSettingsItemList;
	QtNotificationSettingsItemList _list;
};

#endif	//QTNOTIFICATIONSETTINGS_H
