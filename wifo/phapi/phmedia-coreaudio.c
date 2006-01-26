/*
 The phmedia-coreaudio  module implements interface to a coreaudio driver
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
#include <CoreAudio/CoreAudio.h>
#include <AudioToolbox/AudioConverter.h>
#include <AudioUnit/AudioUnit.h>

#include <osip2/osip_mt.h>
#include <osip2/osip.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ortp.h>
#include <ortp-export.h>
#include <telephonyevents.h>
#include <stdlib.h>
#include "phapi.h"
#include "phcall.h"
#include "phmedia.h"
#include "phcodec.h"
#include "tonegen.h"
#include "phmbuf.h"

#include "phmstream.h"
#include "phastream.h"
#include "phaudiodriver.h"

#include <strings.h>

#define ph_printf  printf
//#define ph_printf

/**
 * Declare the driver to phmedia-audio and initialize it.
 */
void ph_ca_driver_init();

/**
 * Start the stream.
 */
void ca_start(phastream_t *as);

/**
 * Open a device.
 *
 * @param name desired devices. eg: "ca:IN=1 OUT=257"
 * @param rate desired audio rate
 * @param framesize 
 * @param cbk 
 * @return 0 if no error
 */
int ca_open(phastream_t *as, char *name, int rate, int framesize, ph_audio_cbk cbk);

/**
 * Get out data size.
 *
 * @param used
 */
int ca_get_out_space(phastream_t *as, int *used);

/**
 * Get available data.
 */
int ca_get_avail_data(phastream_t *as);

/**
 * Close the stream.
 */
void ca_close(phastream_t *as);


typedef struct _ca_dev {
	AudioDeviceID inputID;
	AudioDeviceID outputID;
	AudioUnit outputAU;
	AudioConverterRef inputConverter;
	unsigned inputConverterBufferSize;
	ph_audio_cbk cbk;
	
	char tmpOutputBuffer[4096]; // FIXME: should be set dynamically
	unsigned tmpOutputCount;
	
	char tmpInputBuffer[32768]; // FIXME: should be set dynamically
	unsigned tmpInputCount;
	
	char *currentInputBuffer;
	
	char convertedInputBuffer[320]; // FIXME: should be set dynamically
	unsigned convertedInputCount;
	
	char *recordBuffer;
	unsigned recordBufferCount;
	
	unsigned sumDataSize;
} ca_dev;


/**
 * Initialize a audio unit.
 * @param au AudioUnit to set.
 */
static void init_audio_unit(AudioUnit *au);


/**
 * Set Audio device of an audio unit.
 *
 * @param au AudioUnit to set device
 * @param id device id
 */
static void set_audio_device(AudioUnit au, AudioDeviceID id);


/**
 * Set the render callback.
 *
 * @param au AudioUnit to set up
 * @param cbk Callback to use
 * @param data user data for callback
 */
static void set_render_callback(AudioUnit au, AURenderCallback cbk, void *data);


/**
 * Set the intput callback.
 *
 * @param id AudioDevice to set up
 * @param cbk Callback to use
 * @param data user data for callback
 */
static void set_input_callback(AudioDeviceID id, AudioDeviceIOProc cbk, void *data);


/**
 * Open and initialize input device.
 *
 * @param rate desired rate e.g. 8000 Hz
 * @param channels desired amount of channels e.g. 2 (stereo)
 * @param format data format e.g. 16 (16 bits)
 * @param framesize amount of data to send to phapi callback
 */
static void init_input_device(phastream_t *as, float rate, unsigned channels, unsigned format);


/**
 * Open and initialize output device.
 *
 * @param rate desired rate e.g. 8000 Hz
 * @param channels desired amount of channels e.g. 2 (stereo)
 * @param format data format e.g. 16 (16 bits)
 * @param framesize amount of data to send to phapi callback
 */
static void init_output_device(phastream_t *as, float rate, unsigned channels, unsigned format);


/**
 * Set format of data that will be played.
 *
 * @param au AudioUnit to set
 * @param rate desired rate e.g. 8000 Hz
 * @param channels desired amount of channels e.g. 2 (stereo)
 * @param format data format e.g. 16 (16 bits) 
 */
static void set_played_format(AudioUnit au, float rate, unsigned channels, unsigned format);


/**
 * Set format of data that will be recorded.
 *
 * @param rate desired rate e.g. 8000 Hz
 * @param channels desired amount of channels e.g. 2 (stereo)
 * @param format data format e.g. 16 (16 bits) 
 */
static void set_recorded_format(phastream_t *as, float rate, unsigned channels, unsigned format);


/**
 * Output callback.
 */
static OSStatus output_renderer(void *inRefCon,
	AudioUnitRenderActionFlags *ioActionFlags,
	const AudioTimeStamp *inTimeStamp,
	UInt32 inBusNumber, 
	UInt32 inNumberFrames, 
	AudioBufferList *ioData);


/**
 * Input callback.
 */
static OSStatus input_proc(AudioDeviceID device,
	const AudioTimeStamp *currentTime,
	const AudioBufferList *inputData,
	const AudioTimeStamp *inputTime,
	AudioBufferList *outputData,
	const AudioTimeStamp *outputTime,
	void *context);


/**
 * Set devices in cadev from name.
 *
 * @param cadev struct to fill in
 * @param name name of the device to find
 */
static void parse_device(ca_dev *cadev, const char *name);


/**
 * Procedure used by input audio converter to provide data.
 *
 * @see Apple documentation
 */
static OSStatus buffer_data_proc(AudioConverterRef inAudioConverter, UInt32 *ioDataSize, void **outData, void *context);



struct ph_audio_driver ph_ca_audio_driver = {
	"ca",
	PH_SNDDRVR_PLAY_CALLBACK | PH_SNDDRVR_REC_CALLBACK,
	0,
	ca_start,
	ca_open,
	0,
	0,
	ca_get_out_space,
	ca_get_avail_data,
	ca_close
};











static OSStatus output_renderer(void *inRefCon,
	AudioUnitRenderActionFlags *ioActionFlags,
	const AudioTimeStamp *inTimeStamp,
	UInt32 inBusNumber,
	UInt32 inNumberFrames,
	AudioBufferList *ioData)
{
	phastream_t *as = ((phastream_t *)inRefCon);
	ca_dev *cadev = (ca_dev *) as->drvinfo;
	int needMore;
	int outCount = inNumberFrames * sizeof(short);
	char *playBuf = (char *) ioData->mBuffers[0].mData;

	/* do we have some data left from previous callback? */
	if (cadev->tmpOutputCount) {
		int xsize = (cadev->tmpOutputCount > outCount) ? outCount : cadev->tmpOutputCount;
		int newtmpOutputCount;
		
		memcpy(playBuf, cadev->tmpOutputBuffer, xsize);
		
		outCount -= xsize;
		newtmpOutputCount = cadev->tmpOutputCount - xsize;
		cadev->tmpOutputCount = newtmpOutputCount;
		if (newtmpOutputCount) {
			memcpy(cadev->tmpOutputBuffer, cadev->tmpOutputBuffer+xsize, newtmpOutputCount);
		}
		
		playBuf += xsize;
	}
	
	
	/* did we fill the playback buffer completely? */
	if (!outCount)
		return noErr;
	
	needMore = outCount;
	while (needMore > as->ms.codec->decoded_framesize) {
		int chunkSize = as->ms.codec->decoded_framesize;
		
		cadev->cbk(as, (void *) 0, 0,  playBuf, &chunkSize);
		
		needMore -= chunkSize;
		playBuf += chunkSize;
		
		if (chunkSize != as->ms.codec->decoded_framesize)
			break;
	}
	
	if (needMore >= as->ms.codec->decoded_framesize) {
		memset(playBuf, 0, needMore);
	} 
	else if (needMore) {
		/* 
		we still need some data to fill the buffer, but the amount needed 
		 is LESS than complete decoded audio frame 
		 */
		int chunkSize = as->ms.codec->decoded_framesize;
		
		cadev->cbk(as, (void *) 0, 0,  cadev->tmpOutputBuffer,  &chunkSize);
		cadev->tmpOutputCount = chunkSize;
		ph_printf("**CoreAudio: chunkSize: %d, needMore: %d\n", chunkSize, needMore);
		
		/* did we got some more data? */ 
		if (chunkSize) {
			int xsize = (chunkSize > needMore) ? needMore : chunkSize;
			int newtmpOutputCount;
			
			memcpy(playBuf, cadev->tmpOutputBuffer, xsize);
			
			needMore -= xsize;
			newtmpOutputCount = cadev->tmpOutputCount - xsize;
			cadev->tmpOutputCount = newtmpOutputCount;
			if (newtmpOutputCount) {
				memcpy(cadev->tmpOutputBuffer, cadev->tmpOutputBuffer + xsize, newtmpOutputCount);
			}
			
			playBuf += xsize;
			
		}
		
		/* if we still need more data fill it with zeroes */
		if (needMore > 0)
			memset(playBuf, 0, needMore);
	}
	
	return noErr;	
}


static OSStatus input_proc(AudioDeviceID device,
	const AudioTimeStamp *currentTime,
	const AudioBufferList *inputData,
	const AudioTimeStamp *inputTime,
	AudioBufferList *outputData,
	const AudioTimeStamp *outputTime,
	void *context)
{
	OSStatus err = noErr;
	phastream_t *as = ((phastream_t *) context);
	ca_dev *cadev = (ca_dev *) as->drvinfo;

	cadev->recordBuffer = inputData->mBuffers[0].mData;
	cadev->recordBufferCount = inputData->mBuffers[0].mDataByteSize;
	
	ph_printf("**CoreAudio: available input data: %d\n",
		  inputData->mBuffers[0].mDataByteSize);

	ph_printf("**CoreAudio: phapi framesize:%d, input converter buffer size: %d\n",
		as->ms.codec->decoded_framesize, cadev->inputConverterBufferSize);
	
	memcpy(cadev->tmpInputBuffer + cadev->tmpInputCount, cadev->recordBuffer, cadev->recordBufferCount);
	cadev->tmpInputCount += cadev->recordBufferCount;
	
	if (cadev->tmpInputCount >= cadev->inputConverterBufferSize) {
		cadev->convertedInputCount = sizeof(cadev->convertedInputBuffer); //FIXME: sizeof will not work if convertedInputBuffer is set dynamically
		unsigned savedtmpInputCount = cadev->tmpInputCount;
		cadev->currentInputBuffer = cadev->tmpInputBuffer;
		cadev->sumDataSize = 0;
		
		err = AudioConverterFillBuffer(cadev->inputConverter, buffer_data_proc, cadev,
			&cadev->convertedInputCount, cadev->convertedInputBuffer);
		if (err != noErr) {
			ph_printf("!!CoreAudio: error while converting\n");
		}
		ph_printf("**CoreAudio: converted data: %d\n", cadev->convertedInputCount);
		
		// Send converted data to phapi
		cadev->cbk(as, cadev->convertedInputBuffer, cadev->convertedInputCount, NULL, 0);
		
		unsigned usedData = cadev->sumDataSize;
		if (usedData != 0) {
			unsigned remainingData = savedtmpInputCount - usedData;
			memcpy(cadev->tmpInputBuffer, cadev->tmpInputBuffer + usedData, remainingData);
			cadev->tmpInputCount = remainingData;
		}
		ph_printf("**CoreAudio: used data: %d\n", usedData);
	}
	
	return noErr;
}


static OSStatus buffer_data_proc(AudioConverterRef inAudioConverter, UInt32 *ioDataSize, void **outData, void *context) {
	ca_dev *cadev = (ca_dev *) context;

	ph_printf("**CoreAudio: buffer size: %d, ioDataSize: %d\n", cadev->tmpInputCount, *ioDataSize);

	cadev->sumDataSize += *ioDataSize;
	
	unsigned enough = (cadev->tmpInputCount < *ioDataSize) ? 0 : 1;
	
	if (!enough) {
		*ioDataSize = 0;
		*outData = 0;
	} else {		
		*outData = cadev->currentInputBuffer;
		cadev->tmpInputCount -= *ioDataSize;
		cadev->currentInputBuffer += *ioDataSize;		
	}
		
	return kAudioHardwareNoError;
}


static AudioDeviceID defaultInputDevice() {
	OSStatus err = noErr;
	AudioDeviceID device;
	UInt32 size = sizeof(AudioDeviceID);
	
	err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultInputDevice, &size, &device);
	if (err != noErr) {
		ph_printf("!!CoreAudio: can't get default input device\n");
	}
	
	return device;
}


static AudioDeviceID defaultOutputDevice() {
	OSStatus err = noErr;
	AudioDeviceID device;
	UInt32 size = sizeof(AudioDeviceID);
	
	err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice, &size, &device);
	if (err != noErr) {
		ph_printf("!!CoreAudio: can't get default output device\n");
	}
	
	return device;
}


static void parse_device(ca_dev *cadev, const char *name) {
	char buf[256];
	char *input, *output, *buffer = buf;
	
	strncpy(buffer, name, sizeof(buf));
	
	ph_printf("**CoreAudio: parsing %s\n", name);
	
	if (strncasecmp(buffer, "ca:", 3) == 0)
		buffer += 3;
	
	if ((input = strcasestr(buffer, "in="))) {
		cadev->inputID = atoi(input + 3);
	} else {
		cadev->inputID = defaultInputDevice();
	}
	
	if ((output = strcasestr(buffer, "out="))) {
		cadev->outputID = atoi(output + 4);
	} else {
		cadev->outputID = defaultOutputDevice();
	}
	
	ph_printf("**CoreAudio: using devices in=%d out=%d\n", cadev->inputID, cadev->outputID);
}

static void init_audio_unit(AudioUnit *au) {
	OSStatus err = noErr;
	ComponentDescription desc;
	desc.componentType = kAudioUnitType_Output;
	desc.componentSubType = kAudioUnitSubType_HALOutput;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;
	
	Component comp = FindNextComponent(NULL, &desc);
	if (comp == NULL) {
		ph_printf("!!CoreAudio: can't audio component\n");
	}
	
	err = OpenAComponent(comp, au);
	if (err != noErr) {
		ph_printf("!!CoreAudio: can't open audio component\n");
	}
}


static void set_audio_device(AudioUnit au, AudioDeviceID id) {
	OSStatus err = noErr;
	
	err = AudioUnitSetProperty(au,
				   kAudioOutputUnitProperty_CurrentDevice,
				   kAudioUnitScope_Global,
				   0,
				   &id,
				   sizeof(id));
	
	if (err != noErr) {
		ph_printf("!!CoreAudio: can't set device #%d\n", id);
	}
}


static void set_render_callback(AudioUnit au, AURenderCallback cbk, void *data) {
	OSStatus err = noErr;
	AURenderCallbackStruct input;
	input.inputProc = cbk;
	input.inputProcRefCon = data;
	
	err = AudioUnitSetProperty(au,
				   kAudioUnitProperty_SetRenderCallback,
				   kAudioUnitScope_Input,
				   0,
				   &input,
				   sizeof(input));
	
	if (err != noErr) {
		ph_printf("!!CoreAudio: can't set render callback\n");
	}
}


static void set_input_callback(AudioDeviceID id, AudioDeviceIOProc cbk, void *data) {
	OSStatus err = noErr;

	err = AudioDeviceAddIOProc(id, cbk, data);
	if (err != noErr) {
		ph_printf("!!CoreAudio: can't set input callback\n");
	}	
}


static void init_output_device(phastream_t *as, float rate, unsigned channels, unsigned format) {
	OSStatus err = noErr;
	ca_dev *cadev = (ca_dev *) as->drvinfo;
	
	init_audio_unit(&cadev->outputAU);
	set_played_format(cadev->outputAU, rate, channels, format);
	set_audio_device(cadev->outputAU, cadev->outputID);
	set_render_callback(cadev->outputAU, output_renderer, as);

	err = AudioUnitInitialize(cadev->outputAU);
	if (err) { ph_printf ("!!CoreAudio: AudioUnitInitialize-SF=%4.4s, %ld\n", (char*)&err, err); return; }
}


static void init_input_device(phastream_t *as, float rate, unsigned channels, unsigned format) {
	ca_dev *cadev = (ca_dev *) as->drvinfo;

	set_recorded_format(as, rate, channels, format);
	set_input_callback(cadev->inputID, input_proc, as);
}


static void set_recorded_format(phastream_t *as, float rate, unsigned channels, unsigned format) {
	ca_dev *cadev = (ca_dev *) as->drvinfo;
	OSStatus err = noErr;
	AudioStreamBasicDescription imgFmt, devFmt;
	UInt32 propsize = sizeof(devFmt);
	
	err = AudioDeviceGetProperty(cadev->inputID, 0, 1, kAudioDevicePropertyStreamFormat, &propsize, &devFmt);
	if (err != noErr) {
		ph_printf("!!CoreAudio: can't get device info\n");
		return;
	}

	ph_printf("**CoreAudio: Input format:\n\t");
	ph_printf("SampleRate=%f,", devFmt.mSampleRate);
	ph_printf("BytesPerPacket=%ld,", devFmt.mBytesPerPacket);
	ph_printf("FramesPerPacket=%ld,", devFmt.mFramesPerPacket);
	ph_printf("BytesPerFrame=%ld,", devFmt.mBytesPerFrame);
	ph_printf("BitsPerChannel=%ld,", devFmt.mBitsPerChannel);
	ph_printf("ChannelsPerFrame=%ld\n", devFmt.mChannelsPerFrame);
	
	imgFmt.mSampleRate = rate;
	imgFmt.mFormatID = kAudioFormatLinearPCM;
	imgFmt.mFormatFlags = kLinearPCMFormatFlagIsBigEndian |
		kLinearPCMFormatFlagIsSignedInteger |
		kLinearPCMFormatFlagIsPacked |
		kLinearPCMFormatFlagIsNonInterleaved;
	imgFmt.mBytesPerPacket = 2;
	imgFmt.mFramesPerPacket = 1;
	imgFmt.mBytesPerFrame = 2;
	imgFmt.mChannelsPerFrame = channels;
	imgFmt.mBitsPerChannel = format;
	
	err = AudioConverterNew(&devFmt, &imgFmt, &cadev->inputConverter);
	if (err != noErr) {
		ph_printf("!!CoreAudio: can't create audio converter for input\n");
		return;
	}
	
	propsize = sizeof(unsigned);
	cadev->inputConverterBufferSize = as->ms.codec->decoded_framesize;
	
	err = AudioConverterGetProperty(cadev->inputConverter, kAudioConverterPropertyCalculateInputBufferSize,
		&propsize, &cadev->inputConverterBufferSize);
	if (err != noErr) {
		ph_printf("!!CoreAudio: can't get input converter buffer size\n");
		return;
	}
}


static void set_played_format(AudioUnit au, float rate, unsigned channels, unsigned format) {
	OSStatus err = noErr;
	
	AudioStreamBasicDescription streamFormat;
	streamFormat.mSampleRate = rate;
	streamFormat.mFormatID = kAudioFormatLinearPCM;
	streamFormat.mFormatFlags = 
		// Defaults to Little endian, otherwise specify kLinearPCMFormatFlagIsBigEndian
		kLinearPCMFormatFlagIsBigEndian |
		kLinearPCMFormatFlagIsSignedInteger | //FIXME: need to be tested for more portability
		//kLinearPCMFormatFlagIsPacked |
		kLinearPCMFormatFlagIsNonInterleaved;
	streamFormat.mBytesPerPacket = 2;
	streamFormat.mBytesPerFrame = 2;
	streamFormat.mFramesPerPacket = 1;
	streamFormat.mChannelsPerFrame = channels;
	streamFormat.mBitsPerChannel = format;
	
	ph_printf("**CoreAudio: Rendering source:\n\t");
	ph_printf("SampleRate=%f,", streamFormat.mSampleRate);
	ph_printf("BytesPerPacket=%ld,", streamFormat.mBytesPerPacket);
	ph_printf("FramesPerPacket=%ld,", streamFormat.mFramesPerPacket);
	ph_printf("BytesPerFrame=%ld,", streamFormat.mBytesPerFrame);
	ph_printf("BitsPerChannel=%ld,", streamFormat.mBitsPerChannel);
	ph_printf("ChannelsPerFrame=%ld\n", streamFormat.mChannelsPerFrame);
	
	err = AudioUnitSetProperty(au,
				   kAudioUnitProperty_StreamFormat,
				   kAudioUnitScope_Input,
				   0,
				   &streamFormat,
				   sizeof(AudioStreamBasicDescription));
	if (err) { ph_printf ("!!CoreAudio: AudioUnitSetProperty-SF=%4.4s, %ld\n", (char*)&err, err); return; }
}


void ph_ca_driver_init() {
	ph_printf("** Register and initialize ca audio driver\n");
	ph_register_audio_driver(&ph_ca_audio_driver);
}

void ca_start(phastream_t *as) {
	ph_printf("** Starting audio stream\n");
	
	ca_dev *cadev = (ca_dev *) as->drvinfo;
	
	verify_noerr(AudioOutputUnitStart (cadev->outputAU));
	OSStatus err = noErr;
	err = AudioDeviceStart(cadev->inputID, input_proc);
	if (err != noErr) {
		ph_printf("!!CoreAudio: can't start input proc\n");
	}
}

int ca_open(phastream_t *as, char *name, int rate, int framesize, ph_audio_cbk cbk) {
	ph_printf("** Opening device %s with rate: %d, framesize: %d, and callback: %p\n",
		  name, rate, framesize, cbk);
	
	
	ca_dev *cadev = (ca_dev *) calloc(1, sizeof(ca_dev));
	cadev->cbk = cbk;
	
	as->drvinfo = cadev;
	
	parse_device(cadev, name);
	
	init_input_device(as, rate, 1, 16); //FIXME: channels and format should be given by phapi
	init_output_device(as, rate, 1, 16);

	return 0;
}

int ca_get_out_space(phastream_t *as, int *used) {
	*used = 320;
	
	ph_printf("** Out space used: %d\n", *used);
	
	
	return *used;
}

int ca_get_avail_data(phastream_t *as) {
	ph_printf("** Available data: 0\n");
	
	
	return 320;
}



void ca_close(phastream_t *as) {
	ph_printf("** Closing audio stream\n");
	
	ca_dev *cadev = (ca_dev *) as->drvinfo;
	
	verify_noerr(AudioDeviceStop(cadev->inputID, input_proc));
	verify_noerr(AudioDeviceRemoveIOProc(cadev->inputID, input_proc));
	
	verify_noerr(AudioOutputUnitStop(cadev->outputAU));
	verify_noerr(AudioUnitUninitialize (cadev->outputAU));
	
	free(cadev);
}

