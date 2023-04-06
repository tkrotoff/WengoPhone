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

#include "VisioPageWidget.h"
#include "VideoCodecConfigWidget.h"

#include <webcam/webcam.h>

#include "MyWidgetFactory.h"
#include "sip/SipPrimitives.h"
#include "Softphone.h"
#include "AudioCall.h"
#include "login/User.h"
#include "contact/Contact.h"
#include "util/EventFilter.h"
#include "config/Video.h"
#include "SessionWindow.h"
#include "AudioCall.h"

#include <qwidget.h>
#include <qsplitter.h>
#include <qdatetime.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <qcolor.h>
#include <qvaluelist.h>
#include <qpainter.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qgroupbox.h>

#include <phapi.h>
#include <avcodec.h>

#include <iostream>
using namespace std;

VisioPageWidget::VisioPageWidget(SessionWindow * sessionWindow, AudioCall & call)
: QObject(sessionWindow->getWidget()), _audioCall(call) {
	_sessionWindow = sessionWindow->getWidget();

	_visioPageWidget = MyWidgetFactory::create("VisioPageWidgetForm.ui", this, _sessionWindow);
	assert(_visioPageWidget != NULL && "visio page widget cannot be NULL");
	_visioPageWidget->resize(0, 0);

	_visioOutput = new VisioOutputWidget(sessionWindow, this);
	_visioOutput->getWidget()->resize(0, 0);

#ifdef WIN32
	phGetConfig()->videoHandle = (HWND) _visioOutput->getWidget()->winId();
#endif

#ifdef ENABLE_CODEC_CONFIG
	VideoCodecConfigWidget * videoConf = new VideoCodecConfigWidget(0, _audioCall);
	videoConf->getWidget()->show();
#endif
}

void VisioPageWidget::recalculateSizes() {
	_visioOutput->recalculateSizes();
}

void VisioPageWidget::frameReceived(AudioCall *ca, phVideoFrameReceivedEvent_t *ev) {
	_visioOutput->frameReceived(ca, ev);
}

void VisioPageWidget::repack() {
	_visioOutput->ratio();
}

void VisioPageWidget::initDefaultQuality() {
	Video::getInstance();
}
