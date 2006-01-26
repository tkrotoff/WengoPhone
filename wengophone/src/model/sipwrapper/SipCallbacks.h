/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#ifndef SIPCALLBACKS_H
#define SIPCALLBACKS_H

#include <string>

class WengoPhone;
class LocalWebcam;
class VideoFrame;

/**
 * Callbacks for SIP stacks.
 *
 * This methods should be called inside the SIP stack callbacks handling system
 * (i.e SipXCallbacks and PhApiCallbacks).
 *
 * phApi states: [INCOMING], [ACCEPTING], [OUTGOING], [ESTABLISHED], [ONHOLD], [CLOSED]
 * phApi call events (in the right order):
 * phDIALING, phRINGING, phNOANSWER, phCALLBUSY, phCALLREDIRECTED,
 * phCALLOK, phCALLHELD, phCALLRESUMED, phHOLDOK, phRESUMEOK, 
 * phINCALL, phCALLCLOSED, phCALLERROR, phDTMF, phXFERPROGRESS,
 * phXFEROK, phXFERFAIL, phXFERREQ, phCALLREPLACED
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class SipCallbacks {
public:

	SipCallbacks(WengoPhone & wengoPhone);

	/**
	 * @name Call States
	 * @{
	 */

	static const int CALL_DEFAULT_STATE = 0;
	static const int CALL_ERROR = 1;
	static const int CALL_RESUME_OK = 2;
	static const int CALL_TALKING = 3;
	static const int CALL_DIALING = 4;
	static const int CALL_RINGING = 5;
	static const int CALL_CLOSED = 6;
	static const int CALL_INCOMING = 7;
	static const int CALL_HOLD_OK = 8;

	void callProgress(int callId, int status, const std::string & from);

	/** @} */

	/**
	 * @name Virtual Line States (SIP States)
	 * @{
	 */

	static const int LINE_DEFAULT_STATE = 0;
	static const int LINE_OK = 1;
	static const int LINE_PROXY_ERROR = 2;
	static const int LINE_SERVER_ERROR = 3;
	static const int LINE_TIMEOUT = 4;

	void registerProgress(int lineId, int status);

	/** @} */

	/**
	 * @name Video
	 * @{
	 */

	void videoFrameReceived(int callId, const VideoFrame & frame, const LocalWebcam & localWebcam);

	/** @} */

	WengoPhone & getWengoPhone() const {
		return _wengoPhone;
	}

private:

	/** WengoPhone instance in order to access the phone lines and the phone calls. */
	WengoPhone & _wengoPhone;
};


class VideoFrame {
public:

	VideoFrame() {
		_width = 0;
		_height = 0;
		_frame = NULL;
	}

	virtual ~VideoFrame() = 0;

	void setFrame(unsigned char * frame) {
		_frame = frame;
	}

	unsigned char * getFrame() const {
		return _frame;
	}

	void setWidth(int width) {
		_width = width;
	}

	int getWidth() const {
		return _width;
	}

	void setHeight(int height) {
		_height = height;
	}

	int getHeight() const {
		return _height;
	}

protected:

	int _width;

	int _height;

	unsigned char * _frame;
};


class LocalWebcam {
public:

	LocalWebcam() {
		_width = 0;
		_height = 0;
		_frame = NULL;
	}

	virtual ~LocalWebcam() = 0;

	void setFrame(unsigned char * frame) {
		_frame = frame;
	}

	unsigned char * getFrame() const {
		return _frame;
	}

	void setWidth(int width) {
		_width = width;
	}

	int getWidth() const {
		return _width;
	}

	void setHeight(int height) {
		_height = height;
	}

	int getHeight() const {
		return _height;
	}

protected:

	int _width;

	int _height;

	unsigned char * _frame;
};

#endif	//SIPCALLBACKS_H
