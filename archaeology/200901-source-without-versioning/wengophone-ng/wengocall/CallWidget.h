/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWCALLWIDGET_H
#define OWCALLWIDGET_H

#include <sipwrapper/EnumPhoneCallState.h>

#include <QtCore/QMutex>
#include <QtGui/QImage>
#include <QtGui/QWidget>

class TCallSession;
class TCoIpManager;

namespace Ui { class CallWidget; }

/**
 * Test program for Call service of CoIpManager.
 *
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class CallWidget : public QWidget {
	Q_OBJECT
public:

	CallWidget(TCoIpManager &tCoIpManager);

	~CallWidget();

private Q_SLOTS:

	void callButtonClicked();

	void hangUpButtonClicked();

	void videoFrameReceivedSlot(QImage remoteVideoFrame, QImage localVideoFrame);

	void phoneCallStateChangedSlot(EnumPhoneCallState::PhoneCallState state);

	//void newCallSessionCreatedSlot(TCallSession * tCallSession);

private:

	Ui::CallWidget *_ui;

	TCallSession *_tCallSession;

	TCoIpManager &_tCoIpManager;

	QImage *_localImage, *_remoteImage;

};

#endif //OWCALLWIDGET_H
