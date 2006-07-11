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

#include <sound/Sound.h>

#include <util/NonCopyable.h>

#include <QSound>

/**
 * SoundPrivate is a QSound object under UNIX.
 *
 * @author Tanguy Krotoff
 */

Sound::Sound(const std::string & filename) {
}

Sound::~Sound() {
}

void Sound::setLoops(int loops) {
}

bool Sound::setWaveOutDevice(const AudioDevice & /*device*/) {
	return false;
}

void Sound::stop() {
}

void Sound::play() {
}

void Sound::play(const std::string & filename, const AudioDevice & /*device*/) {
}
