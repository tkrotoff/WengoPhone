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

#include <AudioDevice.h>

#include <portaudio.h>
#ifdef WIN32
#include <windows.h>
#endif

#define AUDIODEVICE_VERBOSE

#include <stdio.h>

/**
 * @author Mathieu Stute
 * @author David Ferlier
 */

StringList AudioDevice::getInputMixerDeviceList() {
	int i, numDevices;
	const PaDeviceInfo *deviceInfo;
	PaError err;
	StringList deviceList;
	
	Pa_Initialize();
	
	//retrive number of device found
	numDevices = Pa_GetDeviceCount();
	if( numDevices < 0 ) {
		printf( "ERROR: Pa_CountDevices returned 0x%x\n", numDevices );
		err = numDevices;
		Pa_Terminate();
		fprintf( stderr, "An error occured while using the portaudio stream\n" );
		fprintf( stderr, "Error number: %d\n", err );
		fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
		return StringList();
	}
	
	//iterate over devices
	for( i=0; i<numDevices; i++ ) {
		deviceInfo = Pa_GetDeviceInfo( i );
		
		if( deviceInfo->maxInputChannels > 0 ) {
			String deviceName;
			if( (Pa_GetHostApiInfo(deviceInfo->hostApi)->type  == paALSA) ||
			    (Pa_GetHostApiInfo(deviceInfo->hostApi)->type  == paOSS) ||
			  ( i !=Pa_GetHostApiInfo( deviceInfo->hostApi )->defaultInputDevice ))
			{
#ifdef AUDIODEVICE_VERBOSE
				printf("PA :: Input device found: %s\t form host API: %s\n", deviceInfo->name, Pa_GetHostApiInfo(deviceInfo->hostApi)->name);
#endif
				deviceList += deviceInfo->name;
			}
		}
	}
	Pa_Terminate();
	return deviceList;
}
 
StringList AudioDevice::getOutputMixerDeviceList() {
	int i, numDevices;
	const PaDeviceInfo *deviceInfo;
	PaError err;
	StringList deviceList;
	
	Pa_Initialize();
    
	//retrive number of device found
	numDevices = Pa_GetDeviceCount();
	if( numDevices < 0 ) {
		printf( "ERROR: Pa_CountDevices returned 0x%x\n", numDevices );
		err = numDevices;
		Pa_Terminate();
		fprintf( stderr, "An error occured while using the portaudio stream\n" );
		fprintf( stderr, "Error number: %d\n", err );
		fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
		return StringList();
	}
	
	//iterate over devices
	for( i=0; i<numDevices; i++ ) {
		deviceInfo = Pa_GetDeviceInfo( i );
		if( deviceInfo->maxOutputChannels > 0 ) {
			String deviceName;
			if( (Pa_GetHostApiInfo(deviceInfo->hostApi)->type  == paALSA) ||
			    (Pa_GetHostApiInfo(deviceInfo->hostApi)->type  == paOSS)  ||
			  ( i !=Pa_GetHostApiInfo( deviceInfo->hostApi )->defaultOutputDevice )) 
			{
#ifdef AUDIODEVICE_VERBOSE
				printf("PA :: Output device found: %s\t form host API: %s\n", deviceInfo->name, Pa_GetHostApiInfo(deviceInfo->hostApi)->name);
#endif
				deviceList += deviceInfo->name;
			}
		}
	}
	Pa_Terminate();
	return deviceList;
}

std::string AudioDevice::getDefaultPlaybackDevice() {
	int i, numDevices;
	const PaDeviceInfo *deviceInfo;
	PaError err;

#ifdef AUDIODEVICE_VERBOSE
	printf("PA :: getDefaultPlaybackDevice()\n");
#endif
	
	Pa_Initialize();
	
	//retrive number of device found
	numDevices = Pa_GetDeviceCount();
	if( numDevices < 0 ) {
		printf( "ERROR: Pa_CountDevices returned 0x%x\n", numDevices );
		err = numDevices;
		Pa_Terminate();
		fprintf( stderr, "An error occured while using the portaudio stream\n" );
		fprintf( stderr, "Error number: %d\n", err );
		fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
		return "";
	}
	
	//iterate over devices
	for( i=0; i<numDevices; i++ ) {
		deviceInfo = Pa_GetDeviceInfo( i );
		if( i == Pa_GetDefaultOutputDevice() ) {
#ifdef AUDIODEVICE_VERBOSE
			printf("\n\nPA :: Default device %s\n", deviceInfo->name);
#endif
			std::string name(deviceInfo->name);
			Pa_Terminate();
			return name;
		}
	}
	
	Pa_Terminate();
	return "";
}

bool AudioDevice::setDefaultPlaybackDevice(const std::string & /*deviceName*/) {
	return false;
}

bool AudioDevice::setDefaultRecordDevice(const std::string & /*deviceName*/) {
	return false;
}

int AudioDevice::getWaveOutDeviceId(const std::string & deviceName) {
	int deviceId = 0;
	const PaDeviceInfo *pdi;
	int i;

	Pa_Initialize();

	if( deviceName.length() ) {
		for(i = 0; i < Pa_GetDeviceCount(); i++) {
			pdi = Pa_GetDeviceInfo(i);

			if (pdi->maxOutputChannels > 0 && pdi->name == deviceName) {
				deviceId = Pa_HostApiDeviceIndexToDeviceIndex(pdi->hostApi, i);
#ifdef AUDIODEVICE_VERBOSE
				printf("\nPA :: getWaveOutDeviceId(), found output device id %d for %s (%s)\n", i, deviceName.c_str(), pdi->name);
#endif
				Pa_Terminate();
				//return deviceId;
				return i;
			}
		}
	}
	Pa_Terminate();
	return 0;
}

int AudioDevice::getWaveInDeviceId(const std::string & deviceName) {
	const PaDeviceInfo *pdi;
	int deviceId = 0;
	int i;
	
	Pa_Initialize();
	
	if( deviceName.length() ) {
		for(i = 0; i < Pa_GetDeviceCount(); i++) {
			pdi = Pa_GetDeviceInfo(i);

			if (pdi->maxInputChannels > 0 && pdi->name == deviceName) {
				deviceId = Pa_HostApiDeviceIndexToDeviceIndex(pdi->hostApi, i);
#ifdef AUDIODEVICE_VERBOSE
				printf("\nPA :: getWaveInDeviceId(), found input device id %d for %s (%s)\n", i, deviceName.c_str(), pdi->name);
#endif
				Pa_Terminate();
				//return deviceId;
				return i;
			}
		}
	}
#ifdef AUDIODEVICE_VERBOSE
	printf("\nPA :: getWaveInDeviceId(), didn't found input device id for %s\n", deviceName.c_str());
#endif
    Pa_Terminate();
	return 0;
}

int AudioDevice::getMixerDeviceId(const std::string & mixerName) {
#ifdef WIN32
	unsigned int nbMixers = ::mixerGetNumDevs();
	if (nbMixers == 0) {
		//No audio mixer device are present
		return -1;
	}

	MIXERCAPSA mixcaps;

	for (unsigned int mixerId = 0; mixerId < nbMixers; mixerId++) {
		if (MMSYSERR_NOERROR == ::mixerGetDevCapsA(mixerId, &mixcaps, sizeof(MIXERCAPSA))) {
			if (mixerName == mixcaps.szPname) {
				return mixerId;
			}
		}
	}
#endif
	//Default deviceId is 0
	return 0;
}

std::string AudioDevice::getDefaultRecordDevice() {
	return "";
}
