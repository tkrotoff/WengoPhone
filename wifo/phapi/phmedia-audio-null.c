/*
 * The phmedia-audio-null  module implements interface to a null audio driver
 *
 * Copyright (C) 2004  Vadim Lebedev  <vadim@mbdsys.com>
 * Copyright (C) 2006 WENGO SAS
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with dpkg; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <osip2/osip_mt.h>
#include <osip2/osip.h>
#ifndef T_MSVC
#include <sys/ioctl.h>
#include <sys/time.h>
#endif
#include <fcntl.h>
#include <stdlib.h>
#include <ortp.h>
#include <ortp-export.h>
#include <telephonyevents.h>
#include "phapi.h"
#include "phcall.h"
#include "phmedia.h"
#include "phcodec.h"
#include "tonegen.h"
#include "phmbuf.h"

#include "phmstream.h"
#include "phastream.h"
#include "phaudiodriver.h"
#include "phlog.h"

#define PH_UNREFERENCED_PARAMETER(P) (P)

/**
 * Declare the driver to phmedia-audio and initialize it.
 */
void ph_null_driver_init();

/**
 * Start the stream.
 *
 * @return
 */
void null_start(phastream_t *as);

/**
 * Open a device.
 *
 * @param name desired device name
 * @param rate desired audio rate
 * @param framesize
 * @param cbk
 * @return
 */
int null_open(phastream_t *as, const char *name, int rate, int framesize, ph_audio_cbk cbk);

/**
 * Write sound data onto the device.
 *
 * @param buf data buffer
 * @param len length of data buffer
 */
int null_write(phastream_t *as, void *buf, int len);

/**
 * Read sound data from the device.
 *
 * @param buf data buffer
 * @param len length of data buffer
 */
int null_read(phastream_t *as, void *buf, int len);

/**
 * Get out data size.
 *
 * @param used
 */
int null_get_out_space(phastream_t *as, int *used);

/**
 * Get available data.
 */
int null_get_avail_data(phastream_t *as);

/**
 * Close the stream.
 */
void null_close(phastream_t *as);

struct ph_audio_driver ph_null_audio_driver = {
  "null",
  0,
  0,
  null_start,
  null_open,
  null_write,
  null_read,
  null_get_out_space,
  null_get_avail_data,
  null_close
};

void ph_null_driver_init() {
  DBG_DYNA_AUDIO_DRV("** Register and initialize null audio driver\n");
  ph_register_audio_driver(&ph_null_audio_driver);
}

void null_start(phastream_t *as) {
  PH_UNREFERENCED_PARAMETER(as);
  DBG_DYNA_AUDIO_DRV("** Starting audio stream\n");
}

int null_open(phastream_t *as, const char *name, int rate, int framesize, ph_audio_cbk cbk) {
  PH_UNREFERENCED_PARAMETER(as);
  DBG_DYNA_AUDIO_DRV("** Opening device %s with rate: %d, framesize: %d, and callback: %p\n",
    name, rate, framesize, cbk);
  return 0;
}

int null_write(phastream_t *as, void *buf, int len) {
  PH_UNREFERENCED_PARAMETER(as);
  DBG_DYNA_AUDIO_DRV("** Writing %d bytes of data from buffer %p\n", len, buf);
  return 0;
}

int null_read(phastream_t *as, void *buf, int len) {
  PH_UNREFERENCED_PARAMETER(as);
  DBG_DYNA_AUDIO_DRV("** Reading %d bytes of data and putting it into buffer %p\n", len, buf);
  return 0;
}

int null_get_out_space(phastream_t *as, int *used) {
  *used = 0;
  PH_UNREFERENCED_PARAMETER(as);
  DBG_DYNA_AUDIO_DRV("** Out space used: 0\n", *used);
  return 0;
}

int null_get_avail_data(phastream_t *as) {
  PH_UNREFERENCED_PARAMETER(as);
  DBG_DYNA_AUDIO_DRV("** Available data: 0\n");
  return 0;
}

void null_close(phastream_t *as) {
  PH_UNREFERENCED_PARAMETER(as);
  DBG_DYNA_AUDIO_DRV("** Closing audio stream\n");
}
