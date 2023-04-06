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

#ifndef OWCOMMAND_H
#define OWCOMMAND_H

#include "Menu.h"

class CoIpManager;

/**
 * Command interface used but the CoIpManager test program.
 *
 * @author Philippe Bernery
 */
class Command : public Menu {
public:

	Command(const std::string & name, const std::string & desc, CoIpManager & coIpManager);

	/**
	 * Returns the name of the command.
	 */
	std::string commandName() const { return _menuName; }

	/**
	 * Returns a description of the command.
	 */
	std::string commandDesc() const { return _commandDesc; }

	/**
	 * Executes the command.
	 */
	virtual void exec();

protected:

	/** The description of the command. */
	std::string _commandDesc;

	/** Link to CoIpManager. */
	CoIpManager & _coIpManager;

};

#endif //OWCOMMAND_H
