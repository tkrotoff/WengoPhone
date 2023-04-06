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

#ifndef OWTCOIPMANAGER_H
#define OWTCOIPMANAGER_H

#include <coipmanager_threaded/tcoipmanagerdll.h>

#include <coipmanager/CoIpManagerConfig.h>

#include <util/NonCopyable.h>

#include <QtCore/QThread>

class CoIpManager;
class TCallSessionManager;
class TChatSessionManager;
class TConnectManager;
class TFileSessionManager;
class TSMSSessionManager;
class TUserProfileManager;

/**
 * @defgroup TCoIpManager Threaded CoIpManager
 *
 * Threaded CoIpManager launches an instance of CoIpManager in a Thread, and
 * then provide a non-blocking API to access CoIpManager. It will typically
 * be used by GUI layer.
 * It acts pretty like a Control layer from the PAC Pattern, or a Controller
 * layer from the MVC pattern.
 * It also manages Session memory: CoIpManager does not keep a link to created
 * Session and the user of the library is responsible for deleting such objects.
 * TCoIpManager manages this memory: if a new UserProfile is set, an event
 * will be emitted to say that a Session will be deleted. Then the receiver of
 * the event must tell TCoIpManager that it has released it. When finished,
 * the UserProfile will be set.
 */

/**
 * Entry point for Threaded CoIpManager.
 *
 * @see CoIpManager
 * @ingroup TCoIpManager
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class COIPMANAGER_THREADED_API TCoIpManager : public QThread, NonCopyable {
	Q_OBJECT

	friend class TAccountManager;
	friend class TCallSession;
	friend class TCallSessionManager;
	friend class TChatSession;
	friend class TChatSessionManager;
	friend class TConnectManager;
	friend class TContactManager;
	friend class TFileSessionManager;
	friend class TSendFileSession;
	friend class TSMSSessionManager;
	friend class TUserProfileFileStorage;
	friend class TUserProfileManager;
public:

	/**
	 * Default constructor.
	 *
	 * @param coIpManagerConfig config of CoIpManager, copied internally
	 */
	TCoIpManager(const CoIpManagerConfig & coIpManagerConfig);

	virtual ~TCoIpManager();

	/**
	 * Releases all TCoIpManager resources.
	 * To use TCoIpManager again, call TCoIpManager::start.
	 * This method blocks until all resources are freed.
	 */
	void release();

	/**
	 * @see CoIpManager::getCoIpManagerConfig()
	 */
	CoIpManagerConfig & getCoIpManagerConfig();

	/**
	 * @see CoIpManager::getCallSessionManager()
	 */
	TCallSessionManager & getTCallSessionManager();

	/**
	 * @see CoIpManager::getChatSessionManager()
	 */
	TChatSessionManager & getTChatSessionManager();

	/**
	 * @see CoIpManager::getConnectManager()
	 */
	TConnectManager & getTConnectManager();

	/**
	 * @see CoIpManager::getFileSessionManager()
	 */
	TFileSessionManager & getTFileSessionManager();

	/**
	 * @see CoIpManager::getSMSSessionManager()
	 */
	TSMSSessionManager & getTSMSSessionManager();

	/**
	 * @see CoIpManager::getUserProfileManager()
	 */
	TUserProfileManager & getTUserProfileManager();

	/**
	 * @see CoIpManager::getUserProfileSyncManager()
	 */
	//TUserProfileSyncManager & getTUserProfileSyncManager();

Q_SIGNALS:

	/**
	 * Emitted when TCoIpManager is initialized (all pointers are initalized, thus
	 * one can register to all signals).
	 */
	void initialized();

private:

	/** From QThread. */
	virtual void run();

	CoIpManager & getCoIpManager() const;

	/** Configuration passed to CoIpManager. */
	CoIpManagerConfig _coIpManagerConfig;

	CoIpManager * _coIpManager;

	TCallSessionManager * _tCallSessionManager;

	TChatSessionManager * _tChatSessionManager;

	TFileSessionManager * _tFileSessionManager;

	TSMSSessionManager * _tSMSSessionManager;

	TUserProfileManager * _tUserProfileManager;

	TConnectManager * _tConnectManager;
};

#endif	//OWTCOIPMANAGER_H
