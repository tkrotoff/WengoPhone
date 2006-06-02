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

#ifndef OWWSINFO_H
#define OWWSINFO_H

#include <model/webservices/WengoWebService.h>

/**
 * @class WsWengoInfo wengo information web service
 *
 * @author Mathieu Stute
 */
class WsInfo : public WengoWebService {
public:

	enum WsInfoStatus {
		/** The information could not be retrieved. */
		WsInfoStatusError,

		/** Retrive information successfully */
		WsInfoStatusOk
	};

	enum WsInfoCallForwardMode {
		/** callforward is enabled & forward to voicemail. */
		WsInfoCallForwardModeVoicemail,

		/** callforward is enabled & forward to given PSTN numbers. */
		WsInfoCallForwardModeNumber,

		/** callforward is disabled. */
		WsInfoCallForwardMode_Disabled,

		/** callforward is unauthorized. */
		WsInfoCallForwardMode_Unauthorized,
	};

	/**
	 * default constructor
	 *
	 * @param wengoAccount the WengoAccount used for web services
	 */
	WsInfo(WengoAccount * wengoAccount);

	/**
	 * An anwser about wengo's has been received.
	 *
	 * @param sender this class
	 * @param id unique identifier of the request
	 * @param status the request status (ok or error)
	 * @param wengos wengo's count
	 */
	Event<void (WsInfo & sender, int id, WsInfoStatus status, float wengos)> wsInfoWengosEvent;

	/**
	 * An anwser about sms count has been received.
	 *
	 * @param sender this class
	 * @param id unique identifier of the request
	 * @param status the request status (ok or error)
	 * @param sms sms count
	 */
	Event<void (WsInfo & sender, int id, WsInfoStatus status, int sms)> wsInfoSmsCountEvent;

	/**
	 * An anwser about active mail has been received.
	 *
	 * @param sender this class
	 * @param id unique identifier of the request
	 * @param status the request status (ok or error)
	 * @param activeMail 1 if active, 0 if not
	 */
	Event<void (WsInfo & sender, int id, WsInfoStatus status, int activeMail)> wsInfoActiveMailEvent;

	/**
	 * An anwser about unread voice mail has been received.
	 *
	 * @param sender this class
	 * @param id unique identifier of the request
	 * @param status the request status (ok or error)
	 * @param voicemail unread voice mail count
	 */
	Event<void (WsInfo & sender, int id, WsInfoStatus status, int voicemail)> wsInfoVoiceMailEvent;

	/**
	 * An anwser about unread voice mail has been received.
	 *
	 * @param sender this class
	 * @param id unique identifier of the request
	 * @param status the request status (ok or error)
	 * @param number unread voice mail count
	 */
	Event<void (WsInfo & sender, int id, WsInfoStatus status, std::string number)> wsInfoPtsnNumberEvent;

	/**
	 * An anwser about unread voice mail has been received.
	 *
	 * @param sender this class
	 * @param id unique identifier of the request
	 * @param status the request status (ok or error)
	 * @param voicemail if true forward to voice mail else to the given number
	 * @param dest1	first forward number
	 * @param dest2	second forward number (meaningfull only if voicemail is true)
	 * @param dest3	third forward number (meaningfull only if voicemail is true)
	 */
	Event<void (WsInfo & sender, int id, WsInfoStatus status,
		WsInfoCallForwardMode mode, bool voicemail, std::string dest1, std::string dest2, std::string dest3)> wsCallForwardInfoEvent;

	/**
	 * An anwser active voice mail has been received.
	 *
	 * @param sender this class
	 * @param status the request status (ok or error)
	 * @param voicemail true if a voice mail is active
	 */
	Event<void (WsInfo & sender, int id, WsInfoStatus status, bool voicemail)> wsInfoActiveVoiceMailEvent;

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
	void answerReceived(const std::string & answer, int id);

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

	static const std::string ACTIVEVOICEMAIL_TAG;

	static const std::string PSTNNUMBER_TAG;

	static const std::string CALLFORWARD_TAG;

	static const std::string CALLFORWARD_MODE_TAG;

	static const std::string CALLFORWARD_TOPSTN_DEST1_TAG;

	static const std::string CALLFORWARD_TOPSTN_DEST2_TAG;

	static const std::string CALLFORWARD_TOPSTN_DEST3_TAG;
};

#endif	//OWWSINFO_H
