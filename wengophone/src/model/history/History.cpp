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
#include <util/String.h>

#include <fstream>
#include <sstream>
#include <exception>

History::History(UserProfile & userProfile) : _userProfile(userProfile) {
	_collection = new HistoryMementoCollection();
}

History::~History() {
}

HistoryMementoCollection * History::getHistoryMementoCollection() {
	return _collection;
}

int History::addMemento(HistoryMemento * memento) {
	int id = _collection->addMemento(memento);
	mementoAddedEvent(*this, id);
	return id;
}

void History::updateCallDuration(int callId, int duration) {
	int id = _collection->getMementoByCallId(callId);
	HistoryMemento * memento = getMemento(id);
	if( memento ) {
		memento->updateDuration(duration);
		mementoUpdatedEvent(*this, id);
	}
}

void History::updateSMSState(int callId, HistoryMemento::State state) {
	int id = _collection->getMementoBySMSId(callId);
	HistoryMemento * memento = getMemento(id);
	if( memento ) {
		memento->updateState(state);
		mementoUpdatedEvent(*this, id);
	}
}

void History::updateCallState(int callId, HistoryMemento::State state) {
	int id = _collection->getMementoByCallId(callId);
	HistoryMemento * memento = getMemento(id);
	if( memento ) {
		memento->updateState(state);
		mementoUpdatedEvent(*this, id);
	}
}

int History::size() {
	return _collection->size();
}

std::string History::serialize() {
	std::stringstream ss;
	try {
		boost::archive::xml_oarchive oa(ss);
		oa << BOOST_SERIALIZATION_NVP(_collection);
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
	} catch (boost::archive::archive_exception & e) {
		LOG_DEBUG(e.what());
		return false;
	}
	return true;
}

void History::clear(HistoryMemento::State state) {
	_collection->clear(state);
}

std::string History::toString() {
	return _collection->toString();
}

void History::removeMemento(int id) {
	//TODO: verify that the memento is really removed
	_collection->removeMemento(id);
	mementoRemovedEvent(*this, id);
}

HistoryMemento * History::getMemento(int id) {
	return _collection->getMemento(id);
}

HistoryMementoCollection * History::getMementos(HistoryMemento::State state, int count) {
	return _collection->getMementos(state, count);
}

void History::load(std::string filename) {
	std::string line = "";
	std::string lines = "";
	
	if( filename == "" ) {
		return;
	}
	
	//open the file & read all its content
	std::ifstream myfile (filename.c_str());
	if (myfile.is_open()) {
		while (! myfile.eof() ) {
			getline (myfile,line);
			lines += line;
		}
		myfile.close();
	}
	
	//clear & unserialize the history
	clear();
	unserialize(lines);
	historyLoadedEvent(*this);
	//LOG_DEBUG(toString());
}

void History::save(std::string filename) {
	std::ofstream ofs(filename.c_str());
	ofs << serialize();
	ofs.close();
	historySavedEvent(*this);
	//LOG_DEBUG(toString());
}

void History::replay(int id) {
	
	//replay only outgoing call
	if( getMemento(id)->getState() == HistoryMemento::OutgoingCall ) {
		std::string phoneNumber = getMemento(id)->getPeer();
		_userProfile.getActivePhoneLine()->makeCall(phoneNumber);
	}
}
