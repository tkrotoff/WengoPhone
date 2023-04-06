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

#ifndef OWENUMUPDATESECTION_H
#define OWENUMUPDATESECTION_H

#include <util/NonCopyable.h>

/**
 * EnumUpdateSection is used by Data managers to tell which part of
 * a class has been updated. This gives possibility to optimize some
 * part of code.
 *
 * @author Philippe Bernery
 */
class EnumUpdateSection : NonCopyable {
public:

	enum UpdateSection {
		/** Used when an unknown section has been updated. */
		UpdateSectionUnknown,

		/** Used when alias has been updated. */
		UpdateSectionAlias,

		/** Used when icon has been updated. */
		UpdateSectionIcon,

		/** Used when presence state or connection state has been updated. */
		UpdateSectionPresenceState,

		/** Used when groups has been update. */
		UpdateSectionGroup,

		/**
		 * Used when a volatile attribute has been updated, an attribute
		 * that does not need to be serialized.
		 */
		UpdateSectionVolatileData,
	};
};

#endif	//OWENUMUPDATESECTION_H
