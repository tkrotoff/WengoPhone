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

#ifndef QTPHONECALL_H
#define QTPHONECALL_H

#include <presentation/PPhoneCall.h>

#include <qtutil/QObjectThreadSafe.h>

class WebcamVideoFrame;
class CPhoneCall;
class QtWengoPhone;
class QtVideo;

class QPushButton;
class QImage;
class QWidget;

/**
 * Qt Presentation component for PhoneCall.
 *
 * @author Tanguy Krotoff
 */
class QtPhoneCall : public QObjectThreadSafe, public PPhoneCall {
	Q_OBJECT
public:

	QtPhoneCall(CPhoneCall & cPhoneCall);

	void updatePresentation();

	QWidget * getWidget() const {
		return _phoneCallWidget;
	}

private Q_SLOTS:

	void acceptButtonClicked();

	void rejectButtonClicked();

	void holdResumeButtonClicked();

	void addContactButtonClicked();

	void transferButtonClicked();

private:

	void initThreadSafe();

	void updatePresentationThreadSafe();

	void stateChangedEventHandler(EnumPhoneCallState::PhoneCallState state);

	void stateChangedEventHandlerThreadSafe(EnumPhoneCallState::PhoneCallState state);

	void videoFrameReceivedEventHandler(const WebcamVideoFrame & remoteVideoFrame, const WebcamVideoFrame & localVideoFrame);

	void videoFrameReceivedEventHandlerThreadSafe(QImage * image);

	CPhoneCall & _cPhoneCall;

	QtWengoPhone * _qtWengoPhone;

	QWidget * _phoneCallWidget;

	QPushButton * _acceptButton;

	QPushButton * _rejectButton;

	QPushButton * _holdResumeButton;

	QPushButton * _addContactButton;

	QtVideo * _videoWindow;

	bool _hold;
};

#endif	//QTPHONECALL_H
