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

#include <imwrapper/IMAccountHandlerFileDataLayer.h>

#include <imwrapper/IMAccountHandler.h>

#include <File.h>
#include <Logger.h>

#include <string>

using namespace std;

IMAccountHandlerFileDataLayer::IMAccountHandlerFileDataLayer(IMAccountHandler & imAccountHandler)
: IMAccountHandlerDataLayer(imAccountHandler), _imAccountHandler(imAccountHandler) {
}

IMAccountHandlerFileDataLayer::~IMAccountHandlerFileDataLayer() {
}

bool IMAccountHandlerFileDataLayer::load(const string & url) {
	FileReader file(url);
	LOG_DEBUG("loading " + url);

	if (file.open()) {
		string data = file.read();
		file.close();

		_imAccountHandler.unserialize(data);

		LOG_DEBUG("file " + url + " loaded");
		return true;
	}

	return false;
}

bool IMAccountHandlerFileDataLayer::save(const string & url) {
	FileWriter file(url);
	LOG_DEBUG("saving " + url);

	file.write(_imAccountHandler.serialize());
	file.close();

	LOG_DEBUG(url + " saved");

	return true;
}
