/*
 The phmedia-audio-null  module implements interface to a null audio driver
 Copyright (C) 2005  Philippe Bernery <philippe.bernery@wengo.fr>
 
 this module is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 eXosip is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

#define ph_printf printf


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
	ph_printf("** Register and initialize null audio driver\n");
	ph_register_audio_driver(&ph_null_audio_driver);
}

void null_start(phastream_t *as) {
	ph_printf("** Starting audio stream\n");
}

int null_open(phastream_t *as, const char *name, int rate, int framesize, ph_audio_cbk cbk) {
	ph_printf("** Opening device %s with rate: %d, framesize: %d, and callback: %p\n",
		  name, rate, framesize, cbk);
	
	return 0;
}

int null_write(phastream_t *as, void *buf, int len) {
	ph_printf("** Writing %d bytes of data from buffer %p\n",
		  len, buf);
	
	return 0;
}

int null_read(phastream_t *as, void *buf, int len) {
	ph_printf("** Reading %d bytes of data and putting it into buffer %p\n",
		  len, buf);
	
	return 0;
}

int null_get_out_space(phastream_t *as, int *used) {
	*used = 0;

	ph_printf("** Out space used: 0\n", *used);

	return 0;
}

int null_get_avail_data(phastream_t *as) {
	ph_printf("** Available data: 0\n");
	
	return 0;
}

void null_close(phastream_t *as) {
	ph_printf("** Closing audio stream\n");
}
