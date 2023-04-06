/*
 * WengoPhone, a voice over Internet phone
 *
 * Copyright (C) 2004-2005  Wengo
 * Copyright (C) 2005 David Ferlier <david.ferlier@wengo.fr>
 *
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
 * @file webcam_vfw.cpp
 * Portable webcam driver by David Ferlier (david.ferlier@wengo.fr)
 *
 * VFW driver, for win32
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <windows.h>
#include <vfw.h>
#ifdef __GNUC__
#include "vfwgcc.h"
#endif

#include "yuvops.h"
#include <webcamdriver/webcam.h>



LRESULT CALLBACK frameReadyCallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr);
LRESULT CALLBACK ErrorCallbackProc(HWND hWnd, int nErrID, LPSTR lpErrorText);
LRESULT CALLBACK StatusCallbackProc(HWND hWnd, int nID, LPSTR lpStatusText);
LRESULT CALLBACK CapControlCallbackProc(HWND hWnd, int nState);
LRESULT CALLBACK YieldCallbackProc(HWND hWnd);

struct webcam * webcamInitialize(char *devname) {
	struct webcam * w;

	w = (struct webcam *) malloc (sizeof(struct webcam));
	w->devname = (char *) malloc(strlen(devname));

	w->hDev = 0;
	w->frameSize = 0;
	w->fps = WEBCAM_DEFAULT_FPS;
	w->wcFormat = 0;
	w->wcFlip = 0;
	w->frameReady = 0;
	w->lastFrame = 0;
	w->lastFrameTimer = 0.0;

	strcpy(w->devname, devname);

	return w;
}


void webcamSetCallback(struct webcam * w, int (*callback)(void *, int, uint8_t *)) {
	w->frameReady = callback;
}

char * webcamGetDeviceName(struct webcam *w) {
	return "";
}

int webcamOpen(struct webcam *w, int width, int height)
{
	int actWidth, actHeight;
    const unsigned long _1M = 1000000;
    
	w->hwndCap = capCreateCaptureWindow((LPSTR)"Capture Window",
            WS_POPUP | WS_CAPTION, CW_USEDEFAULT, CW_USEDEFAULT,
	    	width, height, w->hwndWebcam, (int)0);

	if (w->hwndCap == NULL) {
		printf("Can't create capture window\n");
		return -1;
	}

   	webcamSetFps(w, w->fps);

	capSetCallbackOnVideoStream(w->hwndCap, frameReadyCallbackProc);
	capSetCallbackOnError(w->hwndCap, ErrorCallbackProc) ;
	capSetCallbackOnStatus(w->hwndCap, StatusCallbackProc) ;
	capSetCallbackOnCapControl(w->hwndCap, CapControlCallbackProc) ;
	capSetCallbackOnYield(w->hwndCap, YieldCallbackProc);

	capDriverConnect(w->hwndCap, 0);
	capSetUserData(w->hwndCap, (long)w);

	capPreviewRate(w->hwndCap, w->interFrameTime);
	capPreviewScale(w->hwndCap, true);
	capPreview(w->hwndCap, true);

	capCaptureGetSetup(w->hwndCap, &w->capParams, sizeof(w->capParams));
	w->capParams.fYield = true;
	w->capParams.fAbortLeftMouse = false;
	w->capParams.fAbortRightMouse = false;
	w->capParams.fLimitEnabled = false;
	w->capParams.fCaptureAudio = false;
    w->capParams.dwRequestMicroSecPerFrame = _1M / w->fps;
	capCaptureSetSetup(w->hwndCap, &w->capParams, sizeof(w->capParams));
	capCaptureSequenceNoFile(w->hwndCap);

	webcamReadCaps(w);

	if (!webcamSetPalette(w, VIDEO_PALETTE_YUV420P) &&
	    !webcamSetPalette(w, VIDEO_PALETTE_YUV422P) &&
	    !webcamSetPalette(w, VIDEO_PALETTE_RGB24)) {
		printf("Webcam Not supported !!\n");
	}

	w->frameCount = 0;

	webcamSetSize(w, width, height);
	webcamGetCurSize(w, &actWidth, &actHeight);

#ifdef DEBUG
	printf("Webcam resolution %d:%d\n", actWidth, actHeight);
	printf("Webcam palette %d\n", webcamGetPalette(w));
#endif

	if (webcamIsGreyscale(w))
	{
	    webcamClose(w);
	    return 0;
	}
	else 
	{
	    switch (webcamGetPalette(w))
	    {
	    case VIDEO_PALETTE_RGB24:   w->frameSize = RGB24_LEN(w->WCWIDTH, w->WCHEIGHT);   break;
	    case VIDEO_PALETTE_RGB32:   w->frameSize = RGB32_LEN(w->WCWIDTH, w->WCHEIGHT);   break;
	    case VIDEO_PALETTE_YUV420P: w->frameSize = YUV420P_LEN(w->WCWIDTH, w->WCHEIGHT); break;
	    case VIDEO_PALETTE_YUV422P: w->frameSize = YUV422P_LEN(w->WCWIDTH, w->WCHEIGHT); break;
	    default:
		webcamClose(w);
		return 0;
		break;
	    }
	}

	w->lastFrame = (uint8_t *) webcamBufAlloc(w->frameSize * sizeof(uint8_t));

	switch(webcamGetPalette(w))
	{
		case VIDEO_PALETTE_YUV420P:    w->wcFormat = PIX_FMT_YUV420P;    break;
		case VIDEO_PALETTE_YUV422P:    w->wcFormat = PIX_FMT_YUV422P;    break;
		case VIDEO_PALETTE_RGB24:      w->wcFormat = PIX_FMT_BGR24;      break;
		case VIDEO_PALETTE_RGB32:      w->wcFormat = PIX_FMT_RGBA32;     break;
	default:
	    webcamClose(w);
	    return 0;
	    break;
	}

	return 1;
}


void webcamClose(struct webcam *w)
{
    __webcam_common_close(w);

	capSetUserData(w->hwndCap, (long) 0);

	capCaptureStop(w->hwndCap);
	capCaptureAbort(w->hwndCap);
	capDriverDisconnect(w->hwndCap);

	capSetCallbackOnVideoStream(w->hwndCap, NULL);
	capSetCallbackOnWaveStream(w->hwndCap, NULL) ;
	capSetCallbackOnError(w->hwndCap, NULL) ;
	capSetCallbackOnStatus(w->hwndCap, NULL) ;
	capSetCallbackOnFrame(w->hwndCap, NULL) ;
	capSetCallbackOnCapControl(w->hwndCap, NULL) ;

	DestroyWindow(w->hwndCap);
}


void webcamReadCaps(struct webcam *w)
{
    capCaptureGetSetup(w->hwndCap, &w->capParams, sizeof(w->capParams));
    capGetVideoFormat(w->hwndCap, &w->bitmapInfo, sizeof(w->bitmapInfo));
}


void webcamSetSize(struct webcam *w, int width, int height)
{
    w->bitmapInfo.bmiHeader.biHeight = height;
    w->bitmapInfo.bmiHeader.biWidth = width;
    capSetVideoFormat(w->hwndCap, &w->bitmapInfo, sizeof(w->bitmapInfo));
    webcamReadCaps(w);
}

int webcamSetPalette(struct webcam *w, unsigned int palette)
{
	int depth;
	int winPalette = 0;

	switch(palette) {
		case VIDEO_PALETTE_YUV420P: depth = 12;  break;
		case VIDEO_PALETTE_YUV422:  depth = 16;  break;
		case VIDEO_PALETTE_YUV422P: depth = 16;  break;
		case VIDEO_PALETTE_RGB32:   depth = 32;  break;
		case VIDEO_PALETTE_RGB24:   depth = 24;  break;
		default:                    depth = 0;   break;
	}


    switch(palette){
        default:
        case VIDEO_PALETTE_YUV420P:
                winPalette = (MAKEFOURCC('I', 'Y', 'U', 'V')); break;
        case VIDEO_PALETTE_YUV422:
                winPalette = (MAKEFOURCC('U', 'Y', 'V', 'Y')); break;
        case VIDEO_PALETTE_YUV422P:
                winPalette = (MAKEFOURCC('Y', 'V', '1', '6')); break;
        case VIDEO_PALETTE_RGB32:
                winPalette = 0; break;
        case VIDEO_PALETTE_RGB24:
                winPalette = 0; break;
    }
    w->bitmapInfo.bmiHeader.biCompression = winPalette;
    w->bitmapInfo.bmiHeader.biBitCount = depth;
    capSetVideoFormat(w->hwndCap, &w->bitmapInfo, sizeof(w->bitmapInfo));

    webcamReadCaps(w);
    return ((w->bitmapInfo.bmiHeader.biCompression == winPalette) && (w->bitmapInfo.bmiHeader.biBitCount == depth) ? 1 : 0);
}


unsigned int webcamGetPalette(struct webcam *w) {
    int winPalette = 0;
    switch(w->bitmapInfo.bmiHeader.biCompression)
    {
        case MAKEFOURCC('I', 'Y', 'U', 'V'):    return VIDEO_PALETTE_YUV420P;
        case MAKEFOURCC('U', 'Y', 'V', 'Y'):    return VIDEO_PALETTE_YUV422;
        case MAKEFOURCC('Y', 'V', '1', '6'):    return VIDEO_PALETTE_YUV422P;
        default:                    
        case 0:
            if (w->bitmapInfo.bmiHeader.biBitCount == 24)
                return VIDEO_PALETTE_RGB24;
            else if (w->bitmapInfo.bmiHeader.biBitCount == 32)
                return VIDEO_PALETTE_RGB32;
    }
    return 0;
}

int webcamSetBrightness(struct webcam *w, int v)
{
	return 0;
}

int webcamSetContrast(struct webcam *w, int v)
{
	return 0;
}


int webcamSetColour(struct webcam *w, int v)
{
	return 0;
}

int webcamSetHue(struct webcam *w, int v)
{
	return 0;
}

void webcamGetMaxSize(struct webcam *w, int *x, int *y)
{
    *y=w->bitmapInfo.bmiHeader.biHeight; *x=w->bitmapInfo.bmiHeader.biWidth;

}

void webcamGetMinSize(struct webcam *w, int *x, int *y)
{
    *y=w->bitmapInfo.bmiHeader.biHeight; *x=w->bitmapInfo.bmiHeader.biWidth;
}

void webcamGetCurSize(struct webcam *w, int *x, int *y)
{
    *y = w->WCHEIGHT; 
    *x = w->WCWIDTH;
}

int webcamIsGreyscale(struct webcam *w) {
	return 0;
}


LRESULT CALLBACK frameReadyCallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	struct webcam *w = (struct webcam *)capGetUserData(hWnd);
    unsigned long now;
    
#ifdef DEBUG
	printf("Captured a frame from the webcam\n");
#endif

    
	if (!w || w->frameReady == NULL)
		return (LRESULT) false;

	__webcam_process_frame(w, lpVHdr->lpData, lpVHdr->dwBytesUsed);
	return (LRESULT) true;
}

LRESULT CALLBACK ErrorCallbackProc(HWND hWnd, int nErrID, LPSTR lpErrorText)
{
	struct webcam *w = (struct webcam *)capGetUserData(hWnd);

	printf("ErrorCallbackProc %d : %s\n", nErrID, lpErrorText);

	if (!w || w->frameReady == NULL)
		return (LRESULT) false;

	return (LRESULT) true;
}

LRESULT CALLBACK StatusCallbackProc(HWND hWnd, int nID, LPSTR lpStatusText)
{
#ifdef DEBUG
	printf("StatusCallbackProc %d : %s\n", nID, lpStatusText);
#endif
	return (LRESULT) true;
}


LRESULT CALLBACK CapControlCallbackProc(HWND hWnd, int nState)
{
	struct webcam *w = (struct webcam *)capGetUserData(hWnd);
#ifdef DEBUG
	printf("CapControlCallbackProc %d\n", nState);
#endif

	if (nState == CONTROLCALLBACK_PREROLL)
		return (LRESULT) true;

	if (nState == CONTROLCALLBACK_CAPTURING) {
		if (!w || w->frameReady == NULL) {
			return (LRESULT) false;
		} else {
			return (LRESULT) true;
		}
	}

	return (LRESULT) false;
}

LRESULT CALLBACK YieldCallbackProc(HWND hWnd) {
	struct webcam *w = (struct webcam *)capGetUserData(hWnd);
	if (!w || w->frameReady == NULL)
		return (LRESULT) false;
	return (LRESULT) true;
}

