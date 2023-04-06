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
 * @file webcam_v4l.c
 * Portable webcam driver by David Ferlier (david.ferlier@wengo.fr)
 *
 * V4L driver, for linux
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/videodev.h>

#include <webcam/webcam.h>

struct webcam * webcamInitialize(char *devname) {
	struct webcam * w;

	w = (struct webcam *) malloc (sizeof(struct webcam));
    memset(w, 0, sizeof(struct webcam));
	w->devname = (char *) malloc(strlen(devname));

	w->hDev = 0;
	w->frameSize = 0;
	w->fps = 5;
	w->wcFormat = 0;
	w->wcFlip = 0;

	w->vCaps.name[0] = 0;
	w->vCaps.maxwidth = 0;
	w->vCaps.maxheight = 0;
	w->vCaps.minwidth = 0;
	w->vCaps.minheight = 0;
	w->vWin.width = 0;
	w->vWin.height = 0;
	w->vPic.brightness = 0;
	w->vPic.depth = 0;
	w->vPic.palette = 0;
	w->vPic.colour = 0;
	w->vPic.contrast = 0;
	w->vPic.hue = 0;

	strcpy(w->devname, "/dev/video0");
	return w;
}

char * webcamGetDeviceName(struct webcam *w) {

	struct video_capability tempCaps;
	int handle;
	
	handle = open(w->devname, O_RDWR);
	if (handle <= 0)
		return NULL;

	ioctl(handle, VIDIOCGCAP, &tempCaps);
	close(handle);
	return tempCaps.name;
}

void __webcam_read_thread(void *arg) {
    struct webcam *w = (struct webcam *) arg;
    uint8_t *picbuff;
    int len;

    picbuff = (uint8_t *) malloc(w->frameSize * sizeof(uint8_t));

    while (w->hDev) {
        len = read (w->hDev, picbuff, w->frameSize);
        if (len >= w->frameSize && w->frameReady) {
			__webcam_process_frame(w, picbuff, w->frameSize);
        }
    }

	free(picbuff);
}

int webcamOpen(struct webcam *w, int width, int height)
{
	int opened = 1;
	int actWidth, actHeight;

	w->hDev = open(w->devname, O_RDWR);

	if (w->hDev <= 0) {
		return -1;
	}

	webcamReadCaps(w);

	if (!webcamSetPalette(w, VIDEO_PALETTE_YUV420P) && 
	    !webcamSetPalette(w, VIDEO_PALETTE_YUV422P) &&
	    !webcamSetPalette(w, VIDEO_PALETTE_RGB24)) {
		return 0;
	}

	w->frameCount = 0;

	webcamSetSize(w, width, height);
	webcamGetCurSize(w, &actWidth, &actHeight);

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
	w->lastFrame = (uint8_t *) av_malloc (w->frameSize * sizeof(uint8_t));

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

    pthread_create(&w->webcam_thread, NULL, __webcam_read_thread, (void *) w);
	return 1;
}


void webcamClose(struct webcam *w)
{
    if (w->hDev <= 0)
	    return -1;
    else
    {
	close(w->hDev);
	w->hDev = 0;
    }

    __webcam_common_close(w);
}


void webcamReadCaps(struct webcam *w)
{
    if (w->hDev > 0)
    {
	ioctl(w->hDev, VIDIOCGCAP, &w->vCaps);
	ioctl(w->hDev, VIDIOCGWIN, &w->vWin);
	ioctl(w->hDev, VIDIOCGPICT, &w->vPic);
    }
}


void webcamSetSize(struct webcam *w, int width, int height)
{
    memset(&w->vWin, 0, sizeof(struct video_window));
    w->vWin.width = width;
    w->vWin.height = height;

    if (ioctl(w->hDev, VIDIOCSWIN, &w->vWin) == -1) {
	    printf("Error\n");
    }
    webcamReadCaps(w);
}


int webcamSetPalette(struct webcam *w, unsigned int palette)
{
	int depth;

	switch(palette) {
		case VIDEO_PALETTE_YUV420P: depth = 12;  break;
		case VIDEO_PALETTE_YUV422:  depth = 16;  break;
		case VIDEO_PALETTE_YUV422P: depth = 16;  break;
		case VIDEO_PALETTE_RGB32:   depth = 32;  break;
		case VIDEO_PALETTE_RGB24:   depth = 24;  break;
		default:                    depth = 0;   break;
	}

	w->vPic.palette = palette;
	w->vPic.depth = depth;
	ioctl(w->hDev, VIDIOCSPICT, &w->vPic);

	webcamReadCaps(w);
	return (w->vPic.palette == palette ? 1 : 0);
}


unsigned int webcamGetPalette(struct webcam *w) {
    return (w->vPic.palette);
}

int webcamSetBrightness(struct webcam *w, int v)
{
	if ((v >= 0) && (v <= 65535)) {
		if (w->hDev > 0) {
			w->vPic.brightness = v;

			if (ioctl(w->hDev, VIDIOCSPICT, &w->vPic) == -1)
			      printf("Error setting brightness\n");
			webcamReadCaps(w);
		}
	}

	return w->vPic.brightness;
}

int webcamSetContrast(struct webcam *w, int v)
{
  if ((v >= 0) && (v <= 65535))
  {
    if (w->hDev > 0)
    {
      w->vPic.contrast = v ;

      if (ioctl(w->hDev, VIDIOCSPICT, &w->vPic) == -1)
	      printf("Error\n");
      webcamReadCaps(w);
    }
  }
  return w->vPic.contrast;
}


int webcamSetColour(struct webcam *w, int v)
{
  if ((v >= 0) && (v <= 65535))
  {
    if (w->hDev > 0)
    {
      w->vPic.colour = v;

      if (ioctl(w->hDev, VIDIOCSPICT, &w->vPic) == -1)
	      printf("Error\n");

      webcamReadCaps(w);
    }
  }
  return w->vPic.colour;
}


int webcamSetHue(struct webcam *w, int v)
{
  if ((v >= 0) && (v <= 65535))
  {
    if (w->hDev > 0)
    {
      w->vPic.hue = v;

      if (ioctl(w->hDev, VIDIOCSPICT, &w->vPic) == -1)
	      printf("Error\n");

      webcamReadCaps(w);
    }
  }
  return w->vPic.hue;
}


void webcamGetMaxSize(struct webcam *w, int *x, int *y)
{
    *y=w->vCaps.maxheight; *x=w->vCaps.maxwidth; 
}

void webcamGetMinSize(struct webcam *w, int *x, int *y)
{
    *y=w->vCaps.minheight; *x=w->vCaps.minwidth; 
}

void webcamGetCurSize(struct webcam *w, int *x, int *y)
{
    *y = w->WCHEIGHT; 
    *x = w->WCWIDTH;
}

int webcamIsGreyscale(struct webcam *w) {
    return ((w->vCaps.type & VID_TYPE_MONOCHROME) ? 1 : 0); 
}

int webcamReadFrame(struct webcam *w) {
    return 0;
}
