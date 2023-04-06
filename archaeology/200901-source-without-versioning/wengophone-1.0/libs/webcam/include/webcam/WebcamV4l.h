
#ifndef WEBCAM_V4L_H
#define WEBCAM_V4L_H

#include <webcam/webcam.h>
#include <wtimer.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/videodev.h>
#include <pthread.h>

class WebcamVideo4Linux : public Webcam {
    public:
        WebcamVideo4Linux();
        int open();
        void close();
		void uninitialize();
        void startCapture();
        void readCaps();
        virtual int setPalette(int);
        virtual int getPalette() const;
        int setResolution(int, int);
		int setFps(int);
        virtual int getCurrentWidth() const;
        virtual int getCurrentHeight() const;
        int getFrameSize();
        int f_handle;

    private:
        pthread_t read_thread;
        struct video_capability vCaps;
        struct video_window vWin;
        struct video_picture vPic;
        struct video_clip vClips;

        struct timer_impl *t_impl;
        w_timer_t *timer;
};

#endif
