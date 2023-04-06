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


#include <webcam_c/webcam_c.h>

#include "VisioOutput.h"
#include "VisioPopupMenu.h"

#include "MyWidgetFactory.h"
#include "sip/SipPrimitives.h"
#include "Softphone.h"
#include "login/User.h"
#include "contact/Contact.h"
#include "util/EventFilter.h"
#include "config/Video.h"
#include "config/Config.h"
#include "SessionWindow.h"
#include "PhonePageWidget.h"
#include "AudioCallManager.h"
#include "AudioCall.h"
#include "util/emit_signal.h"

#include <qwidget.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qdatetime.h>
#include <qlayout.h>
#include <qtextedit.h>
#include <qapplication.h>
#include <qvaluelist.h>
#include <qimage.h>
#include <qpicture.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qsizepolicy.h>
#include <qevent.h>
#include <qdatetime.h>

#include <phapi.h>

#include <iostream>
using namespace std;

const float VisioOutputWidget::VIDEO_RATIO = 1.22;
FrameProcessorThread * VisioOutputWidget::process_thread_1 = 0;

class VisioRedrawFilter : public EventFilter {
public:
	VisioRedrawFilter(QObject *receiver, const char *member)
	: EventFilter(receiver, member) {}

protected:
	virtual bool eventFilter(QObject *o, QEvent *e);
};

bool VisioRedrawFilter::eventFilter(QObject *o, QEvent *e) {
	if (e->type() == QEvent::Paint) {
		filter();
		return true;
	}
	return EventFilter::eventFilter(o, e);
}

VisioOutputWidget::VisioOutputWidget(SessionWindow * sessionWindow, VisioPageWidget * visioPageWidget)
: QObject(visioPageWidget->getWidget()) {

	_visioWidget = (QFrame *) visioPageWidget->getWidget()->child("visioWidget", "QFrame");
	_visioWidget->reparent(	visioPageWidget->getWidget(),
							QWidget::WRepaintNoErase,
							QPoint(0, 0));

	_sessionWindow = sessionWindow;
	_cachedImage = NULL;

	_freezeVisio = false;

	_visioPageWidget = visioPageWidget;

	_fullscreen = true;

	QPushButton * hangUpButton = (QPushButton *) _visioWidget->child("hangUpButton");
	connect(hangUpButton, SIGNAL(clicked()),
		this, SLOT(hangUp()));

	QPushButton * fullscreenButton = (QPushButton *) _visioWidget->child("fullscreenButton");
	connect(fullscreenButton, SIGNAL(clicked()),
		this, SLOT(fullscreen()));

	QPushButton * flipButton = (QPushButton *) _visioWidget->child("flipButton");
	connect(flipButton, SIGNAL(clicked()),
		this, SLOT(flip()));
	Video & videoConf = Video::getInstance();
	if (videoConf.getFlip()) {
		flip();
	}

	AudioCallManager & audioCallManager = AudioCallManager::getInstance();
	connect(&audioCallManager, SIGNAL(endCall()),
		this, SLOT(unfullscreen()));

	video_op_lock = new QMutex();

	if (!process_thread_1) {
		process_thread_1 = new FrameProcessorThread();
		process_thread_1->start();
	}

	/*
	VisioRedrawFilter *redrawFilter = new VisioRedrawFilter(this,
		SLOT(redraw()));
	_visioWidget->installEventFilter(redrawFilter);
	*/

	//Fullscreen when double click
	MouseButtonDblClickEventFilter * mouseButtonDblClickEventFilter =
		new MouseButtonDblClickEventFilter(this, SLOT(fullscreen()));
	_visioWidget->installEventFilter(mouseButtonDblClickEventFilter);

	//Fullscreen when key 'f' pressed
	FKeyPressEventFilter * fKeyPressEventFilter =
		new FKeyPressEventFilter(this, SLOT(fullscreen()));
	_visioWidget->installEventFilter(fKeyPressEventFilter);

	//Fullscreen when key 'F11' pressed
	F11KeyPressEvent * f11KeyPressEventFilter =
		new F11KeyPressEvent(this, SLOT(fullscreen()));
	_visioWidget->installEventFilter(f11KeyPressEventFilter);

	//Fullscreen when key alt-return pressed
	AltReturnKeyPressEvent * altReturnKeyPressEvent =
		new AltReturnKeyPressEvent(this, SLOT(fullscreen()));
	_visioWidget->installEventFilter(altReturnKeyPressEvent);

	//Fullscreen when key escape pressed
	EscapeKeyPressEvent * escapeKeyPressEvent =
		new EscapeKeyPressEvent(this, SLOT(fullscreen()));
	_visioWidget->installEventFilter(escapeKeyPressEvent);

	ResizeEventFilter * resizeEvent = new ResizeEventFilter(this, SLOT(resize()));
	_visioWidget->installEventFilter(resizeEvent);

	recalculateSizes();
}

void VisioOutputWidget::recalculateSizes() {
	QWidget * hoster;

	hoster = getHoster();

	video_op_lock->lock();
	process_thread_1->recalculate(this, hoster, true);
	video_op_lock->unlock();
}

void VisioOutputWidget::resize() {
	static QTimer * timer = new QTimer(this);

	_freezeVisio = true;

	timer->stop();
	timer->start(150, true);
	timer->disconnect(this);
	connect(timer, SIGNAL(timeout()),
		this, SLOT(ratio()));

}

void VisioOutputWidget::redraw() {
	QWidget * hoster;

	hoster = getHoster();

	_cachedImage = &process_thread_1->img_remote;

	_painter.begin(hoster);

	_painter.scale((double) hoster->width() / _cachedImage->width(),
		(double) hoster->height() / _cachedImage->height());

	_painter.drawImage(0, 0, *_cachedImage, 0, 0, _cachedImage->width(),
						_cachedImage->height());

	_painter.end();
}

QWidget *VisioOutputWidget::getHoster() {
	QWidget *hoster;
	if (_sessionWindow->getCurrentPageIndex() == SessionWindow::INDEX_PHONE_PAGE) {
		PhonePageWidget & phonePage = _sessionWindow->getPhonePageWidget();
		hoster = phonePage.getVideoWidget();
	} else {
		hoster = _visioWidget;
	}

	return hoster;
}

void VisioOutputWidget::customEvent(QCustomEvent * ev) {
	TemporaryEvent * te = (TemporaryEvent *) ev;
	//_cachedImage = te->frame;
	this->redraw();
}

void VisioOutputWidget::frameReceived(AudioCall * ca, phVideoFrameReceivedEvent_t * ev) {
	uint8_t * local_frame;
	ToProcessFrame * tpe;

	if (!_freezeVisio) {

		tpe = new ToProcessFrame();
		tpe->remote_frame = (AVPicture *)ev->frame_remote;
		tpe->local_frame = (AVPicture *)ev->frame_local;
		tpe->width = ev->frame_remote_width;
		tpe->height = ev->frame_remote_height;
		tpe->cam = ev->cam;
		tpe->receiver = this;

		if (!ca->isCalling()) {
			return;
		}

		process_thread_1->enqueueFrame((ToProcessFrame *) tpe);

	}
}

void VisioOutputWidget::ratio() {
	int diffHeight = ((float) _visioWidget->width() / VIDEO_RATIO) - _visioWidget->height();

	//Gets the SessionWindow widget without using AudioCall (impossible)
	QWidget * sessionWindow = _visioPageWidget->getSessionWindow();
	int width = sessionWindow->width();
	int height = sessionWindow->height() + diffHeight;
	if (height > 400) {
		height = 400;
		width = 488;
	}
	sessionWindow->resize(width, height);
	recalculateSizes();

	_freezeVisio = false;
}

void VisioOutputWidget::hangUp() {
	AudioCall * audioCall = AudioCallManager::getInstance().getActiveAudioCall();
	if (audioCall) {
		SipPrimitives::closeCall(audioCall->getCallId());
		emit_signal(&AudioCallManager::getInstance(), SIGNAL(endCall()));
	}
}

void VisioOutputWidget::flip() {
	static bool flip = true;

	Video & videoConf = Video::getInstance();
	videoConf.setFlip(flip);
	phVideoControlSetCameraFlip(flip);
	Config::getInstance().saveAsXml();
	flip = !flip;
}

void VisioOutputWidget::fullscreen(bool max_fs_scale) {
#if defined(WIN32)
	int nModeExist, nModeSwitch;
	DEVMODE devMode;
#endif

	_visioWidget->setFocus();
	if (_fullscreen) {
		QPushButton * fullscreenButton = (QPushButton *) _visioWidget->child("fullscreenButton");
		fullscreenButton->setPixmap(QPixmap::fromMimeSource("video_unfullscreen.png"));

		QLayout * layout = _visioPageWidget->getWidget()->layout();
		layout->remove(_visioWidget);

#if defined(WIN32)
		_visioWidget->reparent(0, 
					QWidget::WRepaintNoErase | Qt::WType_TopLevel | Qt::WStyle_NoBorder | QWidget::WStyle_StaysOnTop,
					QPoint(0, 0));
		// trying to switch resolution to a best bet resolution
		devMode.dmSize = sizeof(DEVMODE);
		EnumDisplaySettings(NULL, 0, &devMode);
		for (int i=1; ;i++)
		{
			nModeExist = EnumDisplaySettings(NULL, i, &devMode);
			if (nModeExist == 0)
			{
				// End of modes.  bail out.
				break;
			}
			if ((devMode.dmBitsPerPel == 32) && (devMode.dmPelsWidth == 640) && (devMode.dmPelsHeight == 480)) {
				nModeSwitch = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
				break;
			}
			cout << devMode.dmBitsPerPel << "," << devMode.dmPelsWidth   << "," << devMode.dmPelsHeight  << "," << endl;
		}
#else
		_visioWidget->reparent(0, 0, QPoint(0,0));
#endif
		_visioWidget->showFullScreen();
		_fullscreen = false;
	} else {
		unfullscreen();
	}

	recalculateSizes();
}

void VisioOutputWidget::unfullscreen() {
	if (!_fullscreen) {

		QPushButton * fullscreenButton = (QPushButton *) _visioWidget->child("fullscreenButton");
		fullscreenButton->setPixmap(QPixmap::fromMimeSource("video_fullscreen.png"));
		_visioWidget->reparent(_visioPageWidget->getWidget(), QWidget::WRepaintNoErase, QPoint(0, 0));
		QLayout * layout = _visioPageWidget->getWidget()->layout();
		layout->add(_visioWidget);

#if defined(WIN32)
		// restoring previous resolution under win32
		ChangeDisplaySettings(NULL, 0);
#endif

		_visioWidget->show();

		_fullscreen = true;
		recalculateSizes();
	}
}
