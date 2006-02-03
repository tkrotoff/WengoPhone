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

#include <Logger.h>

#include <File.h>

#include <iostream>
#include <sstream>

#include <time.h>

using namespace std;

Logger Logger::logger;

Logger::Logger() {
	_file = new FileWriter(File::getApplicationDirPath() + "log.txt");
}

Logger::~Logger() {
	_file->close();
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
	log(Fatal, className, message);
	assert(NULL && "Fatal error");
}

void Logger::operator<<(std::ostream & os) {
	std::ostringstream ostr;
	ostr << os;
	log(Debug, "", ostr.str());
}

void Logger::log(Level level, const std::string & className, const std::string & message) {
	std::string levelString;

	switch(level) {
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
	std::string tmp = "(" + levelString + ") - " + timeMsg + " - " + classNameTmp +  ": " + message;
	std::cerr << tmp << std::endl;

	messageAddedEvent(tmp);
	_file->write(tmp + "\n");
}
