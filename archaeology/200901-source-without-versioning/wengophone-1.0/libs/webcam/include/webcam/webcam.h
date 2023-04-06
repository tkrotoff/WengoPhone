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
 * @author David Ferlier (david.ferlier@wengo.fr)
 * @author Mathieu Stute (mathieu.stute@wengo.fr)
 *
 */

#ifndef WEBCAM_H
#define WEBCAM_H

#include <stdio.h>
#include <string.h>
#include <avcodec.h>

#define WEBCAM_CODE_NOK     0
#define WEBCAM_CODE_OK      1

#define WEBCAM_FLAG_OPENED       (1 << 0)
#define WEBCAM_FLAG_NOT_OPENED   (1 << 1)
#define WEBCAM_FLAG_HORIZONTAL_FLIP	(1 << 2)
#define WEBCAM_FLAG_USE_DEFAULT_DEVICE (1 << 3)

// enumaration of the "pix_osi" palette plane
// this is the palette plane managed by this library
enum WebcamPaletteIds {
	WEBCAM_PALETTE_UNSUPPORTED = -1,
	WEBCAM_PALETTE_YUV420P, // this value: 0 should not be modified. It is currently used as 0 in phmedia-video.c
	WEBCAM_PALETTE_I420,
	WEBCAM_PALETTE_YUV422,
	WEBCAM_PALETTE_YUV411,
	WEBCAM_PALETTE_YUV422P,
	WEBCAM_PALETTE_YUV444P,
	WEBCAM_PALETTE_YUV424P,
	WEBCAM_PALETTE_YUV41P,
	WEBCAM_PALETTE_YUY2,
	WEBCAM_PALETTE_YUYV,
	WEBCAM_PALETTE_YVYU,
	WEBCAM_PALETTE_UYVY,
	WEBCAM_PALETTE_YV12,
	WEBCAM_PALETTE_RGB555,
	WEBCAM_PALETTE_RGB565,
	WEBCAM_PALETTE_RGB1,
	WEBCAM_PALETTE_RGB4,
	WEBCAM_PALETTE_RGB8,
	WEBCAM_PALETTE_RGB32,
	WEBCAM_PALETTE_RGB24,
	WEBCAM_PALETTE_NV12

};

#ifdef WIN32
#ifdef WEBCAM_BUILDING_DLL
#define WEBCAM_DLLEXPORT __declspec(dllexport)
#else
#define WEBCAM_DLLEXPORT __declspec(dllimport)
#endif
#else
#define WEBCAM_DLLEXPORT
#endif

#ifdef __cplusplus

class WEBCAM_DLLEXPORT Webcam {
public:

    Webcam ();
    virtual ~Webcam();

    void setUserData(void *data) {
        userdata = data;
    }

    void * getUserData() {
        return userdata;
    }

    void setDevice(char *name) {
        strcpy(Device, name);
    }

    void setDeviceDefault() {
		setFlag(WEBCAM_FLAG_USE_DEFAULT_DEVICE);
    }

    char * getDevice() {
        return Device;
    }

    unsigned int getFormat() {
        return format;
    }

    void setFormat(int f) {
        format = f;
    }

    void setHorizontalFlip(int flip) {
	    if (flip) {
		    setFlag(WEBCAM_FLAG_HORIZONTAL_FLIP);
	    } else {
		    unsetFlag(WEBCAM_FLAG_HORIZONTAL_FLIP);
	    }
    }

    void setFlag(int flag) {
        flags |= flag;
    }

    void unsetFlag(int flag) {
	    flags &= ~flag;
    }

    int isFlagSet(int flag) {
	    return flags & flag;
    }

    int isOpened() {
        return isFlagSet(WEBCAM_FLAG_OPENED);
    }

    virtual int open() = 0;
    virtual void close() = 0;
	virtual void uninitialize() = 0;

    virtual void setBrightness(int);
    virtual void setContrast(int);
    virtual void setColour(int);
    virtual int setPalette(int);
	virtual int setCaps(int, int, int, int);
    virtual int setFps(int) = 0;
 
    virtual int setResolution(int width, int height);

    virtual int getCurrentWidth() const;
    virtual int getCurrentHeight() const;
    virtual int getPalette() const;
    int getFrameSize() const;

    void setLastFrame(uint8_t *buffer) {
        lastFrame = buffer;
    }

    uint8_t *getLastFrame() const {
        return lastFrame;
    }

    void setFpsTimerInter(int in) {
	    fpsTimerInter = in;
    }

    virtual void startCapture() = 0;

    void frameReady(uint8_t *buf, int size);
    void setFrameReadyCallback(int (*webcam_frame_callback)(void *userdata, uint8_t *buf_frameready, int width_frameready, int height_frameready, int pix_osi_frameready, int len_frameready)) {
        webcamCallbackFrameReady = webcam_frame_callback;
    }

    int (*webcamCallbackFrameReady)(void *userdata, uint8_t *buf_frameready, int width_frameready, int height_frameready, int pix_osi_frameready, int len_frameready);
    void frameCopyAvailable(uint8_t *buf_frame, int width_frame, int height_frame, int pix_osi_frame, int len_frame);
    void frameBufferAvailable(uint8_t *buf_frame, int width_frame, int height_frame, int pix_osi_frame, int len_frame);
    int calculateFrameSize(int palette, int width, int height);
    uint8_t *convertImage(uint8_t *buf, int width, int height, int pix_osi_source, int pix_osi_target);

    uint8_t * lastFrame;

	/**
	* conversion from the osi palette plane to the fmt(avcodec) palette plane.
	*
	* an exception is done for NV12 that does not exist in avcodec
	*/
	int webcam_palette_to_avcodec_pixfmt(int format); 

private:

    int format;
    int opened;
    int flags;

    int fps;
    int fpsTimerLast;
    int fpsTimerInter;

    void *userdata;

    /*
     * A string specifying the device to use
     *
     */
    char Device[512];

	/**
	* @brief helper function to convert between NV12 and YUV420P
	*
	* cf http://www.fourcc.org/yuv.php#NV12	
	* some cameras support this palette format that is not known by ffmpeg
	*/
	uint8_t *nv12_to_yuv420p(uint8_t* buf, int width, int height);
};

/*
 * Define a WebcamOSI (Webcam OS Independent) type
 *
 */

#if defined(WIN32)
#include <webcam/WebcamDirectX.h>
typedef class WebcamDirectX WebcamOSI;
#elif defined(__linux__)
#include <webcam/WebcamV4l.h>
typedef class WebcamVideo4Linux WebcamOSI;
#else
#include <webcam/WebcamNull.h>
typedef class WebcamNull WebcamOSI;
#endif

#endif
#endif
