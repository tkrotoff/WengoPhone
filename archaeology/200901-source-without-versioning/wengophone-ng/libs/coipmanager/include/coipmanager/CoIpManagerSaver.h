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

#ifndef OWCOIPMANAGERSAVER_H
#define OWCOIPMANAGERSAVER_H

#include <coipmanager/ICoIpManager.h>

#include <coipmanager_base/EnumUpdateSection.h>
#include <coipmanager_base/userprofile/UserProfile.h>

#include <QtCore/QMutex>

#include <time.h>

#include <string>

/**
 * Saves CoIpManager data when they changed. Save is temporized by 5 secs
 * (two consecutive saves are separated by 5 secs).
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class COIPMANAGER_API CoIpManagerSaver : ICoIpManager {
	Q_OBJECT
public:

	CoIpManagerSaver(CoIpManager & coIpManager);

	~CoIpManagerSaver();

private Q_SLOTS:

	void dataChangedSlot(EnumUpdateSection::UpdateSection section);

	void userProfileUpdatedSlot(UserProfile);

	void accountAddedSlot(std::string);

	void accountRemovedSlot(std::string);

	void accountUpdatedSlot(std::string, EnumUpdateSection::UpdateSection section);

	void contactAddedSlot(std::string);

	void contactRemovedSlot(std::string);

	void contactUpdatedSlot(std::string, EnumUpdateSection::UpdateSection section);

private:

	virtual ICoIpManagerPlugin * getICoIpManagerPlugin(const Account & account, const ContactList & contactList) const;

	virtual void initialize();

	virtual void uninitialize();

	/**
	 * Asks for save. This method checks if interval between two saves is respected.
	 */
	void askForSave();
	
	/**
	 * Saves configuration and profiles.
	 */
	void save();

	time_t _lastSaveTime;
	
	QMutex * _mutex;
};

#endif //OWCOIPMANAGERSAVER_H
