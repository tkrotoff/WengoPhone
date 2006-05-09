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
class PhoneCall;
class QtWengoPhone;
class QtVideo;

class QPushButton;
class QImage;
class QWidget;
class QLabel;
class QMenu;
class QAction;

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

	CPhoneCall & getCPhoneCall();

Q_SIGNALS:

	void startConference(PhoneCall * sourceCall,  PhoneCall * targetCall);

	void stopConference();

	void deleteMe(QtPhoneCall * qtPhoneCall);

	void callRejected();

public Q_SLOTS:

	void acceptActionTriggered(bool checked);

	void rejectActionTriggered(bool checked);

	void holdResumeActionTriggered(bool checked);

	void addContactActionTriggered(bool checked);

	void inviteToConference(bool checked);

	void transferButtonClicked();

	void switchVideo(bool checked);

	void openPopup(int x, int y);

private:

	void initThreadSafe();

	void updatePresentationThreadSafe();

	void stateChangedEventHandler(EnumPhoneCallState::PhoneCallState state);

	void stateChangedEventHandlerThreadSafe(EnumPhoneCallState::PhoneCallState state);

	void videoFrameReceivedEventHandler(const WebcamVideoFrame & remoteVideoFrame, const WebcamVideoFrame & localVideoFrame);

	void videoFrameReceivedEventHandlerThreadSafe(QImage * image);

	void showVideoWidget();

	void showAvatar();

	QMenu * createMenu();

    QMenu * createInviteMenu();

	QMenu * _popup;

	CPhoneCall & _cPhoneCall;

	QtWengoPhone * _qtWengoPhone;

	QWidget * _phoneCallWidget;

	QtVideo * _videoWindow;

	QLabel * _nickNameLabel;

	QLabel * _statusLabel;

	QLabel * _durationLabel;

	QLabel * _avatarLabel;

	QAction * _actionAcceptCall;

	QAction * _actionHangupCall;

	QAction * _actionHold;

	QAction * _actionResume;

	QAction * _actionInvite;

	QAction * _actionSwitchVideo;

	QAction * _actionAddContact;

	QAction * _actionBlockContact;

	int _timerId;

	int _duration;

	bool _hold;

	bool _showVideo;

	bool _encrustLocalWebcam;

protected:

	void timerEvent(QTimerEvent * event);
};

#endif	//QTPHONECALL_H
