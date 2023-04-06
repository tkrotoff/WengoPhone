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

#include <observer/Subject.h>

#include <observer/Observer.h>
#include <observer/Event.h>

#include <iostream>
using namespace std;

Subject::~Subject() {
	for (unsigned int i = 0; i < _observerList.size(); i++) {
		removeObserver(* _observerList[i]);
	}
}

void Subject::addObserver(Observer & observer) {
	_observerList.push_back(& observer);
	//cerr << "Subject: " + toString() << " observed " << this << endl;
}

bool Subject::removeObserver(Observer & observer) {
	int count = _observerList.size();
	int i;
	for (i = 0; i < count; i++) {
		if (_observerList[i] == & observer) {
			break;
		}
	}
	if (i < count) {
		_observerList.erase(_observerList.begin() + i);
		return true;
	}
	return false;
}

void Subject::notify(const Event & event) {
	//TODO why this method is called several times in a row?
	//cerr << "Subject: " << this << " notify() " << event.getTypeEvent() << endl;

	for (unsigned int i = 0; i < _observerList.size(); i++) {
		_observerList[i]->update(* this, event);
	}
}
