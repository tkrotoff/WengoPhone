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

/*
 * @author David Ferlier
 * @author Jerome Wagner
 */

#ifndef VISIO_FRAME_PROCESSOR_H
#define VISIO_FRAME_PROCESSOR_H

#include "visio/VisioOutput.h"
#include "thread/QtThreadFactory.h"
#include "thread/Thread.h"

#include <qwaitcondition.h>
#include <qmutex.h>
#include <qptrqueue.h>
#include <qobject.h>
#include <qevent.h>
#include <qmutex.h>
#include <qimage.h>

#ifdef ENABLE_VIDEO
#include <avcodec.h>
#endif

class Event;
class QWidget;
class Video;
class ToProcessFrame;

class FrameProcessorThread : public Thread {
public:
	FrameProcessorThread();

	void run();
	void recalculate(QObject *, QWidget *, bool max_scale = false);
	void processFrame(ToProcessFrame *);
	void enqueueFrame(ToProcessFrame *);

	QPtrQueue<ToProcessFrame> framesQueue;
	QMutex mt;
	QWaitCondition frame_queued_cond;

	QImage img_remote;
	QImage img_local;
	uint8_t *img_remote_static_data;
	uint8_t *img_local_static_data;

private:

	/**
	 * Alloc processing buffers
	 */
	void allocProcessingBuffers();

	/**
	 * Free processing buffers
	 */
	void freeProcessingBuffers();

	QObject *_receiver;

	ImgReSampleContext *rz_context_remote;
	ImgReSampleContext *rz_context_local;

	/**
	 * Processing buffers
	 */
	AVPicture avp_rgb_localcapture_resized;
	AVPicture avp_yuv_localcapture_resized;
	AVPicture avp_rgb_localcapture_unresized;

	AVPicture avp_rgb_remoteframe_unresized;
	AVPicture avp_rgb_remoteframe_resized;
	AVPicture avp_yuv_remoteframe_resized;

	int base_width, base_height;
	int overlay_width, overlay_height;
	int incrust_x, incrust_y, incrust_width, incrust_height;
};

#endif
