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

#ifndef OW_CWENGOSUBSCRIBE_H
#define OW_CWENGOSUBSCRIBE_H

#include <model/webservices/subscribe/Subscribe.h>

#include <string>

class CWengoPhone;
class PSubscribe;

/**
 *
 * @ingroup control
 * @author Mathieu Stute
 */
class CSubscribe {
public:

	/**
	 * @see WsWengoSubscribe::wengoSubscriptionEvent
	 */
	Event<void (WsWengoSubscribe & sender, int id, WsWengoSubscribe::SubscriptionStatus status,
				const std::string & errorMessage, const std::string & password)> wengoSubscriptionEvent;

	CSubscribe(WsWengoSubscribe & wsWengoSubscribe, CWengoPhone & cWengoPhone);

	~CSubscribe();

	CWengoPhone & getCWengoPhone() const {
		return _cWengoPhone;
	}

	/**
	 * @see WsWengoSubscribe::subscribe()
	 */
	int subscribe(const std::string & email, const std::string & nickname,
		const std::string & lang, const std::string & password = "");

private:

	WsWengoSubscribe & _wsWengoSubscribe;

	CWengoPhone & _cWengoPhone;

	PSubscribe * _pSubscribe;
};

#endif	//OW_CWENGOSUBSCRIBE_H
