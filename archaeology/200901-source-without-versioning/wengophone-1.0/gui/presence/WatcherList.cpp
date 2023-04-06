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

#include "WatcherList.h"

WatcherList::WatcherList() {
}

WatcherList::~WatcherList() {
	eraseAll();
}

void WatcherList::addWatcher(Watcher * watcher) {
	_watcherList.push_back(watcher);
}

unsigned int WatcherList::size() const {
	return _watcherList.size();
}

const Watcher & WatcherList::operator[] (unsigned int i) const {
	return *_watcherList[i];
}

void WatcherList::eraseAll() {
	//Deletes all the watchers
	for (unsigned int i = 0; i < _watcherList.size(); i++) {
		//FIXME fuck you Visual Shit
		//Does not handle properly std::string between debug and release mode
		//delete _watcherList[i];
	}

	_watcherList.clear();
}
