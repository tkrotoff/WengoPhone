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

#include <cutil/log.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#if !defined(OS_WINDOWS)
	static const int MAX_PATH = 256;
#endif

enum Level {
	Debug,
	Info,
	Warn,
	Error,
	Fatal
};

static void logPrivate(enum Level level, const char * class_name, const char * message, const char * filename, int line) {
	char * level_string;
	switch (level) {
	case Debug:
		level_string = strdup("debug");
		break;
	case Info:
		level_string = strdup("info");
		break;
	case Warn:
		level_string = strdup("warn");
		break;
	case Error:
		level_string = strdup("error");
		break;
	case Fatal:
		level_string = strdup("fatal");
		break;
	default:
		assert(NULL && "unknown log level");
	}

	if (filename) {
		fprintf(stderr, "(%s) %s:%d %s: %s\n", level_string, filename, line, class_name, message);
	} else {
		fprintf(stderr, "(%s) %s: %s\n", level_string, class_name, message);
	}
}

void log_debug(const char * class_name, const char * message) {
	logPrivate(Debug, class_name, message, NULL, -1);
}

void log_info(const char * class_name, const char * message) {
	logPrivate(Info, class_name, message, NULL, -1);
}

void log_warn(const char * class_name, const char * message) {
	logPrivate(Warn, class_name, message, NULL, -1);
}

void log_error(const char * class_name, const char * message) {
	logPrivate(Error, class_name, message, NULL, -1);
}

void log_fatal(const char * class_name, const char * message, const char * filename, int line) {
	logPrivate(Fatal, class_name, message, filename, line);
	assert(NULL && "fatal error");
}
