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

#ifndef VISIOWIDGET_H
#define VISIOWIDGET_H

#include "VisioOutput.h"
#include "AudioCall.h"
#include <observer/Observer.h>
#include "util/WidgetStackControl.h"

#ifdef ENABLE_VIDEO
#include <webcam/webcam.h>
#endif

#include <qobject.h>
#include <qevent.h>

class VisioOutputWidget;
class Video;
class Event;
class SessionWindow;
class QWidget;

/**
 * The visio component tab of the session window
 *
 * @author David Ferlier
 * @author Mathieu Stute
 */
class VisioPageWidget : public QObject {
	Q_OBJECT
public:

	VisioPageWidget(SessionWindow * sessionWindow, AudioCall & call);

	QWidget * getWidget() const {
		return _visioPageWidget;
	}

	void setSessionWindow(QWidget * sessionWindow) {
		_sessionWindow = sessionWindow;
	}

	QWidget * getSessionWindow() const {
		return _sessionWindow;
	}

	void recalculateSizes();
	
	void frameReceived(AudioCall *ca, struct phVideoFrameReceivedEvent *ev);

	void initDefaultQuality();

public slots:

	void repack();

private:

	QWidget * _sessionWindow;
	QWidget * _visioPageWidget;

	VisioOutputWidget * _visioOutput;

	AudioCall & _audioCall;
};

#endif	//VISIOWIDGET_H
