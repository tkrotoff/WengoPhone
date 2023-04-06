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
 * @file webcam_c.h
 * Portable webcam driver by David Ferlier (david.ferlier@wengo.fr)
 *
 * Some parts were taken from the mythphone webcam driver, mostly the v4l part
 *
 */

#ifndef WEBCAM_C_H
#define WEBCAM_C_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#ifdef __cplusplus
extern "C"
{
#endif

struct webcam;

struct webcam * webcamInitialize();
void webcamUninitialize(struct webcam *);
void webcamSetCallback(struct webcam *, int (*callback)(void *, uint8_t *, int, int, int, int));
void webcamSetUserData(struct webcam *, void *);
void webcamStartCapture(struct webcam *);
uint8_t *webcamGetLastFrame(struct webcam *);
int webcamGetCurrentWidth(struct webcam *);
int webcamGetCurrentHeight(struct webcam *);
int webcamOpen(struct webcam *);
int webcamIsOpened(struct webcam *);
void webcamClose(struct webcam *);
void webcamClose(struct webcam *);
void webcamSetFps(struct webcam *, unsigned int);
int webcamSetResolution(struct webcam *, int width, int height);
int webcamGetPalette(struct webcam *);
int webcamSetPalette(struct webcam *, int);
uint8_t *webcamConvertImage(struct webcam *, uint8_t *, int, int, int, int);
void webcamSetDevice(struct webcam *w, char *);
void webcamSetCameraFlip(struct webcam *w, int);
int webcamPixOsiToPixFmt(struct webcam *w, int pix_osi);


#ifdef __cplusplus
}
#endif

#endif
