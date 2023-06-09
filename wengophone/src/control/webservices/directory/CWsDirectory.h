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

#ifndef OWCWSDIRECTORY_H
#define OWCWSDIRECTORY_H

#include <model/webservices/directory/WsDirectory.h>

#include <util/Trackable.h>

class CWengoPhone;
class PWsDirectory;
class ContactProfile;

/**
 *
 * @ingroup control
 * @author Mathieu Stute
 */
class CWsDirectory : public Trackable {
public:

	CWsDirectory(CWengoPhone & cWengoPhone, WsDirectory & wsDirectory);

	~CWsDirectory();

	/**
	 * @see WsDirectory::contactFoundEvent
	 */
	Event< void(WsDirectory & sender, ContactProfile * profile, bool online) > contactFoundEvent;

	void searchEntry(const std::string & query, WsDirectory::Criteria criteria = WsDirectory::None);

	CWengoPhone & getCWengoPhone() const {
		return _cWengoPhone;
	}

private:

	WsDirectory & _wsDirectory;

	CWengoPhone & _cWengoPhone;

	PWsDirectory * _pWsDirectory;
};

#endif	//OWCWSDIRECTORY_H
