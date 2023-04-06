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

#include "OpenGlWidget.h"

#include <avcodec.h>
#include <webcam/webcam.h>
#include <CpuUsage.h>
#include <time.h>
#include <qapplication.h>
#include <qimage.h>
#include <qpainter.h>
#include <qmainwindow.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qtextstream.h>

#include <SDL.h>
#include <SDL_main.h>

#include <iostream>
using namespace std;

enum DrawMethod {
	none,
	scalingonly,
	qimage,
	qpainter,
	opengl,
	sdl
};

DrawMethod drawMethod = none;

const int DEST_W = 800;
const int DEST_H = 600;

class WebcamTest {
public:

	WebcamTest(const QString & method);
	~WebcamTest();

	WebcamOSI *webcam;
	CCpuUsage *cpu;
	int cpuShortTermCount;
	int cpuShortTermSum;
	int cpuLongTermCount;
	int cpuLongTermSum;
	QMainWindow *mainWindow;
	QImage image;
	QPainter painter;
	OpenGlWidget *ogl;
	SDL_Surface *sdl_surface;
	SDL_Overlay *overlay;
	AVPicture avp;
};

QFile logFile;

void appendToLog(int meanCpu) {
	if (!logFile.name().isEmpty() && logFile.open(IO_WriteOnly | IO_Append)) {
		QTextStream stream(&logFile);
		stream << meanCpu << "\r\n";
		logFile.close();
	}
}

int webcam_frame_callback(void *userdata, uint8_t *sampled_data, int width_frameready, int height_frameready, int pix_osi, int len_frameready) {
	int cpuValue;
	int w, h;

	WebcamTest *wt = (WebcamTest *) userdata;
	ImgReSampleContext *res_ctx;

	AVPicture picture_yuv_resized;
	AVPicture picture_yuv_unresized;
	AVPicture picture_rgb_resized;

	w = wt->webcam->getCurrentWidth();
	h = wt->webcam->getCurrentHeight();

	uint8_t * frame_yuv_unresized = wt->webcam->convertImage(sampled_data, w, h,
				pix_osi, WEBCAM_PALETTE_YUV420P);

	res_ctx = img_resample_init(DEST_W, DEST_H, w, h);

	avpicture_alloc(&picture_yuv_resized, PIX_FMT_YUV420P, DEST_W, DEST_H);
	avpicture_alloc(&picture_rgb_resized, PIX_FMT_RGBA32, DEST_W, DEST_H);
	avpicture_fill(&picture_yuv_unresized, frame_yuv_unresized, PIX_FMT_YUV420P, w, h);

	img_resample(res_ctx, &picture_yuv_resized, &picture_yuv_unresized);

	wt->image.create(DEST_W, DEST_H, 32);
	img_convert(&picture_rgb_resized, PIX_FMT_RGBA32,
		    &picture_yuv_resized, PIX_FMT_YUV420P, DEST_W, DEST_H);
	memcpy(wt->image.bits(), picture_rgb_resized.data[0], DEST_W * DEST_H * 4);

	switch(drawMethod) {
	case none:
		break;

	case scalingonly: {
		QImage image_rescaled = wt->image.scale(wt->mainWindow->width(), wt->mainWindow->height());
		break;
	}

	case qimage: {
	        bitBlt(wt->mainWindow, 0, 0, &wt->image);
		break;
	}

	case qpainter: {
		wt->painter.begin(wt->mainWindow);
		wt->painter.drawImage(0, 0, wt->image, 0, 0, wt->image.width(), wt->image.height());
		wt->painter.end();
		break;
       }

	case opengl: {
		wt->ogl->updateBuffer(wt->image);
		break;
	}

	case sdl: {
		w = wt->webcam->getCurrentWidth();
		h = wt->webcam->getCurrentHeight();
		
		SDL_LockYUVOverlay(wt->overlay);

		memcpy(wt->overlay->pixels[0], picture_yuv_resized.data[0], DEST_W*DEST_H);
		memcpy(wt->overlay->pixels[1], picture_yuv_resized.data[1], DEST_W*DEST_H/4);
		memcpy(wt->overlay->pixels[2], picture_yuv_resized.data[2], DEST_W*DEST_H/4);



		SDL_UnlockYUVOverlay(wt->overlay);
		
		SDL_Rect rect;

		rect.x = 0;
		rect.y = 0;
		rect.w = wt->sdl_surface->w;
		rect.h = wt->sdl_surface->h;

		SDL_DisplayYUVOverlay(wt->overlay, &rect);
		break;
	}

	default:
		break;
	}

	cpuValue = wt->cpu->GetCpuUsage();
	wt->cpuShortTermSum += cpuValue;
	wt->cpuShortTermCount += 1;
	wt->cpuLongTermSum += cpuValue;
	wt->cpuLongTermCount += 1;
	if (wt->cpuShortTermCount % 50 == 0) {
		int meanCpu = wt->cpuShortTermSum/wt->cpuShortTermCount;
		printf("mean cpu: %d (long term: %d)\n", meanCpu, wt->cpuLongTermSum/wt->cpuLongTermCount);
		wt->cpuShortTermSum = 0;
		wt->cpuShortTermCount = 0;

		appendToLog(meanCpu);
	}

	av_free(frame_yuv_unresized);
	avpicture_free(&picture_yuv_resized);
	avpicture_free(&picture_rgb_resized);
	img_resample_close(res_ctx);
	return 0;
}

WebcamTest::WebcamTest(const QString & method) {
	if (drawMethod == opengl) {
		ogl = new OpenGlWidget();
		ogl->resize(DEST_W, DEST_H);
		ogl->show();
	} else if (drawMethod == sdl) {
		sdl_surface = SDL_SetVideoMode(DEST_W, DEST_H, 24, SDL_HWSURFACE | SDL_DOUBLEBUF |SDL_RESIZABLE);
	} else {
		mainWindow = new QMainWindow();
	}
	webcam = new WebcamOSI();
	cpu = new CCpuUsage();

	cpuShortTermCount = 0;
	cpuShortTermSum = 0;
	cpuLongTermCount = 0;
	cpuLongTermSum = 0;
	webcam->setFrameReadyCallback(webcam_frame_callback);
	webcam->setUserData((void *) this);
	webcam->setDeviceDefault();

	if (drawMethod != opengl && drawMethod != sdl) {
		mainWindow->setCaption(method);
		mainWindow->resize(DEST_W, DEST_H);
		mainWindow->show();
	}
	logFile.setName("WebcamTest-" + method + ".txt");

	if( webcam->open() ) {
		image.create(webcam->getCurrentWidth(), webcam->getCurrentHeight(), 32);
		if (drawMethod == sdl) {
			overlay = SDL_CreateYUVOverlay(DEST_W, DEST_H, SDL_IYUV_OVERLAY, sdl_surface);
		}
		webcam->startCapture();
	} else {
		QMessageBox::critical(0, "Webcam test", "No webcam found");
	}
}

int main(int argc, char **argv) {
	QString method(argv[1]);

	if (argc == 2) {
		if (method == "qimage") {
			drawMethod = qimage;
		} else if (method == "qpainter") {
			drawMethod = qpainter;
		} else if (method == "opengl") {
			drawMethod = opengl;
		} else if (method == "scalingonly") {
			drawMethod = scalingonly;
		} else if (method == "sdl") {
			drawMethod = sdl;
			if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
				return EXIT_FAILURE;
			}
		} else {
			cout << "Usage: " << argv[0]
				<< " qimage ou qpainter ou opengl ou scalingonly ou sdl" << endl;
			return EXIT_FAILURE;
		}
	} else {
		drawMethod = none;
	}

	QApplication app(argc, argv);
	avcodec_init();
	if (method.isEmpty()) {
		method = "none";
	}
	WebcamTest *t = new WebcamTest(method);
	app.setMainWidget(t->mainWindow);
	return app.exec();
}
