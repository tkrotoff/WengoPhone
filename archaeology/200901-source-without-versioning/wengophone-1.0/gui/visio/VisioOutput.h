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

#ifndef VISIOOUTPUTWIDGET_H
#define VISIOOUTPUTWIDGET_H

#include "VisioPopupMenu.h"
#include "VisioPageWidget.h"
#include "WebcamEvent.h"
#include "AudioCall.h"
#ifdef ENABLE_VIDEO
#include "VisioFrameProcessor.h"
#endif

#include "util/WidgetStackControl.h"
#include "thread/QtThreadFactory.h"
#include "thread/Thread.h"
#include <observer/Observer.h>

#include <qwaitcondition.h>
#include <qptrqueue.h>
#include <qobject.h>
#include <qcheckbox.h>
#include <qwidgetstack.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qptrlist.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qmutex.h>
#include <qimage.h>
#include <qcolor.h>
#include <qsplashscreen.h>

#ifdef ENABLE_VIDEO

#include <avcodec.h>

class Event;
class QWidget;
class Video;
class SessionWindow;
class VisioPopupMenu;
class VisioPageWidget;

/**
 * @author David Ferlier
 * @author Mathieu Stute
 */
class ToProcessFrame {
public:
	ToProcessFrame() {};
	struct webcam *cam;
	AVPicture *remote_frame;
	AVPicture *local_frame;
	int width;
	int height;
	QObject *receiver;
};

class TemporaryEvent : public QCustomEvent {
public:
	enum Type {
		FrameReady = (QEvent::User + 200)
	};

	TemporaryEvent(Type t, QImage * f, int w, int h) : QCustomEvent(t) {
		width = w;
		height = h;
		frame = f;
	}

	~TemporaryEvent() {}
	QString msg() { return text;}
	QImage *frame;

	int width, height;

private:
	QString text;
};


class VisioOutputWidget : public QObject {
	Q_OBJECT
public:

	VisioOutputWidget(SessionWindow * sessionWindow, VisioPageWidget * visioPageWidget);

	QWidget * getWidget() const {
		return _visioWidget;
	}

	/**
	 * This function is registered as a callback on the lower grounds and is called when a frame is received.
	 *
	 * It receives a YUV420P frame from the network
	 */
	void frameReceived(AudioCall *ca, struct phVideoFrameReceivedEvent *ev);

	void recalculateSizes();
	QWidget *getHoster();

public slots:

	void fullscreen(bool max_fs_scale = true);

	void ratio();

private slots:

	void resize();
	void hangUp();
	void flip();
	void customEvent(QCustomEvent * ev);
	void unfullscreen();
	void redraw();

	QMutex * video_op_lock;

private:

	VisioPageWidget * _visioPageWidget;
	QWidget * _visioWidget;
	QImage *_cachedImage;
	SessionWindow * _sessionWindow;
	QPainter _painter;
	bool _freezeVisio;

	bool _fullscreen;

	static const float VIDEO_RATIO;

	static FrameProcessorThread * process_thread_1;
};

#endif	//ENABLE_VIDEO

#endif	//VISIOOUTPUTWIDGET_H
