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

#ifndef LOGGER_H
#define LOGGER_H

#include <NonCopyable.h>
#include <StringList.h>

#include <typeinfo>
#include <iostream>
#include <sstream>
#include <iosfwd>

/**
 * Logger class.
 *
 * Example:
 * <pre>
 * //get a logger instance named "com.foo"
 * Logger logger = Logger.getLogger("com.foo");
 *
 * //Now set its level. Normally you do not need to set the
 * //level of a logger programmatically. This is usually done
 * //in configuration files.
 * logger.setLevel(Level.INFO);
 *
 * Logger barlogger = Logger.getLogger("com.foo.Bar");
 *
 * //This request is enabled, because WARN >= INFO.
 * logger.warn("Low fuel level.");
 *
 * //This request is disabled, because DEBUG < INFO.
 * logger.debug("Starting search for nearest gas station.");
 *
 * //The logger instance barlogger, named "com.foo.Bar",
 * //will inherit its level from the logger named
 * //"com.foo" Thus, the following request is enabled
 * //because INFO >= INFO.
 * barlogger.info("Located nearest gas station.");
 *
 * //This request is disabled, because DEBUG < INFO.
 * barlogger.debug("Exiting gas station search");
 * </pre>
 *
 * @see org.apache.log4j
 * @see java.util.logging.Logger
 * @see log4cpp
 * @author Tanguy Krotoff
 */
class Logger : NonCopyable {
public:

	enum Level {
		Debug,
		Info,
		Warn,
		Error,
		Fatal
	};

	Logger() {
	}

	void debug(const std::string & className, const std::string & message) {
		log(Level::Debug, className, message);
	}

	void info(const std::string & className, const std::string & message) {
		log(Level::Info, className, message);
	}

	void warn(const std::string & className, const std::string & message) {
		log(Level::Warn, className, message);
	}

	void error(const std::string & className, const std::string & message) {
		log(Level::Error, className, message);
	}

	void fatal(const std::string & className, const std::string & message) {
		log(Level::Fatal, className, message);
	}

	void operator<<(std::ostream & os) {
		std::ostringstream ostr;
		ostr << os;
		log(Level::Debug, "", ostr.str());
	}

	friend std::ostream & operator<<(std::ostream & os, const Logger & /*logger*/) {
		return os;
	}

private:

	void log(Level level, const std::string & className, const std::string & message) {
		std::string levelString;

		switch(level) {
		case Level::Debug:
			levelString = "debug";
			break;
		case Level::Info:
			levelString = "info";
			break;
		case Level::Warn:
			levelString = "warn";
			break;
		case Level::Error:
			levelString = "error";
			break;
		case Level::Fatal:
			levelString = "fatal";
			break;
		}

		String classNameTmp = className;
		classNameTmp.remove("class");
		classNameTmp.remove("*");
		classNameTmp.remove(" ");
		std::string tmp = "(" + levelString + ") " + classNameTmp + ": " + message;
		std::cerr << tmp << std::endl;
	}
};

#endif	//LOGGER_H
