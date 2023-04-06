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

#ifndef DATABASE_DATABASE_H
#define DATABASE_DATABASE_H

#include <singleton/Singleton.h>

class CallHistory;
class Contact;
class UpdateBuildId;
class User;
class Config;
class Authentication;
class QString;

namespace database {

class Source;
class Target;

/**
 * Database to access (read/write) datas from the hard disk or the network.
 *
 * Design Pattern Singleton
 * The database is implemented using the concept: ETL Extract Transform Load.
 *
 * @author Tanguy Krotoff
 */
class Database : public Singleton<Database> {
	friend class Singleton<Database>;
public:

	~Database();

	/**
	 * Extracts the datas from a source.
	 *
	 * Reads the datas from a specified source. Extracts a desired subset of data.
	 *
	 * @param source source from where the datas will be extracted (input)
	 * @return the raw data non interpreted (output)
	 */
	QString extract(const Source & source) const;

	/**
	 * Transforms the datas to a specified type.
	 *
	 * Interprets the raw data in input. Converts data to the desired state.
	 *
	 * @param data data source (input)
	 * @return data interpreted (output)
	 */
	Contact & transformFromVCard(const QString & data) const;

	/**
	 * @param contact data interpreted (input)
	 * @return raw data (output)
	 */
	QString transformToVCard(const Contact & contact) const;

	Config & transformConfigFromXml(const QString & data) const;

	QString transformConfigToXml(const Config & config) const;

	UpdateBuildId transformBuilIdFromXml(const QString & data) const;

	CallHistory & transformCallHistoryFromXml(const QString & data) const;

	QString transformCallHistoryToXml(const CallHistory & callHistory) const;

	QString transformAuthToXml(const Authentication & auth) const;

	Authentication & transformAuthFromXml(const QString & data) const;

	QString transformUserToXml(const User & user) const;

	User transformUserFromXml(const QString & data) const;

	/**
	 * Loads an interpreted data to a target.
	 *
	 * Writes the resulting data to a target database.
	 *
	 * @param contact raw data (input)
	 * @param target data target (output)
	 */
	void load(const QString & data, Target & target) const;

private:

	Database();

	Database(const Database &);
	Database & operator=(const Database &);
};

}

#endif	//DATABASE_DATABASE_H
