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

#ifndef DATE_H
#define DATE_H

#include <util/StringList.h>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/archive_exception.hpp>

#include <string>

#include <ctime>

/**
 * Represents a Date.
 *
 * @author Philippe Bernery
 */
class Date {
public:

	static const unsigned int SERIALIZATION_VERSION = 1;

	/**
	 * Creates a date with the current time.
	 */
	Date() {
		std::time_t curTime = time(NULL);
		struct std::tm *timeinfo = std::localtime(&curTime);
		_day = timeinfo->tm_mday;
		_month = timeinfo->tm_mon + 1;
		_year = timeinfo->tm_year + 1900;
	}

	/**
	 * Copy constructor.
	 */
	Date(const Date &d) {
		_day = d._day;
		_month = d._month;
		_year = d._year;
	}

	/**
	 * Creates a date from a day a month and a year.
	 *
	 * @param day day of the date
	 * @param month month of the date
	 * @param year year of the date
	 */
	Date(unsigned day, unsigned month, unsigned year) {
		setDay(day);
		setMonth(month);
		setYear(year);
	}

	~Date() {}

	bool operator==(const Date & date) const {
		return ((_day == date._day)
			&& (_month == date._month)
			&& (_year == date._year));
	}

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
	 * @return the month number of the date, month range: 1-12
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
	 * Gets the date year.
	 *
	 * Year range: 0-infinite.
	 */
	unsigned getYear() const {
		return _year;
	}

	/**
	 * Sets the date year.
	 *
	 * @see getYear()
	 */
	void setYear(unsigned year) {
		_year = year;
	}

	/**
	 * @return a string representing the date (e.g: "dd/mm/yyyy")
	 */
	std::string toString() const {
		return String::fromNumber(_day) + "/" + String::fromNumber(_month) + "/" + String::fromNumber(_year);
	}

private:

	friend class boost::serialization::access;

	template<class Archive>
	void load(Archive & ar, const unsigned int version) {
		if (version == SERIALIZATION_VERSION) {
			unsigned day;
			unsigned month;
			unsigned year;

			ar >> BOOST_SERIALIZATION_NVP(day);
			ar >> BOOST_SERIALIZATION_NVP(month);
			ar >> BOOST_SERIALIZATION_NVP(year);

			_day = day;
			_month = month;
			_year = year;
		}
	}

	template<class Archive>
	void save(Archive & ar, const unsigned int version) const {
		unsigned day= _day;
		unsigned month = _month;
		unsigned year = _year;

		ar << BOOST_SERIALIZATION_NVP(day);
		ar << BOOST_SERIALIZATION_NVP(month);
		ar << BOOST_SERIALIZATION_NVP(year);
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER()

	unsigned _day;
	unsigned _month;
	unsigned _year;
};

BOOST_CLASS_VERSION(Date, Date::SERIALIZATION_VERSION)

#endif //DATE_H
