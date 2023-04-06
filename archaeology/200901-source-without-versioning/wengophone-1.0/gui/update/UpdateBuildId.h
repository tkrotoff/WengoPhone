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

#ifndef UPDATEBUILDID_H
#define UPDATEBUILDID_H

#include <qstring.h>

/**
 * Build id of the update: the last version of WengoPhone available.
 *
 * This class is a simple pattern messenger.
 *
 * @author Tanguy Krotoff
 */
class UpdateBuildId {
public:

	UpdateBuildId() {
	}

	/**
	 * Constructs a UpdateBuildId object.
	 *
	 * @param buildId update build id number
	 * @param url update URL (where to download the update)
	 * @param updateType kind of update, compulsery or not
	 * @param name update name
	 */
	UpdateBuildId(const Q_ULLONG buildId, const QString & url, const QString & updateType, const QString & name) {
		_buildId = buildId;
		_url = url;
		_updateType = updateType;
		_name = name;
	}

	/**
	 * Gets the update build id number.
	 *
	 * @return update build id number
	 */
	const Q_ULLONG getBuildId() const {
		return _buildId;
	}

	/**
	 * Gets the update URL (where to download the update).
	 *
	 * @return update URL
	 */
	QString getUrl() const {
		return _url;
	}

	/**
	 * Tells if the update is compulsery or not.
	 *
	 * @return true if the update is compulsery, false otherwise
	 */
	bool isCompulsery() const {
		if (_updateType == "must") {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Gets the update release name.
	 *
	 * @return update release name
	 */
	QString getName() const {
		return _name;
	}

private:

	/*UpdateBuildId();
	UpdateBuildId(const UpdateBuildId &);
	UpdateBuildId & operator=(const UpdateBuildId &);*/

	/**
	 * Update build id number.
	 */
	Q_ULLONG _buildId;

	/**
	 * Update URL.
	 */
	QString _url;

	/**
	 * Type of update (compulsery or not).
	 */
	QString _updateType;

	/**
	 * Update release name.
	 */
	QString _name;
};

#endif	//UPDATEBUILDID_H
