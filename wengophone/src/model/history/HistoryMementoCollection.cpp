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

#include "HistoryMementoCollection.h"

#include <util/String.h>
#include <util/Logger.h>

unsigned int HistoryMementoCollection::_historyId = 0;

HistoryMementoCollection::HistoryMementoCollection() {
}

HistoryMementoCollection::~HistoryMementoCollection() {
}

int HistoryMementoCollection::addMemento(HistoryMemento * memento) {
	_privateCollection[_historyId] = memento;
	_historyId++;
	return _historyId - 1;
}

int HistoryMementoCollection::getMementoByCallId(int callId) {
	if( callId != -1 ) {
		HistoryMap::iterator it;
		for(it = _privateCollection.begin(); it != _privateCollection.end(); it++) {
			if( ((*it).second->isCallMemento()) &&
				((*it).second->getCallId() == callId) ) {
				return (*it).first;
				}
		}
	}
	return -1;
}

int HistoryMementoCollection::getMementoBySMSId(int callId) {
		if( callId != -1 ) {
		HistoryMap::iterator it;
		for(it = _privateCollection.begin(); it != _privateCollection.end(); it++) {
			if( ((*it).second->isSMSMemento()) &&
				((*it).second->getCallId() == callId)) {
				return (*it).first;
			}
		}
	}
	return -1;
}

int HistoryMementoCollection::size() {
	return _privateCollection.size();
}

HistoryMemento * HistoryMementoCollection::getMemento(int id) {
	HistoryMemento * memento = (*(_privateCollection.find(id))).second;
	return memento;
}

void HistoryMementoCollection::removeMemento(int id) {
	_privateCollection.erase(_privateCollection.find(id));
}

void HistoryMementoCollection::clear(HistoryMemento::State state) {
	
	HistoryMap::iterator it;
	for(it = _privateCollection.begin(); it != _privateCollection.end(); it++) {
		
		if( ((*it).second->getState() == state) || (state == HistoryMemento::Any)) {
			_privateCollection.erase(it);
		}
	}
}

void HistoryMementoCollection::setMaxEntries(int max) {
	LOG_WARN("Not implemented yet");
}

void HistoryMementoCollection::privateAdd(int id, HistoryMemento * memento) {
	_privateCollection[id] = memento;
}

HistoryMementoCollection * HistoryMementoCollection::getMementos(HistoryMemento::State state, int count) {
	int c = 0;
	HistoryMementoCollection * toReturn = new HistoryMementoCollection();
	
	HistoryMap::iterator it;
	for(it = _privateCollection.begin(); it != _privateCollection.end(); it++) {
		
		if( (state == HistoryMemento::Any) || ((*it).second->getState() == state) ) {
			toReturn->privateAdd((*it).first, (*it).second);
			c++;
		}
		
		if( c > count ) {
			break;
		}
	}
	return toReturn;
}

HistoryMap::iterator HistoryMementoCollection::begin() {
	return _privateCollection.begin();
}
	
HistoryMap::iterator HistoryMementoCollection::end() {
	return _privateCollection.end();
}

std::string HistoryMementoCollection::toString() {
	std::string toReturn = "_historyId: " + String::fromNumber(_historyId) + "\n";
	
	HistoryMap::iterator it;
	for(it = _privateCollection.begin(); it != _privateCollection.end(); it++) {
		toReturn += (*it).second->toString() + "\n";
	}
	return toReturn;
}
