
#include "WengoPhoneBuildId.h"

#define stringize(x) #x
const unsigned long long WengoPhoneBuildId::BUILDID = DD_BUILDID;
const char * WengoPhoneBuildId::VERSION = stringize(DD_VERSION);
const unsigned long long WengoPhoneBuildId::REVISION = DD_REVISION;
const char * WengoPhoneBuildId::SOFTPHONE_NAME = stringize(DD_SOFTPHONE_NAME);
