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

#include "HistoryMemento.h"

#include <util/Logger.h>

#include <fstream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

const std::string HistoryMemento::StateIncomingCall = "StateIncomingCall";
const std::string HistoryMemento::StateOutgoingCall = "StateOutgoingCall";
const std::string HistoryMemento::StateMissedCall = "StateMissedCall";
const std::string HistoryMemento::StateRejectedCall = "StateRejectedCall";
const std::string HistoryMemento::StateOutgoingSMSOK = "StateOutgoingSMSOK";
const std::string HistoryMemento::StateOutgoingSMSNOK = "StateOutgoingSMSNOK";
const std::string HistoryMemento::StateNone = "StateNone";
const std::string HistoryMemento::StateAny = "StateAny";

HistoryMemento::HistoryMemento() {
	//set internal data
	_state = None;
	_peer = "";
	_callId = -1;
	_duration = -1;
	_date = Date();
	_time = Time();
	_data = "";
}

HistoryMemento::HistoryMemento(State state, std::string peer, int callId, std::string data) {
	//set internal data
	_state = state;
	_peer = peer;
	_callId = callId;
	_duration = -1;
	_date = Date();
	_time = Time();
	_data = data;
}

HistoryMemento::~HistoryMemento() {
}

HistoryMemento::State HistoryMemento::getState() {
	return _state;
}

std::string HistoryMemento::getPeer() {
	return _peer;
}

int HistoryMemento::getDuration() {
	return _duration;
}

Date HistoryMemento::getDate() {
	return _date;
}

Time HistoryMemento::getTime() {
	return _time;
}

int HistoryMemento::getCallId() {
	return _callId;
}

void HistoryMemento::updateDuration(int duration) {
	_duration = duration;
}

void HistoryMemento::updateState(State state) {
	_state = state;
}

bool HistoryMemento::canReplay() {
	if( ( _state == OutgoingCall ) || ( _state == OutgoingSmsOk ) ) {
		return true;
	} else {
		return false;
	}
}

bool HistoryMemento::isCallMemento() {
	return ((_state == OutgoingCall) || (_state == IncomingCall) || 
			(_state == MissedCall) || (_state == MissedCall) || (_state == RejectedCall));
}

bool HistoryMemento::isSMSMemento() {
	return ( (_state == OutgoingSmsOk) || (_state == OutgoingSmsNok) );
}

bool HistoryMemento::isChatSessionMemento() {
	return (_state == ChatSession);
}

void HistoryMemento::replay() {
	//TODO: replay outgoing call & outgoing sms
	switch(_state) {
	case OutgoingCall:
		break;
	case OutgoingSmsOk:
		break;
	case OutgoingSmsNok:
		//can't replay
		break;
	case IncomingCall:
		//can't replay
		break;
	case RejectedCall:
		//can't replay
		break;
	case MissedCall:
		//can't replay
		break;
	case ChatSession:
		break;
	case None:
		//can't replay
		break;
	case Any:
		//can't replay
		break;
	default:
		LOG_FATAL("Unknown state");
	}
}

std::string HistoryMemento::toString() {
	std::string toReturn = "";
	static std::string separator = "\n\t- ";
	
	toReturn += "Peer: " + _peer + separator;
	toReturn += "date: " + _date.toString() + separator;
	toReturn += "time: " + _time.toString() + separator;
	toReturn += "duration: " + String::fromNumber(_duration) + separator;
	toReturn += "state: " + stateToString(_state) + separator;
	toReturn += "callid: " + String::fromNumber(_callId) + separator;
	toReturn += "data: " + _data;
	
	return toReturn;
}

std::string HistoryMemento::stateToString(State state) {
	switch(state) {
	case OutgoingCall:
		return StateOutgoingCall;
		break;
	case IncomingCall:
		return StateIncomingCall;
		break;
	case OutgoingSmsOk:
		return StateOutgoingSMSOK;
		break;
	case OutgoingSmsNok:
		return StateOutgoingSMSNOK;
		break;
	case RejectedCall:
		return StateRejectedCall;
		break;
	case MissedCall:
		return StateMissedCall;
		break;
	case None:
		return StateNone;
		break;
	case Any:
		return StateAny;
		break;
	default:
		LOG_FATAL("Unknown state");
	}
}
