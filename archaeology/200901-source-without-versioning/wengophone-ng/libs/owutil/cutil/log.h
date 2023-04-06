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

#ifndef OWLOG_H
#define OWLOG_H

#include <cutil/owcutildll.h>

#include <cutil/global.h>

#ifdef __cplusplus
extern "C"
{
#endif

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
#define CLOG_DEBUG(message) log_debug(__FUNCTION__, message)
#define CLOG_INFO(message) log_info(__FUNCTION__, message)
#define CLOG_WARN(message) log_warn(__FUNCTION__, message)
#define CLOG_ERROR(message) log_error(__FUNCTION__, message)
#define CLOG_FATAL(message) log_fatal(__FUNCTION__, message, __FILE__, __LINE__)

/**
 * Logger functions.
 *
 * @see util/Logger.h
 * @see org.apache.log4j
 * @see java.util.logging.Logger
 * @see log4cpp
 * @see http://developers.sun.com/solaris/articles/logging.html
 * @see http://logging.apache.org/log4j/docs/manual.html
 * @author Tanguy Krotoff
 */

OWCUTIL_API void log_debug(const char * class_name, const char * message);

OWCUTIL_API void log_info(const char * class_name, const char * message);

OWCUTIL_API void log_warn(const char * class_name, const char * message);

OWCUTIL_API void log_error(const char * class_name, const char * message);

OWCUTIL_API void log_fatal(const char * class_name, const char * message, const char * filename, int line);

#ifdef __cplusplus
}
#endif

#endif	/* OWLOG_H */
