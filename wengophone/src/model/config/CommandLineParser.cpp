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

#include "CommandLineParser.h"

#include "ConfigManager.h"
#include "Config.h"

#include <boost/program_options.hpp>
#include <iostream>
using namespace boost::program_options;
using namespace std;

#include <util/Logger.h>

CommandLineParser::CommandLineParser(int argc, char * argv[]) {

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//reset all key to default
	config.resetToDefaultValue(Config::GUI_BACKGROUND_KEY);

	try {
		options_description desc("Allowed options");
		desc.add_options()
		// First parameter describes option name/short name
		// The second is parameter to option
		// The third is description
		("help,h", "print usage message")
		("background,b", "run in background mode")
		;

		variables_map vm;
		store(parse_command_line(argc, argv, desc), vm);

		if (vm.count("help")) {
			cout << desc << endl;
			exit(0);
		}

		if (vm.count("background")) {
			LOG_DEBUG("run in background mode");
			config.set(Config::GUI_BACKGROUND_KEY, true);
		}
	}
	catch(exception & e) {
		cerr << e.what() << endl;
	}
}

CommandLineParser::~CommandLineParser() {
}
