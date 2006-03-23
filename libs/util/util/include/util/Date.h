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

#ifndef DATE_H
#define DATE_H

#include <ctime>
#include <string>

/**
 * Represents a Date.
 *
 * @author Philippe Bernery
 */
class Date {
	friend class DateXMLSerializer;
public:

	/**
	 * Creates a date with the current time.
	 */
	Date();

	/**
	 * Copy constructor.
	 */
	Date(const Date & date);

	/**
	 * Creates a date from a day a month and a year.
	 *
	 * @param day day of the date
	 * @param month month of the date
	 * @param year year of the date
	 */
	Date(unsigned day, unsigned month, unsigned year);

	~Date();

	bool operator==(const Date & date) const;

	/**
	 * @return the day number of the date. Day range: 1-31.
	 */
	unsigned getDay() const {
		return _day;
	}

	/**
	 * Sets the date day.
	 *
	 * @see getDay()
	 */
	void setDay(unsigned day) {
		if (day < 1) {
			day = 1;
		} else if (day > 31) {
			day = 31;
		}

		_day = day;
	}

	/**
	 * @return the month number of the date. Month range: 1-12.
	 */
	unsigned getMonth() const {
		return _month;
	}

	/**
	 * Sets the date month.
	 *
	 * @see getMonth()
	 */
	void setMonth(unsigned month) {
		if (month < 1) {
			month = 1;
		} else if (month > 12) {
			month = 12;
		}

		_month = month;
	}

	/**
	 * Gets the date year. Year range: 0-infinite.
	 */
	unsigned getYear() const {
		return _year;
	}

	/**
	 * Sets date year.
	 *
	 * @see getYear()
	 */
	void setYear(unsigned year) {
		_year = year;
	}

	/**
	 * @return a string representing the date. (e.g: "dd/mm/yyyy")
	 */
	std::string toString() const;

private:
	unsigned _day;
	unsigned _month;
	unsigned _year;
};

#endif	//DATE_H
