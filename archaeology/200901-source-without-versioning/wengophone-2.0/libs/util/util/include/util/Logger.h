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

#ifndef OWLOGGER_H
#define OWLOGGER_H

#include <util/NonCopyable.h>
#include <util/String.h>
#include <util/Event.h>

#include <cutil/global.h>

#include <boost/thread/mutex.hpp>

#include <fstream>

/*
__FILE__ : file source name
__LINE__ : line number inside the source file
__DATE__ : compilation date
__TIME__ : compilation time
__FUNCTION__ : function name
__PRETTY_FUNCTION__ : function name
*/

#ifdef CC_GCC
	#define __FUNCTION__ __PRETTY_FUNCTION__
#endif

/** Macros for the Logger class. */
#define LOG_DEBUG(message) Logger::logger.debug(std::string(__FUNCTION__), message)
#define LOG_INFO(message) Logger::logger.info(std::string(__FUNCTION__), message)
#define LOG_WARN(message) Logger::logger.warn(std::string(__FUNCTION__), message)
#define LOG_ERROR(message) Logger::logger.error(std::string(__FUNCTION__), message)
#define LOG_FATAL(message) Logger::logger.fatal(std::string(__FUNCTION__), message, std::string(__FILE__), String::fromNumber(__LINE__))

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

	~Logger();

	void debug(const std::string & className, const std::string & message);

	void info(const std::string & className, const std::string & message);

	void warn(const std::string & className, const std::string & message);

	void error(const std::string & className, const std::string & message);

	void fatal(const std::string & className, const std::string & message, const std::string & filename, const std::string & line);

	/**
	 * Flushes the logger.
	 */
	void flush();

private:

	Logger();

	enum Level {
		Debug,
		Info,
		Warn,
		Error,
		Fatal
	};

	void log(Level level, const std::string & className, const std::string & message,
		const std::string & filename = "", const std::string & line = "");

	/** Writes the log messages to a file named log.txt. */
	std::ofstream _file;

	/** File created or not. */
	bool _fileCreated;

	/** Ensures Logger is thread-safe. */
	mutable boost::mutex _mutex;
};

#endif	//OWLOGGER_H
