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

#ifndef OWCOIPMANAGERUSER_H
#define OWCOIPMANAGERUSER_H

#include <coipmanager/coipmanagerdll.h>

#include <util/Interface.h>

class CoIpManager;

/**
 * Each module that uses data provided should inherit this class.
 *
 * Inheriting this class will register the subclass to the CoIpManager users
 * database.
 * This class provides a pure virtual method that will be called when CoIpManager
 * will be reset (i.e when switching UserProfile). Then the subclass must
 * call _coIpManager->released(*this) when it can ensure that it will no use
 * the CoIpManager instance again until an init event (i.e a new UserProfile set).
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_API CoIpManagerUser : Interface {
	friend class CoIpManager;
public:

	CoIpManagerUser(CoIpManager & coIpManager);

	virtual ~CoIpManagerUser();

protected:

	/**
	 * Retains the CoIpManager instance.
	 */
	void retain();

	/**
	 * Releases the CoIpManager instance.
	 */
	void release();

	/**
	 * Tells CoIpManagerUser that CoIpManager is initializing.
	 */
	virtual void coIpManagerIsInitializing() = 0;

	/**
	 * Tells CoIpManagerUser to release CoIpManager.
	 */
	virtual void releaseCoIpManager() = 0;

	/** Link to CoIpManager. */
	CoIpManager & _coIpManager;
};

#endif	//OWCOIPMANAGERUSER_H
