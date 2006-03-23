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

#include <util/Date.h>

#include <util/String.h>

Date::Date() {
	std::time_t curTime = time(NULL);
	struct std::tm *timeinfo = std::localtime(&curTime);
	_day = timeinfo->tm_mday;
	_month = timeinfo->tm_mon + 1;
	_year = timeinfo->tm_year + 1900;
}

Date::Date(const Date & date) {
	_day = date._day;
	_month = date._month;
	_year = date._year;
}

Date::Date(unsigned day, unsigned month, unsigned year) {
	setDay(day);
	setMonth(month);
	setYear(year);
}

Date::~Date() {
}

bool Date::operator==(const Date & date) const {
	return ((_day == date._day)
		&& (_month == date._month)
		&& (_year == date._year));
}

std::string Date::toString() const {
	return String::fromNumber(_day) + "/" + String::fromNumber(_month) + "/" + String::fromNumber(_year);
}
