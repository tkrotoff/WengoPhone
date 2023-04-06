/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include <model/config/Config.h>
#include <model/config/ConfigManager.h>
#include <model/config/ConfigManagerFileStorage.h>

#include <boost/program_options.hpp>
#include <iostream>
using namespace boost::program_options;
using namespace std;

#include <util/File.h>
#include <util/Logger.h>
#include <util/Path.h>
#include <util/StringList.h>

CommandLineParser::CommandLineParser(int argc, char * argv[]) {

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	
	_severalWengoPhoneAllowed = false;

	//Set executable name
	std::string executableName;
	if (argv[0]) {
		char * p = strrchr(argv[0], Path::getPathSeparator()[0]);
		executableName = std::string(p ? p + 1 : argv[0]);
		config.set(Config::EXECUTABLE_NAME_KEY, executableName);
	}

	//Reset all key to default
	config.resetToDefaultValue(Config::CMDLINE_BACKGROUND_MODE_ENABLE_KEY);
	config.resetToDefaultValue(Config::CMDLINE_PLACECALL_KEY);

	try {
		options_description desc("Allowed options");
		desc.add_options()
		//First parameter describes option name/short name
		//The second is parameter to option
		//The third is description
		("help,h", "print usage message")
		("background,b", "run in background mode")
		("several,s", "allow several WengoPhone at the same time")
		("configpath,q", value<string>(), "pass a path to load configuration directory")
		("command,c", value<string>(), "pass a command to the wengophone")
		;

		variables_map vm;
		store(parse_command_line(argc, argv, desc), vm);

		if (vm.count("help")) {
			cout << desc << endl;
			exit(0);
		}

		if (vm.count("background")) {
			LOG_DEBUG("run in background mode");
			config.set(Config::CMDLINE_BACKGROUND_MODE_ENABLE_KEY, true);
		}
		
		if (vm.count("several")) {
			LOG_DEBUG("allow several WengoPhone at the same time");
			_severalWengoPhoneAllowed = true;
		}
		

		if (vm.count("configpath")) {
			std::string newPath = vm["configpath"].as<string>();
			std::string pathSeparator = File::getPathSeparator();
			
			int newpathSize = newPath.size();
			if (newPath[newpathSize-1] != pathSeparator[0]) {
				
				// remove " at the end
				if (newPath[newpathSize-1] == '\"') {
					newPath = newPath.substr(0,newpathSize-1);
				}
				////
				
				// add path separator
				newPath += pathSeparator;
				////
			}
			
			// create directory if it does'nt exist
			if (!File::exists(newPath)) {
				File::createPath(newPath);
			}
			////
			
			// change config directory
			if (File::isDirectory(newPath)) {
				LOG_DEBUG("change configuration path to " + newPath);
				config.set(Config::CONFIG_DIR_KEY, newPath);
				
				ConfigManagerFileStorage configManagerStorage(ConfigManager::getInstance());
				configManagerStorage.load(config.getConfigDir());
				
			} else {
				LOG_ERROR("Can not change configuration path because " + newPath + " is NOT a directory");
			}
			////
		}
		
		if (vm.count("command")) {
			static String commandCall = "call/";
			String command = String(vm["command"].as<string>());

			//FIXME is it right to do that here?
			command.remove("wengo://");

			LOG_DEBUG("command:" + command);
			if (command.beginsWith(commandCall)) {
				config.set(Config::CMDLINE_PLACECALL_KEY, command.split("/")[1]);
			}
		}
	}
	catch(exception & e) {
		cerr << e.what() << endl;
	}
}

CommandLineParser::~CommandLineParser() {
}
