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
 * NULL Webcam class
 *
 * @author David Ferlier
 */



#ifndef WEBCAM_NULL_H
#define WEBCAM_NULL_H

#include <webcam/webcam.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>

class WebcamNull : public Webcam {
    public:
        WebcamNull();
        ~WebcamNull();
        int open();
        void close();
		void uninitialize();
        void startCapture();
        void stopCapture();
        void readCaps();
        virtual int setPalette(int);
        virtual int getPalette() const;
        int setResolution(int, int);
        int setFps(int);
        virtual int getCurrentWidth() const;
        virtual int getCurrentHeight() const;
        int getFrameSize();

    private:
        int cached_palette;
        int cached_width;
        int cached_height;
        int cached_fps;
};

#endif
