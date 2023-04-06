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

#define ph_printf  

int ca_read(phastream_t *as, void *buf, int len);

/**
 * Declare the driver to phmedia-audio and initialize it.
 */
void ph_ca_driver_init();

/**
 * Start the stream.
 *
 * @return 
 */
void ca_start(phastream_t *as);

/**
 * Open a device.
 *
 * @param name desired devices. eg: "ca:IN=1 OUT=257"
 * @param rate desired audio rate
 * @param framesize 
 * @param cbk 
 * @return 
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
	ph_audio_cbk cbk;
	char tmpOutBuf[4096];
	int tmpOutCount;
	char tmpInBuf[4096];
	int tmpInCount;
	char *recBuf;
	unsigned recSize;
} ca_dev;


/**
 * @param au AudioUnit to set.
 */
static void init_audio_unit(AudioUnit *au);


/**
 * Set Audio device.
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
 * @param data parameter for callback
 */
static void set_render_callback(AudioUnit au, AURenderCallback cbk, void *data);


/**
 * Set the intput callback.
 *
 * @param id AudioDevice to set up
 * @param cbk Callback to use
 * @param data parameter for callback
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
static void init_input_device(phastream_t *as, float rate, unsigned channels, unsigned format, unsigned framesize);


/**
 * Open and initialize output device.
 *
 * @param rate desired rate e.g. 8000 Hz
 * @param channels desired amount of channels e.g. 2 (stereo)
 * @param format data format e.g. 16 (16 bits)
 * @param framesize amount of data to send to phapi callback
 */
static void init_output_device(phastream_t *as, float rate, unsigned channels, unsigned format, unsigned framesize);


/**
 * Set format of data that will be played.
 *
 * @param au AudioUnit to set
 * @param rate desired rate e.g. 8000 Hz
 * @param channels desired amount of channels e.g. 2 (stereo)
 * @param format data format e.g. 16 (16 bits) 
 */
static void set_played_format(AudioUnit au, float rate, unsigned channels, unsigned format);

static void set_recorded_format(ca_dev *cadev, float rate, unsigned channels, unsigned format);

/**
 * Set frame size.
 *
 * @param framesize desired framesize.
 */
static void set_framesize(AudioDeviceID id, int isInput, unsigned framesize);


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
 * Setup audio unit for input.
 *
 * Need to be called before set_device
 *
 * @param au AudioUnit to setup
 */
static void enable_io(AudioUnit au);


static OSStatus buffer_data_proc(AudioConverterRef inAudioConverter, UInt32 *ioDataSize, void **outData, void *context);



struct ph_audio_driver ph_ca_audio_driver = {
	"ca",
	PH_SNDDRVR_PLAY_CALLBACK | PH_SNDDRVR_REC_CALLBACK,
	0,
	ca_start,
	ca_open,
	0,
	ca_read,
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
	if (cadev->tmpOutCount) {
		int xsize = (cadev->tmpOutCount > outCount) ? outCount : cadev->tmpOutCount;
		int newTmpOutCount;
		
		memcpy(playBuf, cadev->tmpOutBuf, xsize);
		
		outCount -= xsize;
		newTmpOutCount = cadev->tmpOutCount - xsize;
		cadev->tmpOutCount = newTmpOutCount;
		if (newTmpOutCount) {
			memcpy(cadev->tmpOutBuf, cadev->tmpOutBuf+xsize, newTmpOutCount);
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
		
		cadev->cbk(as, (void *) 0, 0,  cadev->tmpOutBuf,  &chunkSize);
		cadev->tmpOutCount = chunkSize;
		ph_printf("**CoreAudio: chunkSize: %d, needMore: %d\n", chunkSize, needMore);
		
		/* did we got some more data? */ 
		if (chunkSize) {
			int xsize = (chunkSize > needMore) ? needMore : chunkSize;
			int newTmpOutCount;
			
			memcpy(playBuf, cadev->tmpOutBuf, xsize);
			
			needMore -= xsize;
			newTmpOutCount = cadev->tmpOutCount - xsize;
			cadev->tmpOutCount = newTmpOutCount;
			if (newTmpOutCount) {
				memcpy(cadev->tmpOutBuf, cadev->tmpOutBuf + xsize, newTmpOutCount);
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
			   void *context) {
	phastream_t *as = ((phastream_t *) context);
	ca_dev *cadev = (ca_dev *) as->drvinfo;
	div_t dv;
	char *tmpBuf = cadev->tmpInBuf;
	UInt32 count = /*as->ms.codec->decoded_framesize*/as->ms.codec->decoded_framesize /*- cadev->tmpInCount*/;
	int N = as->ms.codec->decoded_framesize / 16;
	
	cadev->recBuf = inputData->mBuffers[0].mData;
	cadev->recSize = inputData->mBuffers[0].mDataByteSize;
	
	ph_printf("**CoreAudio: available input data: %d, available space: %d\n",
		  inputData->mBuffers[0].mDataByteSize, count);
	ph_printf("**CoreAudio: will copy %d bytes of data from %p to %p\n",
		  count, cadev->tmpInBuf + count, cadev->tmpInBuf + cadev->tmpInCount + sizeof(cadev->tmpInBuf));

	while (cadev->recSize) {
		count = as->ms.codec->decoded_framesize;
		AudioConverterFillBuffer(cadev->inputConverter, buffer_data_proc, cadev, &count, tmpBuf);
		//tmpBuf += as->ms.codec->decoded_framesize;

		ph_printf("**CoreAudio: converter output count: %d\n", count);
		
		cadev->cbk(as, tmpBuf, as->ms.codec->decoded_framesize, 0, 0);
		
		cadev->tmpInCount = 0;
	}
	
	return noErr;
#if 0	
	if (cadev->tmpInCount) {
		/* tmpInBuf contains a partial frame, complete it and pass it to the callback  */
		int xsize = as->ms.codec->decoded_framesize - cadev->tmpInCount;
		
		if (xsize > recSize)
			xsize = recSize;
		
		memcpy(cadev->tmpInBuf + cadev->tmpInCount, recBuf, xsize);
		cadev->tmpInCount = xsize;
		recBuf += xsize;
		recSize -= xsize;
		
		/* do we have complete frame in the tmpInBuf? */
		if (xsize ==  as->ms.codec->decoded_framesize) {
			cadev->cbk(as, cadev->tmpInBuf, as->ms.codec->decoded_framesize, 0, 0);
			cadev->tmpInCount = 0;
		}
		
	}
	
	if (!recSize)
		return noErr;
#endif	
	
	if (!cadev->tmpInCount)
		return noErr;
	
	/* how meny full frames we have ready? */
	dv = div(cadev->tmpInCount, as->ms.codec->decoded_framesize);
	
	ph_printf("**CoreAudio: passing %d frames to audio streamer\n", dv.quot);
	
	/* pass the full frames to the callback */
	if (dv.quot) {
		int len = dv.quot * as->ms.codec->decoded_framesize;
		
		cadev->cbk(as, tmpBuf, len, 0, 0);
		tmpBuf += len;
	}
	
	
	/* save the reminder of data in the tmpInBuf */
	if (dv.rem) {
		memcpy(cadev->tmpInBuf, tmpBuf, dv.rem);
		cadev->tmpInCount = dv.rem;
	}
	
	ph_printf("**CoreAudio: tmpInCount at end: %d\n", cadev->tmpInCount);
	
	return noErr;
}


static OSStatus buffer_data_proc(AudioConverterRef inAudioConverter, UInt32 *ioDataSize, void **outData, void *context) {
	ca_dev *cadev = (ca_dev *) context;
	//ph_printf("**CoreAudio: recSize: %d, ioDataSize: %d\n", cadev->recSize, *ioDataSize);
	
	unsigned xsize = (cadev->recSize < *ioDataSize) ? cadev->recSize : *ioDataSize;
	static char zeroPad[16] = { 0 };
	
	if (xsize) {
		*outData = cadev->recBuf;
		*ioDataSize = xsize;

		cadev->recSize -= xsize;
		cadev->recBuf += xsize;
	} else {
		*ioDataSize = 0;
		*outData = 0;
		/*
		*outData = zeroPad;
		*ioDataSize = sizeof(zeroPad);
		 */
	}
		
	return kAudioHardwareNoError;
}


static AudioDeviceID defaultInputDevice() {
	OSStatus err = noErr;
	AudioDeviceID device;
	UInt32 size = sizeof(AudioDeviceID);
	
	err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultInputDevice, &size, &device);
	if (err != noErr)
		ph_printf("!!CoreAudio: can't get default input device\n");
	
	return device;
}


static AudioDeviceID defaultOutputDevice() {
	OSStatus err = noErr;
	AudioDeviceID device;
	UInt32 size = sizeof(AudioDeviceID);
	
	err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice, &size, &device);
	if (err != noErr)
		ph_printf("!!CoreAudio: can't get default output device\n");
	
	return device;
}


static void parse_device(ca_dev *cadev, const char *name) {
	char buf[512];
	char *input, *output, *buffer = buf;
	
	strncpy(buffer, name, 512);
	
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
	/*
	AURenderCallbackStruct input;
	input.inputProc = cbk;
	input.inputProcRefCon = data;
	
	err = AudioUnitSetProperty(au,
				   kAudioOutputUnitProperty_SetInputCallback,
				   kAudioUnitScope_Input,
				   0,
				   &input,
				   sizeof(input));
	*/
	err = AudioDeviceAddIOProc(id, cbk, data);
	
	if (err != noErr) {
		ph_printf("!!CoreAudio: can't set input callback\n");
	}	
}


static void set_framesize(AudioDeviceID id, int isInput, unsigned framesize) {
	OSStatus err = noErr;
	unsigned propsize = sizeof(unsigned);
	unsigned value;
	
	AudioDeviceGetProperty(id, 0, isInput, kAudioDevicePropertyBufferFrameSize,
			       &propsize, &value);
	ph_printf("**CoreAudio: actual framesize: %d\n", value);

	ph_printf("**CoreAudio: setting framesize: %d\n", framesize);
	AudioValueRange range;
	UInt32 size = sizeof(range);
	
	AudioDeviceGetProperty(id, 0, isInput, kAudioDevicePropertyBufferFrameSizeRange, 
			       &size, &range);
	ph_printf("**CoreAudio: valid range: %d - %d\n", range.mMinimum, range.mMaximum);
	
	err = AudioDeviceSetProperty(id,
				     NULL,
				     0,
				     isInput,
				     kAudioDevicePropertyBufferFrameSize,
				     sizeof(unsigned),
				     &framesize);
	if (err != noErr) {
		ph_printf("!!CoreAudio: can't set buffer size\n");
	}
	
	AudioDeviceGetProperty(id, 0, isInput, kAudioDevicePropertyBufferFrameSize,
			       &propsize, &value);
	ph_printf("**CoreAudio: actual framesize: %d\n", value);
}


static void init_output_device(phastream_t *as, float rate, unsigned channels, unsigned format, unsigned framesize) {
	OSStatus err = noErr;
	ca_dev *cadev = (ca_dev *) as->drvinfo;
	
	init_audio_unit(&cadev->outputAU);
	set_played_format(cadev->outputAU, rate, channels, format);
	set_audio_device(cadev->outputAU, cadev->outputID);
	//FIXME: 2005-12-01: does not set correctly wanted framesize
	//set_framesize(cadev->outputID, 0, framesize);
	set_render_callback(cadev->outputAU, output_renderer, as);

	err = AudioUnitInitialize(cadev->outputAU);
	if (err) { ph_printf ("!!CoreAudio: AudioUnitInitialize-SF=%4.4s, %ld\n", (char*)&err, err); return; }
}


static void init_input_device(phastream_t *as, float rate, unsigned channels, unsigned format, unsigned framesize) {
	ca_dev *cadev = (ca_dev *) as->drvinfo;
/*
	UInt32 bufferSizeFrames, bufferSizeBytes;
	UInt32 propsize = sizeof(UInt32);
	
	init_audio_unit(&cadev->inputAU);

	//FIXME: should support multiple channels
	AudioUnitGetProperty(cadev->inputAU, kAudioDevicePropertyBufferFrameSize, kAudioUnitScope_Global, 0,
			     &bufferSizeFrames, &propsize);
	bufferSizeBytes = bufferSizeFrames * sizeof(short);
	
	cadev->inputBufferList.mBuffers[0].mNumberChannels = 1;
	cadev->inputBufferList.mBuffers[0].mDataByteSize = bufferSizeBytes;
	cadev->inputBufferList.mBuffers[0].mData = malloc(bufferSizeBytes);
	
	enable_io(cadev->inputAU);
	set_recorded_format(cadev, rate, channels, format);

	set_audio_device(cadev->inputAU, cadev->inputID);
	set_input_callback(cadev->inputAU, input_proc, as);
	
	err = AudioUnitInitialize(cadev->inputAU);
	if (err) { ph_printf ("!!CoreAudio: AudioUnitInitialize-SF=%4.4s, %ld\n", (char*)&err, err); return; }
*/
	set_recorded_format(cadev, rate, channels, format);
	set_input_callback(cadev->inputID, input_proc, as);
}


static void set_recorded_format(ca_dev *cadev, float rate, unsigned channels, unsigned format) {
	OSStatus err = noErr;
	AudioStreamBasicDescription imgFmt, devFmt;
	UInt32 propsize = sizeof(devFmt);
	
	err = AudioDeviceGetProperty(cadev->inputID, 0, 1, kAudioDevicePropertyStreamFormat, &propsize, &devFmt);
	if (err != noErr) {
		ph_printf("!!CoreAudio: can't get device info\n");
		return;
	}
	
	imgFmt.mSampleRate = rate;
	imgFmt.mFormatID = kAudioFormatLinearPCM;
	imgFmt.mFormatFlags = kLinearPCMFormatFlagIsBigEndian | kLinearPCMFormatFlagIsSignedInteger;
	imgFmt.mBytesPerPacket = 2;
	imgFmt.mFramesPerPacket = 1;
	imgFmt.mBytesPerPacket = 2;
	imgFmt.mChannelsPerFrame = channels;
	imgFmt.mBitsPerChannel = format;
	
	err = AudioConverterNew(&devFmt, &imgFmt, &cadev->inputConverter);
	if (err != noErr) {
		ph_printf("!!CoreAudio: can't create audio converter for input\n");
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


static void enable_io(AudioUnit au) {
	OSStatus err = noErr;
	UInt32 enableIO;
	
	// Enabling input
	enableIO = 1;
	err = AudioUnitSetProperty(au,
				   kAudioOutputUnitProperty_EnableIO,
				   kAudioUnitScope_Input,
				   1,
				   &enableIO,
				   sizeof(enableIO));
	if (err != noErr) {
		ph_printf("!!CoreAudio: can't enable input on audio unit\n");
	}
	
	//Disabling output
	enableIO = 0;
	err = AudioUnitSetProperty(au,
				   kAudioOutputUnitProperty_EnableIO,
				   kAudioUnitScope_Output,
				   0,
				   &enableIO,
				   sizeof(enableIO));
	if (err != noErr) {
		ph_printf("!!CoreAudio: can't disable output on audio unit\n");
	}
	
}


void ph_ca_driver_init() {
	ph_printf("** Register and initialize ca audio driver\n");
	ph_register_audio_driver(&ph_ca_audio_driver);
}

void ca_start(phastream_t *as) {
	ph_printf("** Starting audio stream\n");
	
	ca_dev *cadev = (ca_dev *) as->drvinfo;
	
	verify_noerr(AudioOutputUnitStart (cadev->outputAU));
	//verify_noerr(AudioOutputUnitStart (cadev->inputAU));
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
	
	init_input_device(as, rate, 1, 16, framesize);
	init_output_device(as, rate, 1, 16, framesize);

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
	
//	verify_noerr(AudioOutputUnitStop(cadev->inputAU));
//	verify_noerr(AudioUnitUninitialize (cadev->inputAU));
	verify_noerr(AudioDeviceStop(cadev->inputID, input_proc));
	verify_noerr(AudioDeviceRemoveIOProc(cadev->inputID, input_proc));
	
	verify_noerr(AudioOutputUnitStop(cadev->outputAU));
	verify_noerr(AudioUnitUninitialize (cadev->outputAU));
	
	free(cadev);
}


int ca_read(phastream_t *as, void *buf, int len) {
	ph_printf("** Reading %d bytes of data and putting it into buffer %p\n",
		  len, buf);
	
	return 320;
}
