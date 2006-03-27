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

#include <util/Time.h>

#include <util/String.h>

Time::Time() {
	std::time_t curTime = time(NULL);
	struct std::tm *timeinfo = std::localtime(&curTime);
	_hour = timeinfo->tm_hour;
	_minute = timeinfo->tm_min;
	_second = timeinfo->tm_sec;
}

Time::Time(const Time & time) {
	_hour = time._hour;
	_minute = time._minute;
	_second = time._second;
}

Time::Time(unsigned hour, unsigned minute, unsigned second) {
	setHour(hour);
	setMinute(minute);
	setSecond(second);
}

Time::~Time() {
}

bool Time::operator==(const Time & time) const {
	return ((_hour == time._hour)
		&& (_minute == time._minute)
		&& (_second == time._second));
}

std::string Time::toString() const {
	return String::fromNumber(_hour) + ":" + String::fromNumber(_minute) + ":" + String::fromNumber(_second);
}
