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

#include <sipwrapper/CodecList.h>

#include <util/Logger.h>

std::string CodecList::toString(AudioCodec audioCodec) {
	switch(audioCodec) {
	case AudioCodecError:
		return "Error";

	case AudioCodecPCMU:
		return "PCMU";

	case AudioCodecPCMA:
		return "PCMA";

	case AudioCodecILBC:
		return "ILBC";

	case AudioCodecGSM:
		return "GSM";

	case AudioCodecAMRNB:
		return "AMR-NB";

	case AudioCodecAMRWB:
		return "AMR-WB";

	case AudioCodecSPEEXNB:
		return "Speex-NB";

	case AudioCodecSPEEXWB:
		return "Speex-WB";

	default:
		LOG_FATAL("unknown audioCodec=" + String::fromNumber(audioCodec));
	}
}

std::string CodecList::toString(VideoCodec videoCodec) {
	switch(videoCodec) {
	case VideoCodecError:
		return "Error";

	case VideoCodecH263:
		return "H263";

	case VideoCodecH264:
		return "H264";

	case VideoCodecMPEG4:
		return "MPEG4";

	default:
		LOG_FATAL("unknown videoCodec=" + String::fromNumber(videoCodec));
	}
}
