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

#ifndef OWCOIPMANAGERTEST_H
#define OWCOIPMANAGERTEST_H

#include "Menu.h"

#include <vector>

class Account;
class CoIpManager;
class Contact;
class Command;

/**
 * Main class for CoIpManager test program.
 *
 * @author Philippe Bernery
 */
class CoIpManagerTest : public Menu {
public:

	CoIpManagerTest();

	~CoIpManagerTest();

	virtual void exec();

private:

	/**
	 * Init the CoIpManager.
	 */
	void initCoIpManager();

	/**
	 * Registers available commands.
	 */
	void initCommands();

	/**
	 * Init the main menu.
	 */
	void initMenu();

	/**
	 * @name Menu entry
	 * @{
	 */

	/** Quits the software. */
	void quit();

	/** @} */

	/** True if the soft must quit. */
	bool _quit;

	/** Commands list. */
	std::vector<Command *> _commandList;

	/** CoIpManager. */
	CoIpManager *_coIpManager;

};

#endif //OWCOIPMANAGERTEST_H
