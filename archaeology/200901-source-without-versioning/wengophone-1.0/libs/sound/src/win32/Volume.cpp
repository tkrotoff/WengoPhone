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
 * Inspired from Win32Volume class from the Zinf
 * (former FreeAmp) project http://www.zinf.org/
 * Check aswell http://www.codeproject.com/audio/admixer.asp
 */

#include "Volume.h"

#include <iostream>
using namespace std;

Volume::Volume(unsigned int deviceId, DeviceType deviceType) throw (SoundMixerException) {
	_hMixer = NULL;
	MMRESULT mr = initVolumeControl(deviceId, deviceType);
	if (mr != MMSYSERR_NOERROR) {
		throw SoundMixerException("Error while opening the mixer", mr);
	}
}

Volume::~Volume() {
}

int Volume::getVolume() {
	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;

	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = _dwVolumeControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;

	MMRESULT mr = ::mixerGetControlDetailsA((HMIXEROBJ) _hMixer, &mxcd,
					MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE);
	if (mr != MMSYSERR_NOERROR) {
		return -1;
	}

	return (int) (( (float) ((mxcdVolume.dwValue - _dwMinimum) * 100) /
			(float) (_dwMaximum - _dwMinimum)) + 0.5);
}

bool Volume::setVolume(unsigned int volume) {
	DWORD dwVolume = (volume * (_dwMaximum - _dwMinimum) / 100);

	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
	MIXERCONTROLDETAILS mxcd;

	memcpy(&mxcdVolume, &dwVolume, sizeof(MIXERCONTROLDETAILS_UNSIGNED));

	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = _dwVolumeControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;

	MMRESULT mr = ::mixerSetControlDetails((HMIXEROBJ) _hMixer, &mxcd, 
				MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE);
	if (mr != MMSYSERR_NOERROR) {
		return false;
	}
	return true;
}

MMRESULT Volume::initVolumeControl(unsigned int deviceId, DeviceType deviceType) {
	MMRESULT mr = ::mixerOpen(&_hMixer, deviceId, NULL, NULL, MIXER_OBJECTF_MIXER);
	if (mr != MMSYSERR_NOERROR) {
		_hMixer = NULL;
		return mr;
	}

	MIXERCAPSA mxcaps;
	mr = ::mixerGetDevCapsA(deviceId, &mxcaps, sizeof(MIXERCAPSA));
	if (mr != MMSYSERR_NOERROR) {
		return mr;
	}

	cerr << "Volume: manufacturer's name for the mixer " <<
		mxcaps.szPname << " " << mxcaps.wMid << " " << mxcaps.wPid << endl;

	DWORD dwComponentType;

	switch (deviceType) {
	case WaveOut:
		dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
		break;

	case WaveIn:
		dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
		break;

	case CDOut:
		dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;
		break;

	case MicrophoneOut:
		dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;
		break;

	case MicrophoneIn:
		dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
		break;

	case MasterVolume:
	default:
		dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
		break;
	}

	mr = createMixerLine(dwComponentType);
	if (mr != MMSYSERR_NOERROR) {
		return mr;
	}

	//For microphone in, we first look for the wave in mixer
	//and then for the microphone
	if (deviceType == MicrophoneIn) {
		mr = createSecondMixerLine(MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE);
		if (mr != MMSYSERR_NOERROR) {
			return mr;
		}
	}
	//

	mr = createMixerControl(MIXERCONTROL_CONTROLTYPE_VOLUME);
	if (mr != MMSYSERR_NOERROR) {
		return mr;
	}

	//Record dwControlID
	/*
	m_oDstLineName = _mxl.szName;	//Destination line name
	m_oVolumeControlName = _mxc.szName;	//Volume controller name
	*/
	_dwMinimum = _mxc.Bounds.dwMinimum;
	_dwMaximum = _mxc.Bounds.dwMaximum;
	_dwVolumeControlID = _mxc.dwControlID;

	cerr << "Volume: " << _mxl.szName << "/" << _mxc.szName << endl;

	//Everything went fine
	return mr;
}

MMRESULT Volume::createMixerLine(DWORD dwComponentType) {
	_mxl.cbStruct = sizeof(MIXERLINEA);
	_mxl.dwComponentType = dwComponentType;

	cerr << "Volume: createMixerLine() ::mixerGetLineInfoA()" << endl;

	return ::mixerGetLineInfoA((HMIXEROBJ) _hMixer, &_mxl,
				MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);
}

MMRESULT Volume::createSecondMixerLine(DWORD dwComponentType) {
	unsigned int connections = _mxl.cConnections;
	DWORD destination = _mxl.dwDestination;
	MMRESULT mr;

	for (unsigned int i = 0; i < connections; ++i) {
		_mxl.cbStruct = sizeof(MIXERLINEA);
		_mxl.dwSource = i;
		_mxl.dwDestination = destination;

		cerr << "Volume: createSecondMixerLine() ::mixerGetLineInfoA()" << endl;

		mr = ::mixerGetLineInfoA((HMIXEROBJ) _hMixer, &_mxl,
						MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_SOURCE);

		if (mr == MMSYSERR_NOERROR && _mxl.dwComponentType == dwComponentType) {
			break;
		}
	}
	return mr;
}

MMRESULT Volume::createMixerControl(DWORD dwControlType) {
	_mxlc.cbStruct = sizeof(MIXERLINECONTROLSA);
	_mxlc.dwLineID = _mxl.dwLineID;

	//MIXERCONTROL_CONTROLTYPE_VOLUME
	//MIXERCONTROL_CONTROLTYPE_MIXER
	//MIXERCONTROL_CONTROLTYPE_MUX
	_mxlc.dwControlType = dwControlType;

	_mxlc.cControls = 1;
	_mxlc.cbmxctrl = sizeof(MIXERCONTROLA);
	_mxlc.pamxctrl = &_mxc;

	cerr << "Volume: createMixerControl() ::mixerGetLineControlsA()" << endl;

	return ::mixerGetLineControlsA((HMIXEROBJ) _hMixer, &_mxlc,
					MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);
}

void Volume::closeMixer() {
	if (_hMixer) {
		MMRESULT mr = ::mixerClose(_hMixer);
		if (mr != MMSYSERR_NOERROR) {
		}
	}
}
