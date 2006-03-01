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

#include <string.h>

#include "GaimEnumPresenceState.h"

const char *GaimPreState::_GaimPresenceStatus[] = {
		"available",
		"offline",
		"away",
		"unavailable",
		"extended_away",
		"invisible",
		NULL
};


const char *GaimPreState::GetStatusId(EnumPresenceState::PresenceState status) 
{
	return _GaimPresenceStatus[status];
}

EnumPresenceState::PresenceState GaimPreState::GetPresenceState(const char *StatusId)
{
	int i;

	for (i = 0; _GaimPresenceStatus[i]; i++)
		if (strcmp(_GaimPresenceStatus[i], StatusId) == 0)
			break;

	return (PresenceState)i;
}