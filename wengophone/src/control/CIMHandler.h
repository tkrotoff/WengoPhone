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

#ifndef CIMHANDLER_H
#define CIMHANDLER_H

class IMHandler;
class CWengoPhone;
class IMChat;
class Presence;
class PIMHandler;

/**
 *
 * @ingroup control
 * @author Tanguy Krotoff
 */
class CIMHandler {
public:

	CIMHandler(IMHandler & imHandler, CWengoPhone & cWengoPhone);

	CWengoPhone & getCWengoPhone() const {
		return _cWengoPhone;
	}

private:

	void chatCreatedEventHandler(IMHandler & sender, IMChat & chat);

	void presenceCreatedEventHandler(IMHandler & sender, Presence & presence);

	/** Direct link to the model. */
	IMHandler & _imHandler;

	PIMHandler * _pIMHandler;

	CWengoPhone & _cWengoPhone;
};

#endif	//CIMHANDLER_H
