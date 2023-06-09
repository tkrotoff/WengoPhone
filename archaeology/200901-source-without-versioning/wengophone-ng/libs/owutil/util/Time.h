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

#ifndef OWTIME_H
#define OWTIME_H

#include <util/owutildll.h>

#include <string>

/**
 * Represents a time.
 *
 * @author Mathieu Stute
 */
class Time {
	friend class TimeXMLSerializer;
public:

	/**
	 * Creates a time with the current time.
	 */
	OWUTIL_API Time();

	/**
	 * Copy constructor.
	 */
	OWUTIL_API Time(const Time & time);

	/**
	 * Creates a time from an hour  a minute and a second.
	 *
	 * @param hour hours of the time
	 * @param minute minutes of the time
	 * @param second seconds of the time
	 */
	OWUTIL_API Time(unsigned hour, unsigned minute, unsigned second);

	OWUTIL_API ~Time();

	OWUTIL_API bool operator==(const Time & time) const;

	/**
	 * @return the hour of the time. range: 0-23.
	 */
	OWUTIL_API unsigned getHour() const;

	/**
	 * Sets the hour.
	 */
	OWUTIL_API void setHour(unsigned hour);

	/**
	 * @return the minute of the time. range: 0-59.
	 */
	OWUTIL_API unsigned getMinute() const;

	/**
	 * Sets the minute.
	 */
	OWUTIL_API void setMinute(unsigned minute);

	/**
	 * Gets the second. range: 0-59.
	 */
	OWUTIL_API unsigned getSecond() const;

	/**
	 * Sets second.
	 */
	OWUTIL_API void setSecond(unsigned second);

	/**
	 * @return a string representing the time. (e.g: "hh:mm:ss")
	 */
	OWUTIL_API std::string toString() const;

private:
	unsigned _hour;
	unsigned _minute;
	unsigned _second;
};

#endif	//OWTIME_H
