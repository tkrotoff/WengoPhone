/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2007  Wengo
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

#include <sound/AudioDeviceManager.h>

#include "../EnumDeviceType.h"

#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <util/String.h>

std::list<AudioDevice> find_alsa_devices(int input);

AudioDeviceManager::AudioDeviceManager() {
	_mutex = new QMutex(QMutex::Recursive);
}

AudioDeviceManager::~AudioDeviceManager() {
	OWSAFE_DELETE(_mutex);
}

std::list<AudioDevice> AudioDeviceManager::getInputDeviceList() {
	QMutexLocker lock(_mutex);

	//HACK: always return the default one + plughw
	std::list<AudioDevice> deviceList;
	deviceList.push_back(getDefaultInputDevice());

	StringList data;
	data += String("ALSA: plughw");
	data += String("plughw");
	data += EnumDeviceType::toString(EnumDeviceType::DeviceTypeWaveIn);
	deviceList.push_back(AudioDevice(data));

	return deviceList;
	////

	//return find_alsa_devices(1);
}

std::list<AudioDevice> AudioDeviceManager::getOutputDeviceList() {
	QMutexLocker lock(_mutex);

	//HACK: always return the default one + plughw
	std::list<AudioDevice> deviceList;
	deviceList.push_back(getDefaultOutputDevice());

	StringList data;
	data += String("ALSA: plughw");
	data += String("plughw");
	data += EnumDeviceType::toString(EnumDeviceType::DeviceTypeWaveOut);
	deviceList.push_back(AudioDevice(data));

	return deviceList;
	////

	//return find_alsa_devices(0);
}

AudioDevice AudioDeviceManager::getDefaultOutputDevice() {
	QMutexLocker lock(_mutex);

	StringList data;
	data += String("ALSA: default");
	data += String("default");
	data += EnumDeviceType::toString(EnumDeviceType::DeviceTypeWaveOut);

	return AudioDevice(data);
}

bool AudioDeviceManager::setDefaultOutputDevice(const AudioDevice & audioDevice) {
	return false;
}

AudioDevice AudioDeviceManager::getDefaultInputDevice() {
	QMutexLocker lock(_mutex);

	StringList data;
	data += String("ALSA: default");
	data += String("default");
	data += EnumDeviceType::toString(EnumDeviceType::DeviceTypeWaveIn);

	return AudioDevice(data);
}

bool AudioDeviceManager::setDefaultInputDevice(const AudioDevice & audioDevice) {
	QMutexLocker lock(_mutex);

	return false;
}

#include <alsa/asoundlib.h>

std::list<AudioDevice> find_alsa_devices(int input)
{
	int card_index = -1;

	// owsound part
	std::list<AudioDevice> deviceList;
	////

	// iterate over all sound cards
	while (snd_card_next(&card_index) == 0 && card_index >= 0)
	{
		char alsa_card_name[64];
		char *friendly_card_name;
		snd_ctl_t *snd_ctl;
		snd_ctl_card_info_t *card_info;
		size_t len;
		const char * tmpstr;

		// owsound part
		AudioDevice audioDevice;
		////

		// compute internal name of the sound card
		snprintf(alsa_card_name, sizeof(alsa_card_name), "hw:%d", card_index);
		LOG_DEBUG("alsa_card_name: " + String(alsa_card_name));
		////

		// open the sound card
		if( snd_ctl_open(&snd_ctl, alsa_card_name, 0 ) < 0 )
		{
			LOG_WARN("unable to open card: " + String(alsa_card_name));
			break;
		}
		////

		// get info from sound card
		snd_ctl_card_info_alloca(&card_info);
		snd_ctl_card_info(snd_ctl, card_info);

		tmpstr = snd_ctl_card_info_get_name(card_info);
		len = strlen(tmpstr) + 1;
		friendly_card_name = (char*)malloc(len);
		if (friendly_card_name == NULL)
		{
			LOG_WARN("unable to allocate memory: " + String::fromNumber(len));
			break;
		}
		strncpy(friendly_card_name, tmpstr, len);
		LOG_DEBUG("friendly_card_name: " + String(friendly_card_name));
		////

		int device_index = -1;
		// iterate over sound card devices
		while (snd_ctl_pcm_next_device(snd_ctl, &device_index) == 0 && device_index >= 0)
		{
			char alsa_device_name[64];
			char *friendly_device_name;
			snd_pcm_info_t *pcm_info;
			int has_capture = 0;
			int has_playback = 0;

			// compute internal device name
			snprintf(alsa_device_name, sizeof(alsa_device_name), "%s:%d,%d", "hw",
			card_index, device_index);
			LOG_DEBUG("alsa_device_name: " + String(alsa_device_name));
			////

			// get info from the device
			snd_pcm_info_alloca(&pcm_info);
			snd_pcm_info_set_device(pcm_info, device_index);
			snd_pcm_info_set_subdevice(pcm_info, 0);

			// check if device has capture
			snd_pcm_info_set_stream(pcm_info, SND_PCM_STREAM_CAPTURE);
			if (snd_ctl_pcm_info(snd_ctl, pcm_info) >= 0)
			{
				has_capture = 1;
			}
			////

			// check if device has playback
			snd_pcm_info_set_stream(pcm_info, SND_PCM_STREAM_PLAYBACK);
			if (snd_ctl_pcm_info(snd_ctl, pcm_info) >= 0)
			{
				has_playback = 1;
			}
			////

			if (!has_capture && !has_playback)
			{
				LOG_WARN("this device has no capture and no playback\n");
				continue;
			}

			// get friendly device name
			len = snprintf(NULL, 0, "%s", snd_pcm_info_get_name(pcm_info )) + 1;
			friendly_device_name = (char*)malloc(len);
			snprintf(friendly_device_name, len, "%s", snd_pcm_info_get_name(pcm_info));
			LOG_DEBUG("friendly_device_name: " + String(friendly_device_name));
			////

			////

			if ((input && has_capture) || (!input && has_playback))
			{
				// owsound part
				StringList data;
				data += String(friendly_device_name);
				data += String(alsa_device_name);
				//data += EnumDeviceType::toString(EnumDeviceType::DeviceTypeWaveIn);
				deviceList.push_back(AudioDevice(data));
				////
			}
		}

		snd_ctl_close(snd_ctl);
	}

	return deviceList;
}
