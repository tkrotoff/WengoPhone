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

#ifndef OWPHAPICALLBACKS_H
#define OWPHAPICALLBACKS_H

#include <phapiwrapperdll.h>

#include <phapi.h>
#include <phevents.h>

#include <util/Singleton.h>

#include <string>
#include <set>

/**
 * PhApi callbacks.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class PhApiCallbacks : public Singleton<PhApiCallbacks> {
	friend class Singleton<PhApiCallbacks>;
public:

	PHAPIWRAPPER_API void startListeningPhApiEvents();

	PHAPIWRAPPER_API void registerProgress(OWPL_LINESTATE_INFO * info);

	PHAPIWRAPPER_API void callProgress(OWPL_CALLSTATE_INFO * info);

	PHAPIWRAPPER_API void subscriptionProgress(OWPL_SUBSTATUS_INFO * info);

	PHAPIWRAPPER_API void onNotify(OWPL_NOTIFICATION_INFO * info);

	PHAPIWRAPPER_API void messageProgress(OWPL_MESSAGE_INFO * info);

	PHAPIWRAPPER_API void errorNotify(OWPL_ERROR_INFO * info);

private:

	PhApiCallbacks();

	~PhApiCallbacks();

	/**
	 * Gets the sip contact from a sip "full contact".
	 * e.g: <sip:joe@voip.wengo.fr>;tag=dkflskdlfmksdlmf => sip:joe@voip.wengo.fr
	 */
	static std::string computeContactId(const std::string & contactFromPhApi);

	std::set<std::string> _subscribedContacts;
};

#endif	//OWPHAPICALLBACKS_H
