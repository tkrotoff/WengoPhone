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

#ifndef OWREGISTERPROTOCOL_H
#define OWREGISTERPROTOCOL_H

#include <string>

/**
 * Binds a protocol to a command.
 *
 * Registers protocols like http:// callto:// wengo://
 * Under Windows, register the new protocol inside the registry database.
 *
 * @see http://www.w3.org/Addressing/schemes-gen.html
 * @author Mathieu Stute
 */
class RegisterProtocol {
public:

	RegisterProtocol(const std::string & protocol);

	bool bind(const std::string & command, const std::string & icon = "", const std::string & url = "");

	bool isBinded() const;

	bool unbind();

private:

	std::string _protocol;
};

#endif	//OWREGISTERPROTOCOL_H
