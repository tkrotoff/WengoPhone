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

#include <purplewrapper/PurpleEnumPresenceState.h>

#include <cstring>

#define ST_AVAILABLE	"available"
#define ST_OFFLINE		"offline"
#define ST_INVISIBLE	"invisible"
#define ST_AWAY			"away"
#define ST_UNAVAILABLE	"unavailable"
#define ST_EXT_AWAY		"extended_away"
#define ST_BUSY			"busy"
#define ST_DND			"dnd"

const char * PurplePreState::getStatusId(EnumPresenceState::PresenceState status, EnumAccountType::AccountType accountType) {
	switch (status) {
	case EnumPresenceState::PresenceStateOnline:
		return ST_AVAILABLE;

	case EnumPresenceState::PresenceStateOffline:
		return ST_OFFLINE;

	case EnumPresenceState::PresenceStateInvisible:
		return ST_INVISIBLE;

	case EnumPresenceState::PresenceStateAway:
		return ST_AWAY;

	case EnumPresenceState::PresenceStateDoNotDisturb:
		if ((accountType == EnumAccountType::AccountTypeMSN)
			|| (accountType == EnumAccountType::AccountTypeYahoo))
			return ST_BUSY;
		else if ((accountType == EnumAccountType::AccountTypeAIM)
			|| (accountType == EnumAccountType::AccountTypeGTalk)
			|| (accountType == EnumAccountType::AccountTypeICQ)
			|| (accountType == EnumAccountType::AccountTypeJabber))
			return ST_DND;
		else
			return ST_UNAVAILABLE;

	default:
		return ST_AWAY;
	}
}

EnumPresenceState::PresenceState PurplePreState::getPresenceState(const char * statusId) {
	if (!strcmp(statusId, ST_AVAILABLE)) {
		return EnumPresenceState::PresenceStateOnline;
	}

	else if (!strcmp(statusId, ST_OFFLINE)) {
		return EnumPresenceState::PresenceStateOffline;
	}

	else if (!strcmp(statusId, ST_INVISIBLE)) {
		return EnumPresenceState::PresenceStateInvisible;
	}

	else if (!strcmp(statusId, ST_AWAY)) {
		return EnumPresenceState::PresenceStateAway;
	}

	else if (!strcmp(statusId, ST_UNAVAILABLE)
			|| !strcmp(statusId, ST_BUSY)
			|| !strcmp(statusId, ST_DND)) {
		return EnumPresenceState::PresenceStateDoNotDisturb;
	}

	else {
		return EnumPresenceState::PresenceStateAway;
	}
}
