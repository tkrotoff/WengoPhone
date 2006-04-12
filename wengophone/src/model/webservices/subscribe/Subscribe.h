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

#ifndef OW_SUBSCRIBEWEBSERVICE_H
#define OW_SUBSCRIBEWEBSERVICE_H

#include <model/webservices/WengoWebService.h>

/**
 * @class WsWengoSubscribe Wengo subscribe web service
 *
 * @author Mathieu Stute
 */
class WsWengoSubscribe : public WengoWebService {

public:
	
	enum SubscriptionStatus {
		/** Subscription ok */
		SubscriptioOk,
		
		/** Subscription bad query */
		SubscriptionBadQuery,
		
		/** Subscription bad version */
		SubscriptionBadVersion,
		
		/** Subscription unknow error */
		SubscriptionUnknownError,
		
		/** Subscription mail error */
		SubscriptionMailError,
		
		/** Subscription nickname error */
		SubscriptionNicknameError,
		
		/** Subscription failed */
		SubscriptionFailed,
	};
	
	/**
	 * default constructor
	 *
	 * @param wengoAccount the WengoAccount used for web services
	 */
	WsWengoSubscribe();

	virtual ~WsWengoSubscribe() {}

	/**
	 * Callback to check if the SMS was received or not.
	 *
	 * @param sender this class
	 * @param id request unique id
	 * @param status subscription status (ok or error)
	 * @param error error message
	 */
	Event<void (WsWengoSubscribe & sender, int id, SubscriptionStatus status, 
				const std::string & errorMessage, const std::string & password)> wengoSubscriptionEvent;

	/**
	 * Sends a subscription request.
	 *
	 * @param email email
	 * @param nickname Wengo nickname
	 * @param lang language for the subscription process
	 * @param password Wengo password
	 * @return a unique request id
	 */
	 int subscribe(const std::string & email, const std::string & nickname,
		const std::string & lang, const std::string & password = "");

private:

	/**
	 * @see WengoWebService
	 */
	void answerReceived(const std::string & answer, int id);
};

#endif //OW_SUBSCRIBEWEBSERVICE_H
