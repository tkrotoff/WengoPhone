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

#ifndef CODECLIST_H
#define CODECLIST_H

#include <util/List.h>
#include <util/NonCopyable.h>

#include <string>

/**
 * Audio and video codec list for the SIP stack.
 *
 * FIXME NOT FINISHED YET
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class CodecList : NonCopyable, public List<std::string> {
public:

	/** Audio codecs. */
	static const std::string AUDIO_CODEC_ULAW = "PCMU";
	static const std::string AUDIO_CODEC_ALAW = "PCMA";
	static const std::string AUDIO_CODEC_ILBC = "ILBC";
	static const std::string AUDIO_CODEC_GSM = "GSM";
	static const std::string AUDIO_CODEC_AMR_NB = "AMR";
	static const std::string AUDIO_CODEC_AMR_WB = "AMR-WB";
	static const std::string AUDIO_CODEC_SPEEX = "SPEEX";

	/** Video codecs. */
	static const std::string VIDEO_CODEC_H263 = "H263";
	static const std::string VIDEO_CODEC_H264 = "H264";
	static const std::string VIDEO_CODEC_MPEG4 = "MPEG4";


};

#endif	//CODECLIST_H
