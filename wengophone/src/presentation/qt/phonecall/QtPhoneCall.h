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

#ifndef QTPHONECALL_H
#define QTPHONECALL_H

#include "presentation/PPhoneCall.h"

#include <QObjectThreadSafe.h>

class VideoFrame;
class LocalWebcam;
class CPhoneCall;
class QtWengoPhone;
class QPushButton;
class QImage;
class QWidget;
class QtVideo;

class QtPhoneCall : public QObjectThreadSafe, public PPhoneCall {
	Q_OBJECT
public:

	QtPhoneCall(CPhoneCall & cPhoneCall);

	void updatePresentation();

	void close();

	void videoFrameReceived(const VideoFrame & frame, const LocalWebcam & localWebcam);

	void phoneCallStateChangedEvent(PhoneCallState state, int lineId, int callId,
				const std::string & sipAddress, const std::string & userName, const std::string & displayName) { }

	QWidget * getWidget() const {
		return _phoneCallWindow;
	}

private Q_SLOTS:

	void acceptButtonClicked();

	void rejectButtonClicked();

	void muteButtonClicked();

	void holdButtonClicked();

	void addContactButtonClicked();

private:

	void initThreadSafe();

	void updatePresentationThreadSafe();

	void closeThreadSafe();

	void videoFrameReceivedThreadSafe(QImage *image);

	CPhoneCall & _cPhoneCall;

	QtWengoPhone * _qtWengoPhone;

	QWidget * _phoneCallWindow;

	QPushButton * _acceptButton;

	QPushButton * _rejectButton;

	QPushButton * _muteButton;

	QPushButton * _holdButton;

	QPushButton * _addContactButton;

	QtVideo * _videoWindow;
};

#endif	//QTPHONECALL_H
