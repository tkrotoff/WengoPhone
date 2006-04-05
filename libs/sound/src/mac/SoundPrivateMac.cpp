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

#include "SoundPrivateMac.h"

@implementation SoundPrivateMacObjC

- (id)initWithFilename:(NSString *)filename
{
	[super init];

	_pool = [[NSAutoreleasePool alloc] init];

	_sound = [[NSSound alloc] initWithContentsOfFile:filename byReference:YES];
	[_sound setDelegate:self];

	_loops = 0;

	_stopped = NO;

	return self;
}

- (void)dealloc
{
	[_sound release];
	[_pool release];
	[super dealloc];
}

- (void)play
{
	[_sound play];
}

- (void)stop
{
	_stopped = YES;
	[_sound stop];
}

- (void)sound:(NSSound *)sound didFinishPlaying:(BOOL)aBool
{
	if (!_stopped) {
		if (_loops > 0) {
			_loops--;
			[_sound play];
		} else if (_loops == -1) {
			[_sound play];
		}
	}
}

- (void)setLoops:(int)loops
{
	_loops = loops;
}

@end

SoundPrivateMac::SoundPrivateMac(const std::string & filename) {
	NSString * pathName = NULL;
	const char * fname = filename.c_str();

	pathName = [NSString stringWithCString:fname];
	_soundPrivateObjC = [[SoundPrivateMacObjC alloc] initWithFilename:pathName];
}

SoundPrivateMac::~SoundPrivateMac() {
	[_soundPrivateObjC release];
}

void SoundPrivateMac::setLoops(int loops) {
	[_soundPrivateObjC setLoops:loops];
}

bool SoundPrivateMac::setWaveOutDevice(const std::string & deviceName) {
	return true;
}

void SoundPrivateMac::play() {
	[_soundPrivateObjC play];
}

void SoundPrivateMac::stop() {
	[_soundPrivateObjC stop];
}
