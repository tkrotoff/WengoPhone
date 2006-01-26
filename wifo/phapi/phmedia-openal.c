/*
 The phmedia-openal  module implements interface for a OpenAL audio driver
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

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#define ph_printf printf

/* Public functions */
void ph_openal_driver_init();
void openal_start(phastream_t *as);
int openal_open(phastream_t *as, char *name, int rate, int framesize, ph_audio_cbk cbk);
int openal_write(phastream_t *as, void *buf, int len);
int openal_read(phastream_t *as, void *buf, int len);
int openal_get_out_space(phastream_t *as, int *used);
int openal_get_avail_data(phastream_t *as);
void openal_close(phastream_t *as);

/* Private functions */
static void openal_output_initialize(const ALCbyte *deviceSpecifier);
static void openal_input_initialize(const ALCbyte *deviceSpecifier);
static void openal_teardown();

struct ph_audio_driver ph_openal_audio_driver = {
	"openal",
	0,
	0,
	openal_start,
	openal_open,
	openal_write,
	openal_read,
	openal_get_out_space,
	openal_get_avail_data,
	openal_close
};


#define NUM_BUFFERS_SOURCES 1
static ALuint gBuffer[NUM_BUFFERS_SOURCES];
static ALuint gSource[NUM_BUFFERS_SOURCES];

static ALCdevice *captureDevice = NULL;

/**
 * Initialize OpenAL output and create appropriate buffers.
 */
static void 
openal_output_initialize(const ALCbyte *deviceSpecifier) {
	ALenum error;
	ALCcontext *newContext = NULL;
	ALCdevice *newDevice = NULL;
	ALCint flags[] = {
		ALC_FREQUENCY, 8000
	};
	
	newDevice = alcOpenDevice(deviceSpecifier);
	if (newDevice != NULL) {
		newContext = alcCreateContext(newDevice, flags);
		if (newContext != NULL) {
			alcMakeContextCurrent(newContext);
			
			// Creating bufffer objects
			alGenBuffers(NUM_BUFFERS_SOURCES, gBuffer);
			if ((error = alGetError()) != AL_NO_ERROR) {
				ph_printf("!!OpenAL: can't create buffer objects\n");
			}
			
			// Creating source objects
			alGenSources(NUM_BUFFERS_SOURCES, gSource);
			if ((error = alGetError()) != AL_NO_ERROR) {
				ph_printf("!!OpenAL: can't create source objects\n");
			}
			
		} else {
			ph_printf("!!OpenAL: can't create context\n");
		}
	} else {
		ph_printf("!!OpenAL: can't open default ouptut device\n");
	}
	
	//FIXME: remove this part
	atexit(openal_teardown);
}


static void
openal_input_initialize(const ALCbyte * deviceSpecifier) {	
	captureDevice = alcCaptureOpenDevice(deviceSpecifier, 8000, AL_FORMAT_MONO8, 0);
	if (!captureDevice) {
		ph_printf("!!OpenAL: can't create a capture device\n");
	}
}


static void 
openal_teardown() {
	ALCcontext *context = NULL;
	ALCdevice *device = NULL;
	ALuint returnedNames[NUM_BUFFERS_SOURCES];
	
	// Delete the Sources
	alDeleteSources(NUM_BUFFERS_SOURCES, returnedNames);
	// Delete the Buffers
	alDeleteBuffers(NUM_BUFFERS_SOURCES, returnedNames);
	
	//Get active context
	context = alcGetCurrentContext();
	//Get device for active context
	device = alcGetContextsDevice(context);
	//Release context
	alcDestroyContext(context);
	//Close device
	alcCloseDevice(device);
	
	//Close capture device
	if (captureDevice) {
		if (!alcCaptureCloseDevice(captureDevice)) {
			ph_print("!!OpenAL: can't uninitialize capture device\n");
		}
	}
}

void ph_openal_driver_init() {
	openal_output_initialize(NULL);
	openal_input_initialize(NULL);
	
	ph_register_audio_driver(&ph_openal_audio_driver);
}

void openal_start(phastream_t *as) {
	ph_printf("** Starting audio stream\n");
}

int openal_open(phastream_t *as, char *name, int rate, int framesize, ph_audio_cbk cbk) {
	ph_printf("** Opening device %s with rate: %d, framesize: %d, and callback: %p\n",
		  name, rate, framesize, cbk);
	
	return 0;
}

int openal_write(phastream_t *as, void *buf, int len) {
	ph_printf("** Writing %d bytes of data from buffer %p\n",
		  len, buf);
	
	return 0;
}

int openal_read(phastream_t *as, void *buf, int len) {
	ph_printf("** Reading %d bytes of data and putting it into buffer %p\n",
		  len, buf);
	
	return 0;
}

int openal_get_out_space(phastream_t *as, int *used) {
	*used = 0;

	ph_printf("** Out space used: 0\n", *used);

	return 0;
}

int openal_get_avail_data(phastream_t *as) {
	ph_printf("** Available data: 0\n");
	
	return 0;
}

void openal_close(phastream_t *as) {
	ph_printf("** Closing audio stream\n");
}
