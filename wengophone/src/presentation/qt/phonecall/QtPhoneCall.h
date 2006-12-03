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

#ifndef OWQTPHONECALL_H
#define OWQTPHONECALL_H

#include <presentation/PPhoneCall.h>

#include <pixertool/pixertool.h>

#include <QtCore/QObject>

class PhoneCall;
class CPhoneCall;
class QtVideo;
class QtWengoPhone;
class QtCallToaster;

class QMenu;
class QAction;
class QTimer;
class QWidget;
class QMutex;
namespace Ui { class PhoneCallWidget; }

/**
 * Qt Presentation component for PhoneCall.
 *
 * @author Tanguy Krotoff
 * @author Jerome Wagner
 * @author Mathieu Stute
 * @author Philippe Bernery
 */
class QtPhoneCall : public QObject, public PPhoneCall {
	Q_OBJECT
public:

	QtPhoneCall(CPhoneCall & cPhoneCall);

	virtual ~QtPhoneCall();

	void updatePresentation() { }

	QWidget * getWidget() const {
		return _phoneCallWidget;
	}

	CPhoneCall & getCPhoneCall() const {
		return _cPhoneCall;
	}

	bool isIncoming() const;

	void stateChangedEvent(EnumPhoneCallState::PhoneCallState state);

	void videoFrameReceivedEvent(piximage * remoteVideoFrame, piximage * localVideoFrame);

	void close();

Q_SIGNALS:

	void startConference(PhoneCall * sourceCall, PhoneCall * targetCall);

	void stopConference();

	void deleteMe(QtPhoneCall * qtPhoneCall);

	void callRejected();

public Q_SLOTS:

	void acceptActionTriggered(bool checked);

	void rejectActionTriggered(bool checked);

	void holdResumeActionTriggered(bool checked);

	void addContactActionTriggered(bool checked);

	void inviteToConference(bool checked);

	void openPopup(int x, int y);

	void acceptCall();

	void rejectCall();

	void toggleFlipVideoImage();

private Q_SLOTS:

	void updateCallDuration();

private:

	void showVideoWidget();

	void showAvatar();

	void showToaster(const QString & userName);

	static QString getDisplayName(QString str);

	QMenu * createMenu() const;

	QMenu * createInviteMenu() const;

	Ui::PhoneCallWidget * _ui;

	QWidget * _phoneCallWidget;

	QMenu * _popupMenu;

	CPhoneCall & _cPhoneCall;

	QtWengoPhone * _qtWengoPhone;

	QtVideo * _videoWindow;

	QAction * _actionAcceptCall;

	QAction * _actionHangupCall;

	QAction * _actionHold;

	QAction * _actionResume;

	QAction * _actionInvite;

	QAction * _actionAddContact;

	QAction * _actionBlockContact;

	/** Computes the call duration. */
	QTimer * _callTimer;

	int _duration;

	bool _hold;

	bool _showVideo;

	//buffers to free in desctructor
	piximage * _remoteVideoFrame;
	piximage * _localVideoFrame;

	//FIXME hack in order to prevent a crash
	bool _closed;

	/** protect videoFrameReceivedEvent */
	QMutex * _mutex;

	QtCallToaster * _callToaster;
};

#endif	//OWQTPHONECALL_H
