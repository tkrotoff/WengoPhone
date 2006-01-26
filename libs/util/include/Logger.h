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
#include <Event.h>

#include <typeinfo>

class FileWriter;

/*
__FILE__ : file source name
__LINE__ : line number inside the source file
__DATE__ : compilation date
__TIME__ : compilation time
*/

#define TYPEID_C String(__FILE__) + ":" + String::fromNumber(__LINE__) + ":"
#define TYPEID_CPP typeid(this).name()

/** Macros for the Logger class, to use only inside C++ classes. */
#define LOG_DEBUG(message) Logger::logger.debug(TYPEID_CPP, message);
#define LOG_INFO(message) Logger::logger.info(TYPEID_CPP, message);
#define LOG_WARN(message) Logger::logger.warn(TYPEID_CPP, message);
#define LOG_ERROR(message) Logger::logger.error(TYPEID_CPP, message);
#define LOG_FATAL(message) Logger::logger.fatal(TYPEID_CPP, message);

/** Specific macros for C functions. */
#define LOG_DEBUG_C(message) Logger::logger.debug(TYPEID_C, message);
#define LOG_INFO_C(message) Logger::logger.info(TYPEID_C, message);
#define LOG_WARN_C(message) Logger::logger.warn(TYPEID_C, message);
#define LOG_ERROR_C(message) Logger::logger.error(TYPEID_C, message);
#define LOG_FATAL_C(message) Logger::logger.fatal(TYPEID_C, message);

/**
 * Logger class.
 *
 * Example taken from log4j:
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
 * @see http://developers.sun.com/solaris/articles/logging.html
 * @see http://logging.apache.org/log4j/docs/manual.html
 * @author Tanguy Krotoff
 */
class Logger : NonCopyable {
public:

	/**
	 * A message has been added to the logging system.
	 *
	 * @param message message added to the logging system
	 */
	Event<void (const std::string & message)> messageAddedEvent;

	/** Singleton. */
	static Logger logger;

	Logger();

	~Logger();

	void debug(const std::string & className, const std::string & message);

	void info(const std::string & className, const std::string & message);

	void warn(const std::string & className, const std::string & message);

	void error(const std::string & className, const std::string & message);

	void fatal(const std::string & className, const std::string & message);

	void operator<<(std::ostream & os);

	friend std::ostream & operator<<(std::ostream & os, const Logger & /*logger*/) {
		return os;
	}

private:

	enum Level {
		Debug,
		Info,
		Warn,
		Error,
		Fatal
	};

	void log(Level level, const std::string & className, const std::string & message);

	FileWriter * _file;
};

#endif	//LOGGER_H
