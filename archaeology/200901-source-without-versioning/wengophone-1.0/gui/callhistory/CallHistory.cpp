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

#include "CallHistory.h"

#include "AddCallLogEvent.h"
#include "RemoveCallLogEvent.h"
#include "database/Database.h"
using namespace database;
#include "database/FileTarget.h"
#include "database/FileSource.h"
#include "Softphone.h"

#include <exception/FileNotFoundException.h>

#include <iostream>
using namespace std;

CallHistory::CallHistory() {
}

CallHistory::~CallHistory() {
	eraseAll();
}

void CallHistory::addCallLog(CallLog & callLog, bool save) {
	_callLogList.push_back(&callLog);
	AddCallLogEvent event(callLog);
	notify(event);

	if (save) {
		//FIXME suboptimal, the entire callhistory is saved
		saveAsXml();
	}
}

bool CallHistory::removeCallLog(const CallLog & callLog) {
	int count = _callLogList.size();
	int i;
	for (i = 0; i < count; i++) {
		if (_callLogList[i] == & callLog) break;
	}
	if (i < count) {
		RemoveCallLogEvent event(callLog);
		notify(event);
		_callLogList.erase(_callLogList.begin() + i);
		//delete callLog;
		saveAsXml();
		return true;
	}
	return false;
}

unsigned int CallHistory::size() const {
	return _callLogList.size();
}

const CallLog & CallHistory::operator[] (unsigned int i) const {
	return *_callLogList[i];
}

CallLog & CallHistory::operator[] (unsigned int i) {
	return *_callLogList[i];
}

void CallHistory::reset() {
	//Deletes all the CallLog from the CallHistoryWidget
	for (unsigned int i = 0; i < _callLogList.size(); i++) {
		RemoveCallLogEvent event(operator[](i));
		notify(event);
	}

	eraseAll();
}

void CallHistory::eraseAll() {
	//Delete all the CallLog
	for (unsigned int i = 0; i < _callLogList.size(); i++) {
		//delete _callLogList[i];
	}

	_callLogList.clear();
}

CallHistory & CallHistory::loadFromXml() {
	FileSource * source = NULL;
	CallHistory * callHistory = NULL;
	try {
		source = new FileSource(Softphone::getInstance().getCallHistoryFile());

		QString xml = source->read();

		Database & database = Database::getInstance();
		callHistory = &database.transformCallHistoryFromXml(xml);
	} catch (const FileNotFoundException & e) {
		cerr << e.what() << endl;
		callHistory = new CallHistory();
	}
	delete source;
	return *callHistory;
}

void CallHistory::saveAsXml() {
	if (!Softphone::getInstance().isUserLogged()) {
		//Couldn't save the CallHistory since the user is not logged
		//thus we don't know where to save the XML file
		return;
	}

	Database & database = Database::getInstance();
	QString xml = database.transformCallHistoryToXml(*this);

	QString filePath = Softphone::getInstance().getCallHistoryFile();

	FileTarget * target = NULL;
	try {
		target = new FileTarget(filePath);
	} catch (const FileNotFoundException & e) {
		cerr << e.what() << endl;
		delete target;
		return;
	}

	database.load(xml, *target);
	delete target;
}
