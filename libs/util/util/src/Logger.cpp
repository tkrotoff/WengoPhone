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

#include <util/Logger.h>

#include <util/File.h>
#include <util/Path.h>

#include <iostream>
#include <sstream>

#include <ctime>

using namespace std;

Logger Logger::logger;

Logger::Logger() {
	boost::mutex::scoped_lock scopedLock(_mutex);

	std::string fileName = Path::getApplicationDirPath() + "log.txt";

	//Up to 10 different log.txt files
	for (int i = 0; i < 10; i++) {
		FileReader fileReader(fileName);

		//Checks if the file already exists
		if (!fileReader.open()) {
			//File does not exist, lets create it and write into it
			_file = new FileWriter(fileName);
			break;
		} else {
			//Find another log file name
			fileName = Path::getApplicationDirPath() + "log" + String::fromNumber(i) + ".txt";
		}
	}
}

Logger::~Logger() {
	boost::mutex::scoped_lock scopedLock(_mutex);

	flush();
	delete _file;
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

void Logger::fatal(const std::string & className, const std::string & message) {
	boost::mutex::scoped_lock scopedLock(_mutex);

	log(Fatal, className, message);
	flush();
	assert(NULL && "Fatal error");
}

void Logger::log(Level level, const std::string & className, const std::string & message) {
	boost::mutex::scoped_lock scopedLock(_mutex);

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
	}

	time_t t = time(NULL);
	struct tm * localTime = localtime(&t);

	string timeMsg = String::fromNumber(localTime->tm_hour) + ":"
		+ String::fromNumber(localTime->tm_min) + ":"
		+ String::fromNumber(localTime->tm_sec);


	String classNameTmp = className;
	classNameTmp.remove("class");
	classNameTmp.remove("*");
	classNameTmp.remove(" ");
	std::string tmp = "(" + levelString + ") " + timeMsg + " " + classNameTmp +  ": " + message;
	std::cerr << tmp << std::endl;

	//messageAddedEvent(tmp);
	_file->write(tmp + "\n");
}

void Logger::flush() {
	boost::mutex::scoped_lock scopedLock(_mutex);

	_file->close();
}
