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

#include <util/Logger.h>

#include <QtCore/QTime>
#include <QtCore/QDate>
#include <QtCore/QStringList>

#include <util/Path.h>

#include <iostream>
#include <sstream>

#include <ctime>
#include <cassert>

Logger Logger::logger;

Logger::Logger() {
	_fileCreated = false;
}

Logger::~Logger() {
	flush();
	_file.close();
}

void Logger::debug(const std::string & className, const std::string & message) {
	log(Debug, className, message);
}

void Logger::info(const std::string & className, const std::string & message) {
	log(Info, className, message);
}

void Logger::warn(const std::string & className, const std::string & message) {
	log(Warn, className, message);
}

void Logger::error(const std::string & className, const std::string & message) {
	log(Error, className, message);
}

void Logger::fatal(const std::string & className, const std::string & message, const std::string & filename, const std::string & line) {
	log(Fatal, className, message, filename, line);
	flush();
	assert(NULL && "fatal error");
}

void Logger::log(Level level, const std::string & className, const std::string & message, const std::string & filename, const std::string & line) {
	QMutexLocker locker(&_mutex);

	static const char * CLASS_METHOD_SEPARATOR = "::";

	std::string levelString;

	switch (level) {
	case Debug:
		levelString = "debug";
		break;
	case Info:
		levelString = "info";
		break;
	case Warn:
		levelString = "warn";
		break;
	case Error:
		levelString = "error";
		break;
	case Fatal:
		levelString = "fatal";
		break;
	default:
		assert(NULL && "unknown log level");
	}

	std::string tmp = "(" + levelString + ")";
	if ((!filename.empty()) && (!line.empty())) {
		tmp += " " + filename + ":" + line;
	}
	tmp += " " + className + " " + message;

	if (!_fileCreated) {
		QString strippedClassName = QString::fromStdString(className);
		strippedClassName = strippedClassName.split(CLASS_METHOD_SEPARATOR)[0];
		std::string fileName = Path::getApplicationDirPath() + "log-" + strippedClassName.toStdString() + ".txt";
		_file.open(fileName.c_str());
		_file << "Log file=" << fileName << std::endl;
		_file << "Date=" << QDate::currentDate().toString().toStdString() << std::endl;
		_file << "Time=" << QTime::currentTime().toString().toStdString() << std::endl << std::endl;
		_fileCreated = true;
	}

	_file << tmp << std::endl;

#ifdef NDEBUG
	//Release mode, log debug output to log file, but not to stderr
	if (level > Debug) {
		std::cerr << tmp << std::endl;
	}
#else
	//Debug mode, log all output to stderr
	std::cerr << tmp << std::endl;
#endif

	messageAddedSignal(tmp);
}

void Logger::flush() {
	QMutexLocker locker(&_mutex);

	_file.flush();
}
