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

#include "History.h"

#include <model/profile/UserProfile.h>
#include <model/phoneline/IPhoneLine.h>

#include <util/Logger.h>
#include <util/File.h>

#include <sstream>
#include <exception>

History::History(UserProfile & userProfile)
	: _userProfile(userProfile) {

	_collection = new HistoryMementoCollection();
	_missedCallCount = 0;
}

History::~History() {
}

HistoryMementoCollection * History::getHistoryMementoCollection() {
	return _collection;
}

unsigned int History::addMemento(HistoryMemento * memento) {
	unsigned id = _collection->addMemento(memento);
	mementoAddedEvent(*this, id);
	return id;
}

void History::updateCallDuration(int callId, int duration) {
	HistoryMemento * memento = _collection->getMementoByCallId(callId);
	if (memento) {
		memento->updateDuration(duration);
		unsigned id = _collection->getMementoId(memento);
		mementoUpdatedEvent(*this, id);
	}
}

void History::updateSMSState(int callId, HistoryMemento::State state) {
	HistoryMemento * memento = _collection->getMementoBySMSId(callId);
	if (memento) {
		memento->updateState(state);
		unsigned id = _collection->getMementoId(memento);
		mementoUpdatedEvent(*this, id);
	}
}

void History::updateCallState(int callId, HistoryMemento::State state) {
	HistoryMemento * memento = _collection->getMementoByCallId(callId);
	if (memento) {
		memento->updateState(state);
		unsigned id = _collection->getMementoId(memento);
		mementoUpdatedEvent(*this, id);

		if (state == HistoryMemento::MissedCall) {
			_missedCallCount++;
			unseenMissedCallsChangedEvent(*this, _missedCallCount);
		}
	}
}

unsigned int History::size() const {
	return _collection->size();
}

std::string History::serialize() {
	std::stringstream ss;
	try {
		boost::archive::xml_oarchive oa(ss);
		oa << BOOST_SERIALIZATION_NVP(_collection);
		oa << BOOST_SERIALIZATION_NVP(_missedCallCount);
		return ss.str();
	} catch (boost::archive::archive_exception & e) {
		LOG_DEBUG(e.what());
		return String::null;
	}
}

bool History::unserialize(const std::string & data) {
	std::stringstream ss(data);
	try {
		boost::archive::xml_iarchive ia(ss);
		ia >> BOOST_SERIALIZATION_NVP(_collection);
		ia >> BOOST_SERIALIZATION_NVP(_missedCallCount);
	} catch (boost::archive::archive_exception & e) {
		LOG_DEBUG(e.what());
		return false;
	}
	return true;
}

void History::clear(HistoryMemento::State state) {
	_collection->clear(state);
	mementoRemovedEvent(*this, 0);
}

std::string History::toString() const {
	return _collection->toString();
}

void History::removeMemento(unsigned id) {
	_collection->removeMemento(id);
	mementoRemovedEvent(*this, id);
}

HistoryMemento * History::getMemento(unsigned id) {
	Mutex::ScopedLock scopedLock(_mutex);

	return _collection->getMemento(id);
}

HistoryMementoCollection * History::getMementos(HistoryMemento::State state, int count) {
	Mutex::ScopedLock scopedLock(_mutex);

	return _collection->getMementos(state, count);
}

bool History::load(const std::string & url) {
	FileReader file(url);

	if (file.open()) {
		std::string data = file.read();
		file.close();

		//clear & unserialize the history
		clear();
		unserialize(data);
		historyLoadedEvent(*this);

		return true;
	}

	return false;
}

bool History::save(const std::string & url) {
	FileWriter file(url);

	file.write(serialize());
	file.close();

	historySavedEvent(*this);

	return true;
}

void History::replay(unsigned id) {

	//replay only outgoing call
	if (getMemento(id)->getState() == HistoryMemento::OutgoingCall) {
		std::string phoneNumber = getMemento(id)->getPeer();
		_userProfile.getActivePhoneLine()->makeCall(phoneNumber);
	}
}

void History::resetUnseenMissedCalls() {
	_missedCallCount = 0;
	unseenMissedCallsChangedEvent(*this, _missedCallCount);
}

int History::getUnseenMissedCalls() {
	Mutex::ScopedLock scopedLock(_mutex);

	return _missedCallCount;
}

bool History::addChatMementoSession(int chatSessionID) {
	if(_chatSessionsMementos[chatSessionID] == NULL){
		_chatSessionsMementos[chatSessionID] = new HistoryMementoCollection();
		return true;
	}
	return false;
}

void History::removeChatMementoSession(int chatSessionID){
	HistoryMementoCollection * collection = NULL;
	if((collection = _chatSessionsMementos[chatSessionID]) != NULL){
		_chatSessionsMementos.erase(chatSessionID);
		delete collection;
	}
}

unsigned History::addChatMemento(HistoryMemento * memento, int chatSessionID) {
	unsigned id = _chatSessionsMementos[chatSessionID]->addMemento(memento);
	chatMementoAddedEvent(*this, id);
	return id;
}