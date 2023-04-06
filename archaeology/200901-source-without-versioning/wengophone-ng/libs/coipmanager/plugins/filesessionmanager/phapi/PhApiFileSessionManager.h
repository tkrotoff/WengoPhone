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

#ifndef OWPHAPIFILESESSIONMANAGER_H
#define OWPHAPIFILESESSIONMANAGER_H

#include <coipmanager/filesessionmanager/IFileSessionManagerPlugin.h>

class PhApiSFPWrapper;

/**
 * TODO create file session for receive
 *
 * @author Nicolas Couturier
 */
class PhApiFileSessionManager : public IFileSessionManagerPlugin {
	Q_OBJECT
public:

	PhApiFileSessionManager(CoIpManager & coIpManager);

	virtual ~PhApiFileSessionManager();

	ISendFileSessionPlugin * createISendFileSessionPlugin();

	virtual bool canCreateISession(const Account & account, const ContactList & contactList) const;

	virtual IMContactList getValidIMContacts(const Account & account, const ContactList & contactList) const;

private Q_SLOTS:
	
	void newIncomingFileSlot(int callId,
		const std::string & contactId,
		const std::string & fileName,
		const std::string & fileType, int fileSize);

	void needsUpgradeSlot();

	void peerNeedsUpgradeSlot(const std::string & contactId);
};

#endif	//OWPHAPIFILESESSIONMANAGER_H
