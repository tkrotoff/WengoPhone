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

#ifndef OWICOIPMANAGER_H
#define OWICOIPMANAGER_H

#include <coipmanager/CoIpManagerUser.h>

#include <QtCore/QMutex>
#include <QtCore/QObject>

#include <vector>

class CoIpManager;
class ICoIpManagerPlugin;

/**
 * Interface for Managers.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_API ICoIpManager : public QObject, public CoIpManagerUser {
public:

	/**
	 * Default constructor for all managers.
	 *
	 * @param coIpManager link to the CoIpManager
	 */
	ICoIpManager(CoIpManager & coIpManager);

	virtual ~ICoIpManager();

protected:

	/** Called when a UserProfile is set. */
	virtual void initialize();

	/** Called before a UserProfile is unset. */
	virtual void uninitialize();

	QMutex * _mutex;

	typedef std::vector<ICoIpManagerPlugin *> ICoIpManagerPluginList;
	ICoIpManagerPluginList _iCoIpManagerPluginList;

private:

	virtual void coIpManagerIsInitializing();

	virtual void releaseCoIpManager();
};

#endif	//OWICOIPMANAGER_H
