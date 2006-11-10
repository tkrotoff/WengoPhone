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

#include <util/String.h>

#include <curl/curl.h>
#include <portaudio.h>
#include <boost/version.hpp>
#include <avcodec.h>
#include <tinyxml.h>
extern "C" {
#include <glib.h>
#include <gaim/core.h>
}

const unsigned long long WengoPhoneBuildId::BUILDID = DD_BUILDID;
const char * WengoPhoneBuildId::VERSION = DD_VERSION;
const char * WengoPhoneBuildId::REVISION = DD_REVISION;
const char * WengoPhoneBuildId::SOFTPHONE_NAME = DD_SOFTPHONE_NAME;
const char * WengoPhoneBuildId::LIBBOOST_VERSION = BOOST_LIB_VERSION;
const char * WengoPhoneBuildId::GAIM_VERSION = gaim_core_get_version();
const char * WengoPhoneBuildId::CURL_VERSION = curl_version();
const char * WengoPhoneBuildId::PORTAUDIO_VERSION = Pa_GetVersionText();
const char * WengoPhoneBuildId::AVCODEC_VERSION = String::fromNumber(avcodec_version()).c_str();
const char * WengoPhoneBuildId::TINYXML_VERSION = std::string(
			String::fromNumber(TIXML_MAJOR_VERSION) + "." +
			String::fromNumber(TIXML_MINOR_VERSION) + "." +
			String::fromNumber(TIXML_PATCH_VERSION)).c_str();
