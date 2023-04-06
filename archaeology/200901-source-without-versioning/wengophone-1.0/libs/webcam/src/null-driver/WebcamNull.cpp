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

/**
 * Null Webcam class
 *
 * @author David Ferlier
 */

#include <webcam/WebcamNull.h>

#include <stdio.h>
#include <iostream>

using namespace std;

WebcamNull::WebcamNull() {
	cached_palette = WEBCAM_PALETTE_UNSUPPORTED;
	cached_width = 0;
	cached_height = 0;
	cached_fps = 0;
}

int WebcamNull::open() {
    setFlag(WEBCAM_FLAG_OPENED);
    return WEBCAM_CODE_OK;
}

void WebcamNull::uninitialize() {
}

WebcamNull::~WebcamNull() {
}

void WebcamNull::readCaps() {
}

int WebcamNull::getPalette() const {
	return cached_palette;
}

int WebcamNull::getCurrentWidth() const {
	return cached_width;
}

int WebcamNull::getCurrentHeight() const {
	return cached_height;
}

void WebcamNull::stopCapture() {
}

void WebcamNull::startCapture() {
}

int WebcamNull::getFrameSize() {
	return calculateFrameSize(cached_palette, cached_width, cached_height);
}

int WebcamNull::setResolution(int width, int height) {
    cached_width = width;
    cached_height = height;
    return WEBCAM_CODE_OK;
}

int WebcamNull::setPalette(int palette) {
    cached_palette = palette;
    return WEBCAM_CODE_OK;
}

int WebcamNull::setFps(int frames_per_second) {
    return WEBCAM_CODE_OK;
}

void WebcamNull::close() {
}
