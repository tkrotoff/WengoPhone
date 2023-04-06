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

#ifndef OWCOIPMANAGER_H
#define OWCOIPMANAGER_H

#include <coipmanager/CoIpManagerConfig.h>

#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>

#include <list>
#include <vector>

class AccountPresenceManager;
class CallSessionManager;
class ChatSessionManager;
class CoIpManagerSaver;
class CoIpManagerUser;
class ConnectManager;
class ContactPresenceManager;
class ContactSyncManager;
class FileSessionManager;
class SMSSessionManager;
class UserProfileManager;
class ICoIpManagerInitPlugin;

/**
 * @mainpage CoIpManager: Communication over IP Manager
 *
 * CoIpManager is a communication framework.
 * See http://dev.openwengo.org/trac/openwengo/trac.cgi/wiki/CoIpManager
 * for more information.
 */

/**
 * @defgroup CoIpManager CoIpManager
 *
 * This group gather CoIpManager model classes. These classes can be used
 * directly however methods are not threaded (though they are thread-safe).
 * If you want to code a GUI above CoIpManager, classes in TCoIpManager
 * group should be used. Every method than can threaded are threaded.
 *
 * CoIpManager is separated in several groups. Here is a list of them,
 * by dependency, with the classes that are part of this group:
 *
 * - DataManager: AccountManager, ContactManager and UserProfileManager
 *  - ConnectManager: ConnectManager
 *   - CallSessionManager: CallSessionManager, CallSession
 *   - ChatSessionManager: ChatSessionManager, ChatSession
 *   - FileSessionManager: FileSessionManager, FileSession
 *   - SMSSessionManager: SMSSessionManager, SMSSession
 *   - SyncManager: AccountSyncManager, ContactSyncManager
 *     and UserProfileSyncManager
 */

/**
 * Entry point for CoIpManager model.
 *
 * WARNING: CoIpManager must be initialized in two steps (for event synchronization):
 * - first, call CoIpManager constructor
 * - second, call CoIpManager::initialize
 *
 * <pre>
 * //CoIpManager configuration
 * CoIpManagerConfig config;
 *
 * //CoIpManager first initialization step
 * CoIpManager * coIpManager = new CoIpManager(config);
 * coIpManager->initialize();
 *
 * //CoIpManager second initialization step
 * UserProfile userProfile;
 * coIpManager->getUserProfileManager().setUserProfile(userProfile);
 * Account account("mylogin", "mypassword", EnumAccountType::AccountTypeSIP);
 * coIpManager->getUserProfileManager().getAccountManager().add(account);
 *
 * //Starts CoIpManager connection module
 * coIpManager->getConnectManager().connect(account.getUUID());
 *
 * [...]
 *
 * //CoIpManager will stop when deleted
 * delete coIpManager;
 * </pre>
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 */
class CoIpManager {
	friend class CoIpManagerUser;
	friend class UserProfileManager;
public:

	/**
	 * Default constructor.
	 *
	 * @param coIpManagerConfig config of CoIpManager. Copied internally.
	 */
	COIPMANAGER_API CoIpManager(const CoIpManagerConfig & coIpManagerConfig);

	COIPMANAGER_API ~CoIpManager();

	/**
	 * Initializes CoIpManager.
	 */
	COIPMANAGER_API void initialize();

	COIPMANAGER_API CoIpManagerConfig & getCoIpManagerConfig();

	COIPMANAGER_API CallSessionManager & getCallSessionManager();

	COIPMANAGER_API ChatSessionManager & getChatSessionManager();

	COIPMANAGER_API ConnectManager & getConnectManager();

	COIPMANAGER_API FileSessionManager & getFileSessionManager();

	COIPMANAGER_API SMSSessionManager & getSMSSessionManager();

	COIPMANAGER_API UserProfileManager & getUserProfileManager();

private:

	/**
	 * Loads external libraries.
	 */
	void loadExtLibraries();

	/**
	 * Initializes external libraries.
	 */
	void initExtLibraries();

	/**
	 * Uninitializes external libraries.
	 */
	void uninitExtLibraries();

	/**
	 * Called before a UserProfile is set.
	 */
	void initializeCoIpManager();
	
	/**
	 * Called before a UserProfile is unset.
	 *
	 * Will call coIpManagerWillBeReset() on each registered CoIpManagerUser and
	 * wait for all user calling released().
	 *
	 * thread-safe
	 */
	void releaseCoIpManager();

	/**
	 * thread-safe
	 *
	 * @return true if CoIpManager is fully released.
	 */
	bool isReleased() const;

	CallSessionManager * _callSessionManager;

	ChatSessionManager * _chatSessionManager;

	ConnectManager * _connectManager;

	FileSessionManager * _fileSessionManager;

	SMSSessionManager * _smsSessionManager;

	UserProfileManager * _userProfileManager;

	ContactSyncManager * _contactSyncManager;

	CoIpManagerConfig _coIpManagerConfig;

	CoIpManagerSaver * _coIpManagerSaver;

	AccountPresenceManager * _accountPresenceManager;

	ContactPresenceManager *_contactPresenceManager;

	/**
	 * @name Methods used by CoIpManagerUser
	 * @{
	 */

	/**
	 * Called by a CoIpManagerUser to tell CoIpManager that this user
	 * does not use the object anymore and thus can free its resources.
	 *
	 * thread-safe
	 *
	 * @param user method caller
	 */
	void retained(CoIpManagerUser * user);

	/**
	 * Called by a CoIpManagerUser to tell CoIpManager that this user
	 * does not use the object anymore and thus can free its resources.
	 *
	 * thread-safe
	 *
	 * @param user method caller
	 */
	void released(CoIpManagerUser * user);

	/**
	 * Called by a CoIpManagerUser to register itself as a user of CoIpManager.
	 *
	 * thread-safe
	 *
	 * @param user CoIpManagerUser to register
	 */
	void registerCoIpManagerUser(CoIpManagerUser * user);

	/**
	 * Called by a CoIpManagerUser to unregister.
	 *
	 * thread-safe
	 *
	 * @param user CoIpManagerUser to unregister
	 */
	void unregisterCoIpManagerUser(CoIpManagerUser * user);

	QWaitCondition _condition;

	QMutex * _conditionMutex;

	mutable QMutex * _mutex;

	/**
	 * first: pointer to CoIpManagerUser
	 * second: boolean saying that CoIpManagerUser is currently using CoIpManager or not
	 */
	typedef std::list< std::pair<CoIpManagerUser *, bool> > CoIpManagerUserList;

	/** List of registered CoIpManagerUser. */
	CoIpManagerUserList _coIpManagerUserList;

	static CoIpManagerUserList::iterator
		findCoIpManagerUser(CoIpManagerUserList & list, const CoIpManagerUser * user);

	/**
	 * @}
	 */

	/** Vector of available ICoIpManagerInitPlugin. */
	std::vector<ICoIpManagerInitPlugin *> _coIpManagerInitPluginVector;
};

#endif	//OWCOIPMANAGER_H
