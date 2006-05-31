/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include "CHistory.h"

#include <control/CWengoPhone.h>
#include <presentation/PFactory.h>
#include <presentation/PHistory.h>

#include <util/Logger.h>
#include <thread/Thread.h>

CHistory::CHistory(History & history, CWengoPhone & cWengoPhone, Thread & modelThread)
	: _history(history), _cWengoPhone(cWengoPhone), _modelThread(modelThread) {

	_pHistory = PFactory::getFactory().createPresentationHistory(*this);

	_history.historyLoadedEvent += boost::bind(&CHistory::historyLoadedEventHandler, this, _1);
	_history.mementoAddedEvent += boost::bind(&CHistory::historyMementoAddedEventHandler, this, _1, _2);
	_history.mementoUpdatedEvent += boost::bind(&CHistory::historyMementoUpdatedEventHandler, this, _1, _2);
	_history.mementoRemovedEvent += boost::bind(&CHistory::historyMementoRemovedEventHandler, this, _1, _2);
	_history.unseenMissedCallsChangedEvent += boost::bind(&CHistory::unseenMissedCallsChangedEventhandler, this, _1, _2);
}

CWengoPhone & CHistory::getCWengoPhone() const {
	return _cWengoPhone;
}

History & CHistory::getHistory() {
	return _history;
}

void CHistory::historyMementoAddedEventHandler(History & history, unsigned int id) {
	mementoAddedEvent(*this, id);
}

void CHistory::historyMementoUpdatedEventHandler(History & history, unsigned int id) {
	mementoUpdatedEvent(*this, id);
}

void CHistory::historyMementoRemovedEventHandler(History & history, unsigned int id) {
	mementoRemovedEvent(*this, id);
}

void CHistory::historyLoadedEventHandler(History & history) {
	historyLoadedEvent(*this);
}

void CHistory::unseenMissedCallsChangedEventhandler(History &, int count) {
	unseenMissedCallsChangedEvent(*this, count);
}

void CHistory::removeHistoryMemento(unsigned int id) {
	typedef ThreadEvent1<void (unsigned int id), unsigned int> MyThreadEvent;
	MyThreadEvent * event =
			new MyThreadEvent(boost::bind(&CHistory::removeHistoryMementoThreadSafe, this, _1), id);

	_modelThread.postEvent(event);
}

void CHistory::removeHistoryMementoThreadSafe(unsigned int id) {
	_history.removeMemento(id);
}

void CHistory::clear(HistoryMemento::State state) {
	typedef ThreadEvent1<void (HistoryMemento::State state), HistoryMemento::State> MyThreadEvent;
	MyThreadEvent * event =
			new MyThreadEvent(boost::bind(&CHistory::clearThreadSafe, this, _1), state);

	_modelThread.postEvent(event);
}

void CHistory::clearThreadSafe(HistoryMemento::State state) {
	_history.clear(state);
}

std::string CHistory::getMementoData(unsigned int id) {
	return _history.getMemento(id)->getData();
}

std::string CHistory::getMementoPeer(unsigned int id) {
	return _history.getMemento(id)->getPeer();
}

void CHistory::replay(unsigned int id) {
	typedef ThreadEvent1<void (unsigned int id), unsigned int> MyThreadEvent;
	MyThreadEvent * event =
			new MyThreadEvent(boost::bind(&CHistory::replayThreadSafe, this, _1), id);

	_modelThread.postEvent(event);
}

void CHistory::replayThreadSafe(unsigned int id) {
	_history.replay(id);
}

HistoryMementoCollection * CHistory::getMementos(HistoryMemento::State state, int count) {
	return _history.getMementos(state, count);
}

void CHistory::resetUnseenMissedCalls() {
	typedef ThreadEvent0<void ()> MyThreadEvent;
	MyThreadEvent * event =
			new MyThreadEvent(boost::bind(&CHistory::resetUnseenMissedCallsThreadSafe, this));

	_modelThread.postEvent(event);
}

void CHistory::resetUnseenMissedCallsThreadSafe() {
	_history.resetUnseenMissedCalls();
}

int CHistory::getUnseenMissedCalls() {
	return _history.getUnseenMissedCalls();
}
