
#include "WengoPhoneBuildId.h"

#define stringize2(x) #x
#define stringize(x) stringize2(x)

const unsigned long long WengoPhoneBuildId::BUILDID = DD_BUILDID;
const char * WengoPhoneBuildId::VERSION = stringize(DD_VERSION);
const unsigned long long WengoPhoneBuildId::REVISION = DD_REVISION;
const char * WengoPhoneBuildId::SOFTPHONE_NAME = stringize(DD_SOFTPHONE_NAME);
const char * WengoPhoneBuildId::COPYRIGHT = "Copyright (C) 2004-2006 Wengo S.A.S.";
const char * WengoPhoneBuildId::LICENSE = 
          "This program is free software; you can redistribute it and/or\n"
          "modify it under the terms of the GNU General Public License\n"
          "as published by the Free Software Foundation; either version 2\n"
          "of the License, or (at your option) any later version.";

const char * WengoPhoneBuildId::DESCRIPTION = "WengoPhone is a VoIP application";
