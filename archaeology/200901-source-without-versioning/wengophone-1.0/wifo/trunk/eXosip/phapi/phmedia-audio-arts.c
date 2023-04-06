/*
 This module is free software; you can redistribute it and/or modify
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

#include <artsc.h>

/*
 * @author David Ferlier <david.ferlier@wengo.fr>
 *
 * arts driver for phapi
 *
 */

/**
 * Declare the driver to phmedia-audio and initialize it.
 */
void ph_ph_audio_arts_driver_init();

/**
 * Start the stream.
 *
 * @return 
 */
void ph_audio_arts_start(phastream_t *as);

/**
 * Open a device.
 *
 * @param name desired device name
 * @param rate desired audio rate
 * @param framesize 
 * @param cbk 
 * @return 
 */
int ph_audio_arts_open(phastream_t *as, const char *name, int rate, int framesize, ph_audio_cbk cbk);

/**
 * Write sound data onto the device.
 *
 * @param buf data buffer
 * @param len length of data buffer
 */
int ph_audio_arts_write(phastream_t *as, void *buf, int len);

/**
 * Read sound data from the device.
 *
 * @param buf data buffer
 * @param len length of data buffer
 */
int ph_audio_arts_read(phastream_t *as, void *buf, int len);

/**
 * Get out data size.
 *
 * @param used
 */
int ph_audio_arts_get_out_space(phastream_t *as, int *used);

/**
 * Get available data.
 */
int ph_audio_arts_get_avail_data(phastream_t *as);

/**
 * Close the stream.
 */
void ph_audio_arts_close(phastream_t *as);

struct arts_priv_data {
    arts_stream_t input_stream;
    arts_stream_t output_stream;
};

struct ph_audio_driver ph_audio_arts_driver = {
	"arts",
	0,
	0,
	ph_audio_arts_start,
	ph_audio_arts_open,
	ph_audio_arts_write,
	ph_audio_arts_read,
	ph_audio_arts_get_out_space,
	ph_audio_arts_get_avail_data,
	ph_audio_arts_close
};


void ph_audio_arts_driver_init() {
    printf("Registering arts audio driver\n");
	ph_register_audio_driver(&ph_audio_arts_driver);
}

void ph_audio_arts_start(phastream_t *as) {
    printf("Registering arts audio driver\n");
}

int ph_audio_arts_open(phastream_t *as, const char *name, int rate, int framesize, ph_audio_cbk cbk) {
    struct arts_priv_data *ad;
    int errcode;

    printf("ph_audio_arts_open\n");

    ad = (struct arts_priv_data *) malloc (sizeof(struct arts_priv_data));
    arts_init();

    ad->input_stream = arts_record_stream(rate, 16, 2, "phapi-in");
    if (!ad->input_stream) {
        printf("Can't open record stream\n");
        return -PH_NORESOURCES;
    }

    ad->output_stream = arts_play_stream(rate, 16, 2, "phapi-out");
    if (!ad->output_stream) {
        printf("Can't open play stream\n");
        return -PH_NORESOURCES;
    }

    arts_stream_set(ad->input_stream,  ARTS_P_BLOCKING, 0);
    arts_stream_set(ad->output_stream, ARTS_P_BLOCKING, 0);

    as->drvinfo = ad;
    return 0;
}

int ph_audio_arts_write(phastream_t *as, void *buf, int len) {
    struct arts_priv_data *ad = as->drvinfo;
    int write_r;

    if (ad->output_stream) {
        write_r = arts_write(ad->output_stream, buf, len);
        return write_r;
    }

    return 0;
}

int ph_audio_arts_read(phastream_t *as, void *buf, int len) {
    struct arts_priv_data *ad = as->drvinfo;
    int read_r;

    if (ad->input_stream) {
        read_r = arts_read(ad->input_stream, buf, len);
        return read_r;
    }

    return 0;
}

int ph_audio_arts_get_out_space(phastream_t *as, int *used) {
    struct arts_priv_data *ad = as->drvinfo;
    int space;

    space = arts_stream_get(ad->output_stream, ARTS_P_BUFFER_SPACE);
	*used = 2 * space;
	return 2 * space;
}

int ph_audio_arts_get_avail_data(phastream_t *as) {
	return 0;
}

void ph_audio_arts_close(phastream_t *as) {
    struct arts_priv_data *ad = as->drvinfo;
    arts_close_stream(ad->input_stream);
    arts_close_stream(ad->output_stream);
}
