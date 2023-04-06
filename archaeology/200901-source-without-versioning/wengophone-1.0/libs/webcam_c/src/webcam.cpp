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
 * @file webcam.h
 * @author David Ferlier
 *
 */

#include <global.h>
#include <webcam_c/webcam_c.h>
#include <stdio.h>

#include <webcam/webcam.h>
#if defined(OS_WIN32)
#include <webcam/WebcamDirectX.h>
#elif defined(OS_LINUX)
#include <webcam/WebcamV4l.h>
#else
#include <webcam/WebcamNull.h>
#endif

struct webcam 
{
  Webcam *cam;
};


struct webcam * webcamInitialize() {
    struct webcam *w;

    w = (struct webcam *) malloc (sizeof(struct webcam));

#if defined(OS_WIN32)
    w->cam = new WebcamDirectX();
#elif defined(OS_LINUX)
    w->cam = new WebcamVideo4Linux();
#else
	w->cam = new WebcamNull();
#endif
    return w;
}

void webcamUninitialize(struct webcam *w) {
	w->cam->uninitialize();
    //delete w->cam;
    free(w);
}

void webcamSetCallback(struct webcam *w, int (*callback)(void *, uint8_t *, int, int, int, int)) {
    w->cam->setFrameReadyCallback(callback);
}

int webcamOpen(struct webcam *w) {
    return w->cam->open();
}


int webcamIsOpened(struct webcam *w) {
    return w->cam->isOpened();
}

void webcamClose(struct webcam *w) {
    w->cam->close();
}

int webcamSetPalette(struct webcam *w, int palette) {
  w->cam->setPalette(palette);
  return 0;
}

int webcamSetFps(struct webcam *w, int fps) {
    return 0;
}

int webcamGetPalette(struct webcam *w) {
    return w->cam->getPalette();
}

uint8_t *webcamConvertImage(struct webcam *w, uint8_t *frame, int width, int height, int pix_osi_source, int pix_osi_target) {
    return w->cam->convertImage(frame, width, height, pix_osi_source, pix_osi_target);
}

void webcamStartCapture(struct webcam *w) {
    w->cam->startCapture();
}

void webcamSetUserData(struct webcam *w, void *data) {
    w->cam->setUserData(data);
}

int webcamGetCurrentWidth(struct webcam *w) {
    return w->cam->getCurrentWidth();
}

int webcamGetCurrentHeight(struct webcam *w) {
    return w->cam->getCurrentHeight();
}

uint8_t *webcamGetLastFrame(struct webcam *w) {
    return w->cam->getLastFrame();
}

void webcamSetFps(struct webcam *w, unsigned int fps) {
    w->cam->setCaps(WEBCAM_PALETTE_UNSUPPORTED, fps, 0, 0);
}

int webcamSetResolution(struct webcam *w, int width, int height) {
    return w->cam->setCaps(WEBCAM_PALETTE_UNSUPPORTED, 0, width, height);
}

void webcamSetDevice(struct webcam *w, char *device) {
    w->cam->setDevice(device);
}

void webcamSetCameraFlip(struct webcam *w, int flip) {
	w->cam->setHorizontalFlip(flip);
}

int webcamPixOsiToPixFmt(struct webcam *w, int pix_osi) {
	return w->cam->webcam_palette_to_avcodec_pixfmt(pix_osi);
}