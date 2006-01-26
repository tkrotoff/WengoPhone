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

#ifndef XPCOMPHONELINE_H
#define XPCOMPHONELINE_H

#include <presentation/PPhoneLine.h>

#include <List.h>
#include <NonCopyable.h>

#include <string>

class CPhoneLine;

class XPCOMPhoneLine : public PPhoneLine {
public:

	XPCOMPhoneLine(CPhoneLine & cPhoneLine);

	void phoneLineStateChangedEvent(PhoneLineState state, int lineId);

	void updatePresentation() { }

	int makeCall(const std::string & phoneNumber);

private:

	void updatePresentationThreadSafe() { }

	void initThreadSafe() { }

	void phoneLineStateChangedEventThreadSafe(PhoneLineState state, int lineId) { }

	CPhoneLine & _cPhoneLine;
};

class PhoneLineList : public List<XPCOMPhoneLine *>, NonCopyable {
public:

	/** Singleton. */
	static PhoneLineList & getInstance() {
		static PhoneLineList phoneLineList;

		return phoneLineList;
	}

private:

	PhoneLineList() { }
};

#endif	//XPCOMPHONELINE_H
