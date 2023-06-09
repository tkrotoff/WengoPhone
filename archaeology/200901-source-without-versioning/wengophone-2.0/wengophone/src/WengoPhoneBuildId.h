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

#ifndef WENGOPHONEBUILDID_H
#define WENGOPHONEBUILDID_H

/**
 * Identifies uniquely a revision of WengoPhone.
 *
 * @author Tanguy Krotoff
 */
class WengoPhoneBuildId {
public:

	/**
	 * Build id is the WengoPhone compilation date/time (e.g 2005/12/07 - 18:18:09)
	 *
	 * This is a unique id for each release.
	 * It permits WengoPhone to check if a more recent version of itself exist.
	 * Format: year/month/day hour:minutes:seconds
	 */
	static const unsigned long long BUILDID;

	/**
	 * Version name (e.g "1.2.1")
	 * Human readable representation of the build id.
	 *
	 * This is a unique string for each release.
	 */
	static const char * VERSION;

	/**
	 * Revision number (Subversion revision).
	 */
	static const unsigned long long REVISION;

	/**
	 * Softphone name when connecting on the Wengo platform.
	 * WL_TAG
	 */
	static const char * SOFTPHONE_NAME;

private:

	/** Avoid copying of this object. */

	WengoPhoneBuildId() { }
	WengoPhoneBuildId(const WengoPhoneBuildId &);
	WengoPhoneBuildId& operator=(const WengoPhoneBuildId &);
};

#endif	//WENGOPHONEBUILDID_H
