
#include <webcam/webcam.h>
#include <string.h>
#include <time.h>

#if defined(WIN32)
#include <windows.h>
#else
#include <time.h>
#endif
//#define BMP_CHECK


// temporary
int _captured = 0;
int fpsTimerLast_fps;

#ifdef BMP_CHECK
bool copyBitmap(BYTE * pBuffer, long lBufferSize, int width, int height, int depth);
#endif

Webcam::Webcam() {
        lastFrame = NULL;
        flags = opened = 0;
}
Webcam::~Webcam() {
	if (lastFrame) {
		av_free(lastFrame);
	}
}

void Webcam::frameBufferAvailable(uint8_t *buf_frame, int width_frame, int height_frame, int pix_osi_frame, int len_frame) {
    unsigned long now;
	uint8_t *buf_copy;
	
#ifdef WIN32
    now = timeGetTime();
#else
    now = time(NULL)*1000;
#endif

#ifdef WIN32
	if ((now - fpsTimerLast_fps) > 5000) {
		printf("_____ captured fps %d\n", (_captured / 5));
		fpsTimerLast_fps = now;
		_captured = 0;
	}
	if ((now - fpsTimerLast) < fpsTimerInter) {
		return;
	}
#endif
	_captured += 1;
	fpsTimerLast = now;
	
	
	//memcpy(lastFrame, frame, size);
	//buf_copy = (uint8_t*) av_malloc(len_frame * sizeof(uint8_t));
	//memcpy(buf_copy, buf_frame, len_frame);
	webcamCallbackFrameReady(getUserData(), buf_frame, width_frame, height_frame, pix_osi_frame, len_frame);

}


void Webcam::frameCopyAvailable(uint8_t *buf_frame, int width_frame, int height_frame, int pix_osi_frame, int len_frame) {
    //lastFrame = getLastFrame();
    unsigned long now;
    uint8_t *buf_copy;

	/*
    if (!lastFrame) {
        lastFrame = (uint8_t *) av_malloc (size * sizeof(uint8_t));
    }
	*/

#ifdef BMP_CHECK
	int palette = getPalette();
	int depth = 0;
	if (palette == WEBCAM_PALETTE_RGB24) {
		depth = 24;
	} else if (palette == WEBCAM_PALETTE_RGB555) {
		depth = 16;
	} else if (palette == WEBCAM_PALETTE_RGB565) {
		depth = 16;
	} else if (palette == WEBCAM_PALETTE_RGB32) {
		depth = 32;
	}
	if(depth) {
		copyBitmap((BYTE*)buf_frame, len_frame, width_frame, height_frame, depth);
	}
	//printf("width:%i\theight:%i\tdepth:%i\n", getCurrentWidth(), getCurrentHeight(), depth);
#endif
	
#ifdef WIN32
    now = timeGetTime();
#else
    now = time(NULL)*1000;
#endif

#ifdef WIN32
	if ((now - fpsTimerLast_fps) > 5000) {
		printf("_____ captured fps %d\n", (_captured / 5));
		fpsTimerLast_fps = now;
		_captured = 0;
	}
	if ((now - fpsTimerLast) < fpsTimerInter) {
		return;
	}
#endif
	_captured += 1;
	fpsTimerLast = now;


	buf_copy = (uint8_t*) av_malloc(len_frame * sizeof(uint8_t));
	memcpy(buf_copy, buf_frame, len_frame);
	
	//memcpy(lastFrame, frame, size);
	webcamCallbackFrameReady(getUserData(), buf_copy, width_frame, height_frame, pix_osi_frame, len_frame);
}

int Webcam::calculateFrameSize(int palette, int width, int height) {
	
	int pix_fmt;
	int framesize;
	pix_fmt = webcam_palette_to_avcodec_pixfmt(palette);
	framesize = avpicture_get_size(pix_fmt, width, height);
	return framesize;
	
}

int Webcam::webcam_palette_to_avcodec_pixfmt(int format) {
    int palette = -1;
    if (format == WEBCAM_PALETTE_RGB24) {
        palette = PIX_FMT_BGR24;
    } else if (format == WEBCAM_PALETTE_RGB555) {
        palette = PIX_FMT_RGB555;
    } else if (format == WEBCAM_PALETTE_RGB565) {
        palette = PIX_FMT_RGB565;
    } else if (format == WEBCAM_PALETTE_RGB32) {
        palette = PIX_FMT_RGBA32;
    } else if (format == WEBCAM_PALETTE_YUV420P ||
			format == WEBCAM_PALETTE_I420 ||
			format == WEBCAM_PALETTE_NV12) {
        palette = PIX_FMT_YUV420P;
    } else if (format == WEBCAM_PALETTE_YUV422P) {
        palette = PIX_FMT_YUV422P;
    } else if (format == WEBCAM_PALETTE_YUY2) {
        palette = PIX_FMT_YUV422;
    }
    return palette;
}



uint8_t * Webcam::convertImage(uint8_t *buf, int width, int height, int pix_osi_source, int pix_osi_target) {
	AVPicture avp_source, avp_target;
	uint8_t *buf_source;
	uint8_t *buf_target;
	int len_target;
	int pix_fmt_source = PIX_FMT_YUV420P;
	int pix_fmt_target = PIX_FMT_YUV420P;
	int need_avfree = 0;

	buf_source = buf;

    if (!buf) {
	    return NULL;
    }

	
	if (pix_osi_source == WEBCAM_PALETTE_NV12) {
		buf_source = nv12_to_yuv420p(buf_source, width, height);
		need_avfree = 1;
		pix_osi_source = WEBCAM_PALETTE_YUV420P;
	}
		
    pix_fmt_source = webcam_palette_to_avcodec_pixfmt(pix_osi_source);
    pix_fmt_target = webcam_palette_to_avcodec_pixfmt(pix_osi_target);

    len_target = avpicture_get_size(pix_fmt_target, width, height);
	buf_target = (uint8_t *) av_malloc (len_target * sizeof(uint8_t));

	avpicture_fill(&avp_source,  (uint8_t *)buf_source, pix_fmt_source, width, height);
	avpicture_fill(&avp_target, (uint8_t *)buf_target, pix_fmt_target, width, height);

	
	if (isFlagSet(WEBCAM_FLAG_HORIZONTAL_FLIP)) {
		avp_source.data[0] += avp_source.linesize[0] * (height-1);
		avp_source.linesize[0] *= -1;

		//Only flip other planes if the destination palette is YUV420
		//FIXME DUDE

		if (pix_fmt_source == PIX_FMT_YUV420P) {
			avp_source.data[1] += avp_source.linesize[1] * (height/2-1);
			avp_source.linesize[1] *= -1;
			avp_source.data[2] += avp_source.linesize[2] * (height/2-1);
			avp_source.linesize[2] *= -1;
		}
	}
	
	img_convert(
				&avp_target, pix_fmt_target,
				&avp_source, pix_fmt_source,
				width, height
				);
	
	if (need_avfree) {
		av_free(buf_source);
	}
	
	return buf_target;
}

uint8_t * Webcam::nv12_to_yuv420p(uint8_t* buf, int width, int height) {
	uint8_t *buf_source;
	uint8_t *buf_target;
	int len_target;
	
	buf_source = buf;
	len_target = (width*height*3)/2;
	buf_target = (uint8_t*) av_malloc(len_target*sizeof(uint8_t));
	
	memcpy(buf_target, buf_source, width*height);
	for (int i=0; i<(width*height/4);i++) {
		buf_target[(width*height)+i] = buf_source[(width*height)+2*i];
		buf_target[(width*height)+(width*height/4)+i] = buf_source[(width*height)+2*i+1];
	}
	
	return buf_target;
}


int Webcam::getCurrentWidth() const {
    return 0;
}

int Webcam::getCurrentHeight() const {
    return 0;
}


int Webcam::getPalette() const {
    return 0;
}

void Webcam::startCapture() {
}

/*
void Webcam::setFps(int frames_per_second) {
    fps = frames_per_second;
    fpsTimerInter = 1000 / fps;
}
*/

void Webcam::setBrightness(int) {
}

void Webcam::setContrast(int) {
}

void Webcam::setColour(int) {
}

int Webcam::setPalette(int) {
    return 0;
}

int Webcam::setCaps(int,int,int,int) {
    return 0;
}

int Webcam::setResolution(int width, int height) {
    return 0;
}

#ifdef BMP_CHECK
bool copyBitmap(BYTE * pBuffer, long lBufferSize, int width, int height, int depth) {
    if(!pBuffer) {
        return 0;
    }
    
    // write out a BMP file
    HANDLE hf = CreateFile("test.bmp", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
    if(hf == INVALID_HANDLE_VALUE) {
        return 0;
    }
    
    // write out the file header
    BITMAPFILEHEADER bfh;
    memset(&bfh, 0, sizeof(bfh));
    bfh.bfType = 'MB';
    bfh.bfSize = sizeof(bfh) + lBufferSize + sizeof(BITMAPINFOHEADER);
    bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
    
    DWORD dwWritten = 0;
    WriteFile(hf, &bfh, sizeof(bfh), &dwWritten, NULL);
    
    // and the bitmap format
    BITMAPINFOHEADER bih;
    memset(&bih, 0, sizeof(bih));
    bih.biSize = sizeof(bih);
    bih.biWidth = width;
    bih.biHeight = height;
    bih.biPlanes = 1;
    bih.biBitCount = depth;
    
    dwWritten = 0;
    WriteFile(hf, &bih, sizeof(bih), &dwWritten, NULL);
    
    // and the bits themselves
    dwWritten = 0;
    WriteFile(hf, pBuffer, lBufferSize, &dwWritten, NULL);
    CloseHandle(hf);
    return 0;
}
#endif
