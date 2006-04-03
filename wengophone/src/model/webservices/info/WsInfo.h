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

#ifndef OW_WENGOSWEBSERVICE_H
#define OW_WENGOSWEBSERVICE_H

#include <model/webservices/WengoWebService.h>

/**
 * @class WsWengoInfo wengo information web service
 *
 * @author Mathieu Stute
 */
class WsWengoInfo : public WengoWebService {

public:
	
	enum WsWengoInfoStatus {
		/** The information could not be retrieved. */
		WsWengoInfoStatusError,

		/** Retrive information successfully */
		WsWengoInfoStatusOk
	};
	
	/**
	 * default constructor
	 *
	 * @param wengoAccount the WengoAccount used for web services
	 */
	WsWengoInfo(WengoAccount & wengoAccount);

	/**
	 * A anwser about wengo's has been received.
	 *
	 * @param sender this class
	 * @param id unique identifier of the request
	 * @param status the request status (ok or error)
	 * @param wengos wengo's count
	 */
	Event<void (WsWengoInfo & sender, int id, WsWengoInfoStatus status, float wengos)> wsInfoWengosEvent;

	/**
	 * A anwser about sms count has been received.
	 *
	 * @param sender this class
	 * @param id unique identifier of the request
	 * @param status the request status (ok or error)
	 * @param sms sms count
	 */
	Event<void (WsWengoInfo & sender, int id, WsWengoInfoStatus status, int sms)> wsInfoSmsCountEvent;

	/**
	 * A anwser about active mail has been received.
	 *
	 * @param sender this class
	 * @param id unique identifier of the request
	 * @param status the request status (ok or error)
	 * @param activeMail 1 if active, 0 if not
	 */
	Event<void (WsWengoInfo & sender, int id, WsWengoInfoStatus status, int activeMail)> wsInfoActiveMailEvent;

	/**
	 * A anwser about unread voice mail has been received.
	 *
	 * @param sender this class
	 * @param id unique identifier of the request
	 * @param status the request status (ok or error)
	 * @param voicemail unread voice mail count
	 */
	Event<void (WsWengoInfo & sender, int id, WsWengoInfoStatus status, int voicemail)> wsInfoVoiceMailEvent;

	/**
	 * A anwser about unread voice mail has been received.
	 *
	 * @param sender this class
	 * @param id unique identifier of the request
	 * @param status the request status (ok or error)
	 * @param number unread voice mail count
	 */
	Event<void (WsWengoInfo & sender, int id, WsWengoInfoStatus status, std::string number)> wsInfoPtsnNumberEvent;

	/**
	 * A anwser about unread voice mail has been received.
	 *
	 * @param sender this class
	 * @param id unique identifier of the request
	 * @param status the request status (ok or error)
	 * @param voicemail if true forward to voice mail else to the given number
	 * @param dest1	first forward number
	 * @param dest2	second forward number (meaningfull only if voicemail is true)
	 * @param dest3	third forward number (meaningfull only if voicemail is true)
	 */
	Event<void (WsWengoInfo & sender, int id, WsWengoInfoStatus status, 
		bool voicemail, std::string dest1, std::string dest2, std::string dest3)> wsCallForwardInfoEvent;

	/**
	 * Set/unset wengo's request
	 *
	 * @param wengos if true enable wengo's request
	 */
	void getWengosCount(bool wengos);

	/**
	 * Set/unset sms count request
	 *
	 * @param sms if true enable sms count request
	 */
	void getSmsCount(bool sms);

	/**
	 * Set/unset active mail request
	 *
	 * @param mail if true enable active mail request
	 */
	void getActiveMail(bool mail);

	/**
	 * Set/unset voice mail count request
	 *
	 * @param voicemail if true enable voice mail count request
	 */
	void getUnreadVoiceMail(bool voicemail);

	/**
	 * Set/unset call forward request
	 *
	 * @param callForward if true enable call forward request
	 */
	void getCallForwardInfo(bool callForward);

	/**
	 * Set/unset pstn number request
	 *
	 * @param wengos if true enable pstn number request
	 */
	void getPstnNumber(bool pstnNumber);

	/**
	 * Sends the request.
	 *
	 * @return unique request ID
	 */
	int execute();

private:

	/**
	 * @see WengoWebService
	 */
	void answerReceived(std::string answer, int id);

	/** activate wengos count */
	bool _wengosCount;

	/** activate sms count */
	bool _smsCount;

	/** activate active mail request */
	bool _activeMail;

	/** activate unread voice mail count */
	bool _unreadVoiceMail;

	/** activate unread voice mail count */
	bool _callForward;
	
	/** activate pstn number request*/
	bool _pstnNumber;

	static const std::string WENGOSCOUNT_TAG;

	static const std::string SMSCOUNT_TAG;

	static const std::string ACTIVEMAIL_TAG;

	static const std::string UNREADVOICEMAILCOUNT_TAG;

	static const std::string PSTNNUMBER_TAG;

	static const std::string CALLFORWARD_TAG;

	static const std::string CALLFORWARD_TOVOICEMAIL_ENABLE_TAG;

	static const std::string CALLFORWARD_TOVOICEMAIL_DEST_TAG;
	
	static const std::string CALLFORWARD_TOPSTN_ENABLE_TAG;
	
	static const std::string CALLFORWARD_TOPSTN_DEST1_TAG;

	static const std::string CALLFORWARD_TOPSTN_DEST2_TAG;

	static const std::string CALLFORWARD_TOPSTN_DEST3_TAG;
};

#endif //OW_SMSWEBSERVICE_H
