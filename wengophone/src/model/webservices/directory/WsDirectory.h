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

#ifndef WSDIRECTORY_H
#define WSDIRECTORY_H

#include <model/webservices/WengoWebService.h>

#include <util/Event.h>

class ContactProfile;

/**
 * Search for Wengo users web service
 *
 * @ingroup model
 * @author Mathieu Stute
 */
class WsDirectory : public WengoWebService {
public:

	enum Criteria {
		lname,
		fname,
		city,
		country,
		alias,
		wengoid,
		none,
	};

	/**
	 * Emitted when a contact has been found.
	 *
	 * @param sender WsDirectory
	 * @param contact contact found
	 */
	Event< void(WsDirectory & sender, ContactProfile * contact, bool online) > contactFoundEvent;

	/**
	 * Default constructor.
	 *
	 * @param wengoAccount the WengoAccount used for web services
	 */
	 WsDirectory(WengoAccount * wengoAccount);

	virtual ~WsDirectory() {}

	void searchEntry(const std::string & query, Criteria criteria = none);

private:

	void answerReceived(const std::string & answer, int requestId);
};

#endif	//WSDIRECTORY_H
