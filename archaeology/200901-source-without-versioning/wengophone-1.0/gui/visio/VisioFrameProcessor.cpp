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


#include "VisioOutput.h"
#include "VisioFrameProcessor.h"

#ifdef WIN32
#include "Fast2PassScale.h"
#endif

#include <webcam_c/webcam_c.h>

#include "config/Video.h"
#include "SessionWindow.h"
#include "AudioCallManager.h"
#include "AudioCall.h"

#include <qwidget.h>
#include <qdatetime.h>
#include <qtextedit.h>
#include <qapplication.h>
#include <qvaluelist.h>
#include <qimage.h>
#include <qpicture.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qevent.h>

#include <iostream>
using namespace std;

/**
 * @author David Ferlier
 * @author Jerome Wagner
 */
FrameProcessorThread::FrameProcessorThread() {
	rz_context_remote = rz_context_local = NULL;

	img_remote_static_data = NULL;
	img_local_static_data = NULL;

	base_width = 176;
	base_height = 144;

	overlay_width = base_width;
	overlay_height = base_height;

	incrust_width = base_width;
	incrust_height = base_height;

	allocProcessingBuffers();
}

void FrameProcessorThread::processFrame(ToProcessFrame *f) {
	uint8_t *dst, *src;
	struct webcam *localWebcam;
	int width, height;
	uint8_t *buf_rgb_remoteframe_resized;
	uint8_t *buf_rgb_localcapture_resized;

	localWebcam = f->cam;
	width = f->width;
	height = f->height;

	img_resample(rz_context_local,
		&avp_yuv_localcapture_resized,
		(AVPicture *)f->local_frame);

	img_convert(&avp_rgb_localcapture_resized, PIX_FMT_RGBA32,
		&avp_yuv_localcapture_resized, PIX_FMT_YUV420P,
		incrust_width, incrust_height);

	if ((overlay_width==base_width)&&(overlay_height==base_height)) {

		img_convert(&avp_rgb_remoteframe_resized, PIX_FMT_RGBA32,
			(AVPicture *)f->remote_frame, PIX_FMT_YUV420P,
			overlay_width, overlay_height);

	} else {
		img_resample(rz_context_remote,
			&avp_yuv_remoteframe_resized,
			(AVPicture *)f->remote_frame);
		img_convert(&avp_rgb_remoteframe_resized, PIX_FMT_RGBA32,
			&avp_yuv_remoteframe_resized, PIX_FMT_YUV420P,
			overlay_width, overlay_height);
	}

#if defined(WIN32) && defined(NOT_FOR_NOW)
	CFast2PassScale ScaleEngine;
	ScaleEngine.Scale((COLORREF *)avp_rgb_remoteframe_unresized.data[0], base_width, base_height,
			  (COLORREF *)avp_rgb_remoteframe_resized.data[0], overlay_width, overlay_height);
	ScaleEngine.Scale((COLORREF *)avp_rgb_localcapture_unresized.data[0], base_width, base_height,
			  (COLORREF *)avp_rgb_localcapture_resized.data[0], incrust_width, incrust_height);
#else

#endif

	buf_rgb_localcapture_resized = avp_rgb_localcapture_resized.data[0];
	buf_rgb_remoteframe_resized = avp_rgb_remoteframe_resized.data[0];

	memcpy(img_remote.bits(), buf_rgb_remoteframe_resized,
		overlay_width * overlay_height * 4);
	memcpy(img_local.bits(), buf_rgb_localcapture_resized,
		incrust_width * incrust_height * 4);

	/* UGLY HACKISH HACK */

	dst = (uint8_t *)img_remote.scanLine(incrust_y - 1);
	memset(dst + 4 * (incrust_x - 1), 0, (incrust_width + 2) * 4 * sizeof(uint8_t));
	dst = (uint8_t *)img_remote.scanLine(incrust_y + incrust_height);
	memset(dst + 4 * (incrust_x - 1), 0, (incrust_width + 2) * 4 * sizeof(uint8_t));

	for (int y = incrust_y; y < incrust_height + incrust_y; y += 1) {
		src = (uint8_t *)img_local.scanLine(y - incrust_y);
		dst = (uint8_t *)img_remote.scanLine(y);
		memcpy(dst + 4 * incrust_x, src, incrust_width * 4 * sizeof(uint8_t));
		memset(dst + 4 * (incrust_x - 1), 0, 4 * sizeof(uint8_t));
		memset(dst + 4 * (incrust_x + incrust_width), 0, 4 * sizeof(uint8_t));
	}

	QApplication::postEvent(f->receiver,
		new TemporaryEvent(TemporaryEvent::FrameReady,
			&img_remote, width, height));
}

void FrameProcessorThread::run() {
	int q_count, it;
	ToProcessFrame *fitem;

	while (isAlive()) {
		mt.lock();
		if (!frame_queued_cond.wait(&mt, 500)) {
			mt.unlock();
			continue;
		}

		q_count = framesQueue.count();
		if (q_count > 1) {
			cout << "frame processor overrun - " << q_count << endl;
		}
		for (it = 0; it < q_count; it += 1) {
			fitem = (ToProcessFrame *) framesQueue.dequeue();

			if (!fitem) {
				break;
			}
			if (it == (q_count - 1)) {
				processFrame(fitem);
			}
			delete fitem;

		}
		mt.unlock();
	}
}

void FrameProcessorThread::enqueueFrame(ToProcessFrame *f) {
	mt.lock();
	framesQueue.enqueue(f);
	frame_queued_cond.wakeAll();
	mt.unlock();
}

void FrameProcessorThread::recalculate(QObject * recv, QWidget *weed, bool max_scale) {
	int widget_width, widget_height;
	int unmax_width = 704, unmax_height = 576;

	mt.lock();

	_receiver = recv;

	// close and release all buffers and structures that were necessary for the previous viewing size
	freeProcessingBuffers();

	// identify the new sizes of the different components
	// some are forced to be even to better handle YUV420P requirements
	widget_width = weed->width();
	widget_height = weed->height();

	// JWA: different upscaling algos are tested here
	/*
	// algo 1
	if (max_scale) {
		overlay_width = (widget_width);
		overlay_height = (widget_height);
	} else {
		if (widget_width < unmax_width) {
			overlay_width = widget_width;
		} else {
			overlay_width = unmax_width;
		}
		if (widget_height < unmax_height) {
			overlay_height = widget_height;
		} else {
			overlay_height = unmax_height;
		}
	}
	if (overlay_width> (unmax_width/2)) {
		overlay_width /= 2;
		overlay_height /= 2;
	}
	*/

/*
	// algo 2 : max overlay size before qpainter.scale() is the rx h263 size: 176x144
	overlay_width = widget_width;
	overlay_height = widget_height;
	if (overlay_width > 352) {
		overlay_width = 352;
	}
	if (overlay_height > 288) {
		overlay_height = 288;
	}
*/
/*
	// algo 3 : when it is necessary to resample, resample x 2 or /2
	overlay_width = base_width * 2;
	overlay_height = base_height * 2;
	if (widget_width < overlay_width) {
		overlay_width = base_width;
		overlay_height = base_height;
	}
*/
/*
	// algo 4 : best smallest : too pixellized upon fullscreen 640x480
	overlay_width = base_width;
	overlay_height = base_height;
*/

	// algo 5 : when it is necessary to resample, resample x 2 or
	overlay_width = 640;
	overlay_height = 480;
	if (widget_width < overlay_width) {
		overlay_width = 352;
		overlay_height = 288;
	}


	overlay_width = overlay_width & ~1;
	overlay_height = overlay_height & ~1;

	incrust_width = (overlay_width / 5) & ~1;
	incrust_height = (overlay_height / 5) & ~1;

	incrust_x = (overlay_width - incrust_width - 10) & ~1;
	incrust_y = (overlay_height - incrust_height - 10) & ~1;

	// alloc and init all buffers and structures that are necessary for the new viewing size
	allocProcessingBuffers();

	mt.unlock();
}

void FrameProcessorThread::freeProcessingBuffers() {
	img_remote.reset();
	img_local.reset();

	avpicture_free(&avp_rgb_localcapture_resized);
	avpicture_free(&avp_yuv_localcapture_resized);
	avpicture_free(&avp_rgb_localcapture_unresized);

	avpicture_free(&avp_rgb_remoteframe_resized);
	avpicture_free(&avp_yuv_remoteframe_resized);
	avpicture_free(&avp_rgb_remoteframe_unresized);

	if (rz_context_remote != NULL) {
		img_resample_close(rz_context_remote);
	}

	if (rz_context_local != NULL) {
		img_resample_close(rz_context_local);
	}
}

void FrameProcessorThread::allocProcessingBuffers() {
	img_remote.create(overlay_width, overlay_height, 32);
	img_local.create(incrust_width, incrust_height, 32);

	avpicture_alloc(&avp_rgb_remoteframe_unresized, PIX_FMT_RGBA32, base_width,
		base_height);
	avpicture_alloc(&avp_yuv_remoteframe_resized, PIX_FMT_YUV420P, overlay_width,
		overlay_height);
	avpicture_alloc(&avp_rgb_remoteframe_resized, PIX_FMT_RGBA32, overlay_width,
		overlay_height);

	avpicture_alloc(&avp_rgb_localcapture_unresized, PIX_FMT_RGBA32, base_width,
		base_height);
	avpicture_alloc(&avp_yuv_localcapture_resized, PIX_FMT_YUV420P, incrust_width,
		incrust_height);
	avpicture_alloc(&avp_rgb_localcapture_resized, PIX_FMT_RGBA32, incrust_width,
		incrust_height);

	rz_context_remote = img_resample_init(
		overlay_width, overlay_height,
		base_width, base_height);

	rz_context_local = img_resample_init(
		incrust_width, incrust_height,
		base_width, base_height);
}
