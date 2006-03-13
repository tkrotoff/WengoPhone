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

#ifndef XPCOMWENGOPHONE_H
#define XPCOMWENGOPHONE_H

#include <presentation/PWengoPhone.h>

class CWengoPhone;
class Listener;

class XPCOMWengoPhone : public PWengoPhone {
public:

	XPCOMWengoPhone(CWengoPhone * cWengoPhone);

	static CWengoPhone & getCWengoPhone();

	void addPhoneLine(PPhoneLine * pPhoneLine);

	void wengoLoginStateChangedEvent(WengoPhone::LoginState state, const std::string & login, const std::string & password);

	void updatePresentation();

private:

	void addPhoneLineThreadSafe(PPhoneLine * pPhoneLine);

	void updatePresentationThreadSafe();

	void initThreadSafe();

	static CWengoPhone * _cWengoPhone;
};

#endif	//XPCOMWENGOPHONE_H
