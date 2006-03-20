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

#ifndef IPHONELINE_H
#define IPHONELINE_H

#include <Event.h>
#include <Interface.h>
#include <List.h>

#include <sipwrapper/EnumTone.h>
#include <sipwrapper/EnumPhoneCallState.h>
#include <sipwrapper/EnumPhoneLineState.h>

#include <string>

class SipAccount;
class SipWrapper;
class SipAddress;
class SipPresenceState;
class PhoneLineState;
class WengoPhone;
class PhoneCall;

/**
 * Interface representing a phone line.
 *
 * A phone line creates/deletes/contains a list of PhoneCall.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class IPhoneLine : Interface {
public:

	/**
	 * @see SipWrapper::phoneLineStateChangedEvent
	 */
	Event<void (IPhoneLine & sender)> stateChangedEvent;

	/**
	 * A PhoneCall has been created.
	 *
	 * @param sender this class
	 * @param phoneCall phoneCall created
	 */
	Event<void (IPhoneLine & sender, PhoneCall & phoneCall)> phoneCallCreatedEvent;

	virtual ~IPhoneLine() {
	}

	virtual std::string getMySipAddress() const = 0;

	/**
	 * @see SipWrapper::makeCall()
	 */
	virtual int makeCall(const std::string & phoneNumber) = 0;

	/**
	 * Gets the SIP account associated with this PhoneLine.
	 *
	 * @return SIP account associated with this PhoneLine or NULL
	 */
	virtual const SipAccount & getSipAccount() const = 0;

	/**
	 * Establishes a connection with the SIP server.
	 */
	virtual void connect() = 0;

	/**
	 * Finishes the connection with the SIP server.
	 */
	virtual void disconnect() = 0;

	/**
	 * Sets the state of a PhoneCall.
	 *
	 * @param callId PhoneCall id
	 * @param state the state (e.g event)
	 * @param sipAddress from who we get a phone call
	 */
	virtual void setPhoneCallState(int callId, EnumPhoneCallState::PhoneCallState state, const SipAddress & sipAddress) = 0;

	/**
	 * Gets the SIP implementation wrapper.
	 *
	 * @return the SIP implementation wrapper
	 */
	virtual SipWrapper & getSipWrapper() const = 0;

	/**
	 * Gets the PhoneLine id.
	 *
	 * @return the id associated with this PhoneLine
	 */
	virtual int getLineId() const = 0;

	/**
	 * Changes the state of this PhoneLine.
	 *
	 * @param state state code corresponding to the new PhoneLine state
	 */
	virtual void setState(EnumPhoneLineState::PhoneLineState state) = 0;

	/**
	 * Gets the current state of this PhoneLine.
	 *
	 * @return state of this PhoneLine
	 */
	virtual const PhoneLineState & getState() const = 0;

	/**
	 * Gets the WengoPhone instance.
	 *
	 * @return WengoPhone instance
	 */
	virtual WengoPhone & getWengoPhone() const = 0;

	/**
	 * Gets the PhoneCall associated with a given callId.
	 *
	 * @return PhoneCall given its callId or NULL if no PhoneCall corresponds to the callId
	 */
	virtual PhoneCall * getPhoneCall(int callId) /*const*/ = 0;

	/**
	 * Gets the active PhoneCall associated with this PhoneLine.
	 *
	 * @return active PhoneCall or NULL
	 */
	virtual PhoneCall * getActivePhoneCall() const = 0;

	/**
	 * Gets the list of PhoneCall associated with this PhoneLine.
	 *
	 * @return the list of PhoneCall
	 */
	virtual List<PhoneCall *> getPhoneCallList() const = 0;
};

#endif	//IPHONELINE_H
