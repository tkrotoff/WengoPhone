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
 * Video 4 Linux implementation of webcam.h
 *
 * @author David Ferlier
 *
 */

#include <webcam/webcam.h>
#include <webcam/WebcamV4l.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>

#include <wtimer.h>

void *frameReadThread(void *arg) {
    WebcamVideo4Linux *cam = (WebcamVideo4Linux *) arg;
    int len, fsize;
    uint8_t *picbuff;

    fsize = cam->getFrameSize();
    picbuff = (uint8_t *) av_malloc (fsize * sizeof(uint8_t));

    while (cam->isOpened()) {
        usleep(200 * 1000);
        len = read(cam->f_handle, picbuff, fsize);

        if (len >= fsize) {
            if (!cam->isOpened()) {
                break;
            }
            cam->frameBufferAvailable(picbuff,
                cam->getCurrentWidth(),
                cam->getCurrentHeight(),
                cam->getPalette(), fsize);
        }
    }

	::close(cam->f_handle);
}

void v4l_timer_callback(void *userdata) {
    WebcamVideo4Linux *cam = (WebcamVideo4Linux *) userdata;
    int len, fsize;
    uint8_t *picbuff;

    fsize = cam->getFrameSize();
    picbuff = (uint8_t *) av_malloc (fsize * sizeof(uint8_t));

    len = read(cam->f_handle, picbuff, fsize);

    if (len >= fsize) {
        cam->frameBufferAvailable(picbuff,
            cam->getCurrentWidth(),
            cam->getCurrentHeight(),
            cam->getPalette(), fsize);
    }
    av_free(picbuff);
}

WebcamVideo4Linux::WebcamVideo4Linux() {
    setDevice("/dev/video0");
}

int WebcamVideo4Linux::open() {
    if (isOpened()) {
        return 0;
    }

    f_handle = ::open(getDevice(), O_RDWR);
    if (f_handle <= 0) {
        return 0;
    }
    fcntl(f_handle, O_NONBLOCK);
    setFlag(WEBCAM_FLAG_OPENED);
    readCaps();

    return WEBCAM_CODE_OK;
}

void WebcamVideo4Linux::close() {
    unsetFlag(WEBCAM_FLAG_OPENED);
#ifdef WEBCAM_V4L_USE_TIMERS
    t_impl->timer_stop(timer);
#else
    //pthread_join(read_thread, NULL);
#endif
	::close(f_handle);
}

void WebcamVideo4Linux::uninitialize() {
	close();
}

void WebcamVideo4Linux::startCapture () {
#ifdef WEBCAM_V4L_USE_TIMERS
    t_impl = timer_impl_getfirst();
    timer = t_impl->timer_create();
    t_impl->timer_set_delay(timer, 40);
    t_impl->timer_set_callback(timer, v4l_timer_callback);
    t_impl->timer_set_userdata(timer, this);
    t_impl->timer_start(timer);
#else
    pthread_create(&read_thread, NULL, frameReadThread, (void *) this);
#endif
}

int WebcamVideo4Linux::setPalette(int palette) {
    int depth;
    int v4l_palette = -1;

	switch(palette) {
		case WEBCAM_PALETTE_YUV420P:
            depth = 12;
            v4l_palette = VIDEO_PALETTE_YUV420P;
            break;

		case WEBCAM_PALETTE_YUV422:
            depth = 16;
            v4l_palette = VIDEO_PALETTE_YUV422;
            break;

		case WEBCAM_PALETTE_YUV422P:
            depth = 16;
            v4l_palette = VIDEO_PALETTE_YUV422P;
            break;

		case WEBCAM_PALETTE_RGB32:
            depth = 32;
            v4l_palette = VIDEO_PALETTE_RGB32;
            break;

		case WEBCAM_PALETTE_RGB24:
            depth = 24;
            v4l_palette = VIDEO_PALETTE_RGB24;
            break;

		default:
            depth = 0;
            break;
	}

	vPic.palette = v4l_palette;
	vPic.depth = depth;

	ioctl(f_handle, VIDIOCSPICT, &vPic);
    readCaps();

	return (vPic.palette == v4l_palette ? WEBCAM_CODE_OK : WEBCAM_CODE_NOK);
}

int WebcamVideo4Linux::getPalette() const {
	int w_palette;

	switch(vPic.palette) {
		case VIDEO_PALETTE_YUV420P:
			w_palette = WEBCAM_PALETTE_YUV420P;
			break;

		case VIDEO_PALETTE_YUV422:
			w_palette = WEBCAM_PALETTE_YUV422;
			break;

		case VIDEO_PALETTE_YUV422P:
			w_palette = WEBCAM_PALETTE_YUV422P;
			break;

		case VIDEO_PALETTE_RGB32:
			w_palette = WEBCAM_PALETTE_RGB32;
			break;

		case VIDEO_PALETTE_RGB24:
			w_palette = WEBCAM_PALETTE_RGB24;
			break;
	}

    return (w_palette);
}

void WebcamVideo4Linux::readCaps() {
    if (isOpened()) {
        ioctl(f_handle, VIDIOCGCAP, &vCaps);
        ioctl(f_handle, VIDIOCGWIN, &vWin);
        ioctl(f_handle, VIDIOCGPICT, &vPic);
    }
}

int WebcamVideo4Linux::setResolution(int width, int height) {
    memset(&vWin, 0, sizeof(struct video_window));
    vWin.width = width;
    vWin.height = height;

    if (ioctl(f_handle, VIDIOCSWIN, &vWin) == -1) {
        return WEBCAM_CODE_NOK;
    }

    readCaps();
}

int WebcamVideo4Linux::getCurrentWidth() const {
    return vWin.width;
}

int WebcamVideo4Linux::getCurrentHeight() const {
    return vWin.height;
}

int WebcamVideo4Linux::getFrameSize() {
	return calculateFrameSize(getPalette(),
                              getCurrentWidth(),
                              getCurrentHeight());
}


int WebcamVideo4Linux::setFps(int frames_per_second) {
	return WEBCAM_CODE_OK;
}
