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

#include "WengoPhoneBuildId.h"

#define stringize2(x) #x
#define stringize(x) stringize2(x)

const unsigned long long WengoPhoneBuildId::BUILDID = DD_BUILDID;
const char * WengoPhoneBuildId::VERSION = stringize(DD_VERSION);
const char * WengoPhoneBuildId::REVISION = stringize(DD_REVISION);
const char * WengoPhoneBuildId::SOFTPHONE_NAME = stringize(DD_SOFTPHONE_NAME);
