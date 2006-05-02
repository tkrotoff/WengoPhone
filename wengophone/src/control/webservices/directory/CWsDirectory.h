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

#ifndef CWSDIRECTORY_H
#define CWSDIRECTORY_H

#include <model/webservices/directory/WsDirectory.h>

class CWengoPhone;
class PWsDirectory;

/**
 *
 * @ingroup control
 * @author Mathieu Stute
 */
class CWsDirectory {

public:

	CWsDirectory(CWengoPhone & cWengoPhone, WsDirectory& wsDirectory);

	~CWsDirectory();

	/**
	 * Emitted when a contact has been found.
	 *
	 * @param sender WsDirectory
	 * @param profile profile
	 */
	Event< void(WsDirectory & sender, Profile * profile, bool online) > contactFoundEvent;

	void searchEntry(const std::string & query, WsDirectory::Criteria criteria = WsDirectory::none);

	CWengoPhone& getCWengoPhone();

private:

	WsDirectory& _wsDirectory;

	CWengoPhone& _cWengoPhone;
	
	PWsDirectory * _pWsDirectory;
};

#endif /* CWSDIRECTORY_H */
