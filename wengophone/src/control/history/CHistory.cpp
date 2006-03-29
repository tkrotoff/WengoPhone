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

#include <model/history/History.h>
#include <control/CWengoPhone.h>
#include <presentation/PFactory.h>
#include <presentation/PHistory.h>

#include <util/Logger.h>

CHistory::CHistory(History & history, CWengoPhone & cWengoPhone)
	: _history(history), _cWengoPhone(cWengoPhone) {

	_pHistory = PFactory::getFactory().createPresentationHistory(*this);

	History::getInstance().historyLoadedEvent += boost::bind(&CHistory::historyLoadedEventHandler, this, _1);
	History::getInstance().mementoAddedEvent += boost::bind(&CHistory::historyMementoAddedEventHandler, this, _1, _2);
	History::getInstance().mementoUpdatedEvent += boost::bind(&CHistory::historyMementoUpdatedEventHandler, this, _1, _2);
	History::getInstance().mementoRemovedEvent += boost::bind(&CHistory::historyMementoRemovedEventHandler, this, _1, _2);
}

CWengoPhone & CHistory::getCWengoPhone() const {
	return _cWengoPhone;
}

History & CHistory::getHistory() {
	return _history;
}

void CHistory::historyMementoAddedEventHandler(History & history, int id) {
	mementoAddedEvent(*this, id);
}

void CHistory::historyMementoUpdatedEventHandler(History & history, int id) {
	mementoUpdatedEvent(*this, id);
}

void CHistory::historyMementoRemovedEventHandler(History & history, int id) {
	mementoRemovedEvent(*this, id);
}

void CHistory::historyLoadedEventHandler(History & history) {
	historyLoadedEvent(*this);
}

void CHistory::removeHistoryMemento(int id) {
	_history.removeMemento(id);
}

void CHistory::clear() {
	_history.clear();
}
