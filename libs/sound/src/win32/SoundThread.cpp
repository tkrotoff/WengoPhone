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

#include "SoundThread.h"

#include "playsound/PlaySoundFile.h"

#include <iostream>
using namespace std;
#include <cassert>

SoundThread::SoundThread(const std::string & filename) {
	_filename = filename;
	_stop = false;

	//Plays the sound only one time by default
	_loops = 1;
}

SoundThread::~SoundThread() {
}

bool SoundThread::setWaveOutDevice(const std::string & deviceName) {
	_deviceName = deviceName;
	return true;
}

void SoundThread::setLoops(int loops) {
	_loops = loops;
}

void SoundThread::run() {
	PlaySoundFile soundFile;
	soundFile.setWaveOutDevice(_deviceName);

	int i = 0;
	while ((i < _loops || _loops == -1) && !_stop) {
		soundFile.play(_filename);
		i++;
	}
	_stop = false;
}

void SoundThread::stop() {
	_stop = true;
}
