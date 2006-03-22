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

#include <sound/SoundMixer.h>

#include <cutil/global.h>

#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#if defined(OS_LINUX)
	#include <linux/soundcard.h>
#elif defined(OS_BSD)
	#include <sys/soundcard.h>
#endif

#include <iostream>
using namespace std;

const char * sound_device_names[] = SOUND_DEVICE_NAMES;

SoundMixer::SoundMixer(const std::string & /*inputDeviceName*/, const std::string & /*outputDeviceName*/)
	throw(NoSoundCardException, SoundMixerException) {
}

void SoundMixer::closeMixers() {
}

int SoundMixer::getOutputVolume() {
	int fd, devmask, i, level;

	fd = open("/dev/mixer", O_RDONLY);
	ioctl(fd, SOUND_MIXER_READ_DEVMASK, & devmask);

	//Find device
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++) {
		if (((1 << i) & devmask) && !strcmp("pcm", sound_device_names[i])) {
			break;
		}
	}

	ioctl(fd, MIXER_READ(i), & level);
	level = level >> 8;
	close(fd);
	return level;
}

int SoundMixer::getInputVolume() {
	int fd, devmask, i, level;

	fd = open("/dev/mixer", O_RDONLY);
	ioctl(fd, SOUND_MIXER_READ_DEVMASK, & devmask);

	//Find device
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++) {
		if (((1 << i) & devmask) && !strcmp("igain", sound_device_names[i])) {
			break;
		}
	}

	ioctl(fd, MIXER_READ(i), & level);
	level = level >> 8;
	close(fd);
	return level;
}

void SoundMixer::setOutputVolume(int volume) {
	int fd, devmask, i, level;

	fd = open("/dev/mixer", O_RDONLY);
	ioctl(fd, SOUND_MIXER_READ_DEVMASK, & devmask);

	//Find device
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++) {
		if (((1 << i) & devmask) && !strcmp("pcm", sound_device_names[i])) {
			break;
		}
	}

	level = (volume << 8) + volume;
	ioctl(fd, MIXER_WRITE(i), & level);
	close(fd);
}

void SoundMixer::setInputVolume(int volume) {
	int fd, devmask, i, level;

	fd = open("/dev/mixer", O_RDONLY);
	ioctl(fd, SOUND_MIXER_READ_DEVMASK, & devmask);

	//Find device
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++) {
		if (((1 << i) & devmask) && !strcmp("igain", sound_device_names[i])) {
			break;
		}
	}

	level = (volume << 8) + volume;
	ioctl(fd, MIXER_WRITE(i), & level);
	close(fd);
}

void SoundMixer::setMicPlayBack(bool mute) {
	int fd, devmask, i, level;

	fd = open("/dev/mixer", O_RDONLY);
	ioctl(fd, SOUND_MIXER_READ_DEVMASK, & devmask);

	//Find device
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++) {
		if (((1 << i) & devmask) && !strcmp("mic", sound_device_names[i])) {
			break;
		}
	}

	if (mute) {
		level = (0 << 8) + 0;
		ioctl(fd, MIXER_WRITE(i), & level);
	} else {
		level = (100 << 8) + 100;
		ioctl(fd, MIXER_WRITE(i), & level);
	}
	close(fd);
}

bool SoundMixer::isPlaybackMuted() {
	int fd, devmask, i, level;

	fd = open("/dev/mixer", O_RDONLY);
	ioctl(fd, SOUND_MIXER_READ_DEVMASK, & devmask);

	//Find device
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++) {
		if (((1 << i) & devmask) && !strcmp("mic", sound_device_names[i])) {
			break;
		}
	}

	ioctl(fd, MIXER_READ(i), & level);
	level = level >> 8;
	close(fd);
	return (level == 0);
}
