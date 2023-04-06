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
 * DirectX Webcam class
 *
 * @author Mathieu Stute
 * @author David Ferlier
 * @author Jerome Wagner
 */

#include <webcam/WebcamDirectX.h>
#include <webcam/WebcamDirectXUtils.h>

#include <stdio.h>
#include <iostream>

using namespace std;

#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

// Taken from OUR_idCapture

// I420 isn't defined in any of the headers, but everyone seems to use it.... 
extern "C" const __declspec(selectany) GUID OUR_MEDIASUBTYPE_I420 =
{0x30323449,0x0000,0x0010, {0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71}};

// IYUV
extern "C" const __declspec(selectany) GUID OUR_MEDIASUBTYPE_IYUV =
   {0x56555949,0x0000,0x0010, {0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71}};

// Y444
extern "C" const __declspec(selectany) GUID OUR_MEDIASUBTYPE_Y444 = 
   {0x34343459, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};

// Y800
extern "C" const __declspec(selectany) GUID OUR_MEDIASUBTYPE_Y800 = 
   {0x30303859, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};

// Y422
extern "C" const __declspec(selectany) GUID OUR_MEDIASUBTYPE_Y422 = 
   {0x32323459, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};

// NV12
extern "C" const __declspec(selectany) GUID OUR_MEDIASUBTYPE_NV12 = 
   {0x3231564E, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};

struct mstype_and_palette {
	GUID ms_type;
	int webcam_palette;
};

#define __MSTYPE_ENTRIES	20

struct mstype_and_palette _ms_table[__MSTYPE_ENTRIES] = {
	{ MEDIASUBTYPE_YUYV, WEBCAM_PALETTE_YUYV },
	{ MEDIASUBTYPE_Y411, WEBCAM_PALETTE_YUV411 },
	{ MEDIASUBTYPE_Y41P, WEBCAM_PALETTE_YUV41P },
	{ MEDIASUBTYPE_YUY2, WEBCAM_PALETTE_YUY2 },
	{ MEDIASUBTYPE_YVYU, WEBCAM_PALETTE_YVYU },
	{ MEDIASUBTYPE_UYVY, WEBCAM_PALETTE_UYVY },
	{ MEDIASUBTYPE_YV12, WEBCAM_PALETTE_YV12 },
	{ MEDIASUBTYPE_RGB1, WEBCAM_PALETTE_RGB1 },
	{ MEDIASUBTYPE_RGB4, WEBCAM_PALETTE_RGB4 },
	{ MEDIASUBTYPE_RGB8, WEBCAM_PALETTE_RGB8 },
	{ MEDIASUBTYPE_RGB565, WEBCAM_PALETTE_RGB565 },
	{ MEDIASUBTYPE_RGB555, WEBCAM_PALETTE_RGB555 },
	{ MEDIASUBTYPE_RGB24, WEBCAM_PALETTE_RGB24 },
	{ MEDIASUBTYPE_RGB32, WEBCAM_PALETTE_RGB32 },
	{ MEDIASUBTYPE_ARGB32, WEBCAM_PALETTE_RGB32 },
	{ OUR_MEDIASUBTYPE_I420, WEBCAM_PALETTE_I420 },
	{ OUR_MEDIASUBTYPE_Y422, WEBCAM_PALETTE_YUV422 },
	{ OUR_MEDIASUBTYPE_IYUV, WEBCAM_PALETTE_YUV420P },
	{ OUR_MEDIASUBTYPE_NV12, WEBCAM_PALETTE_NV12 },
};

int MediaSubTypeToWPalette(GUID subtype) {
	int i, val = -1;
	struct mstype_and_palette m;
	for (i = 0; i < __MSTYPE_ENTRIES; i += 1) {
		m = _ms_table[i];
		if (m.ms_type == subtype) {
			return m.webcam_palette;
		}
	}
	return -1;
}

GUID WPaletteToMediaSubType(int palette) {
	int i;
	struct mstype_and_palette m;
	for (i = 0; i < __MSTYPE_ENTRIES; i += 1) {
		m = _ms_table[i];
		if (m.webcam_palette == palette) {
			return m.ms_type;
		}
	}
	return MEDIASUBTYPE_NULL;
}

WebcamDirectX::WebcamDirectX() {
	CoInitialize(NULL);
	cached_palette=WEBCAM_PALETTE_UNSUPPORTED;
	cached_width=0;
	cached_height=0;
	cached_fps=0;
}

int WebcamDirectX::open() {
	HRESULT hr;
	
	_pBuild.CoCreateInstance(CLSID_CaptureGraphBuilder2);
	if(!_pBuild) {
		webcam_dx_error("Failed to create Capture Graph builder");
		return WEBCAM_CODE_NOK;
	}
	
	_pGraph.CoCreateInstance(CLSID_FilterGraph);
	if(!_pGraph) {
		webcam_dx_error("Failed to create Graph builder");
		return WEBCAM_CODE_NOK;
	}
	
	_pBuild->SetFiltergraph(_pGraph);
	//Create the Sample Grabber
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&(_pGrabberF));
	if (hr != S_OK) {
		webcam_dx_error("Failed to create COM instance");
		return WEBCAM_CODE_NOK;
	}
	
	//Add the filter to the graph
	hr = (_pGraph)->AddFilter(_pGrabberF, L"Sample Grabber");
	if (hr != S_OK) {
		webcam_dx_error("Failed to add filter");
		return WEBCAM_CODE_NOK;
	}
	
	//Query the Sample Grabber for the ISampleGrabber interface.
	_pGrabberF->QueryInterface(IID_ISampleGrabber, (void**)&_pGrabber);
	hr = _pGrabber->SetBufferSamples(FALSE);
	hr = _pGrabber->SetOneShot(FALSE);
	
	//Set the Sample Grabber callback
	// 0: SampleCB (the buffer is the original buffer, not a copy)
	// 1: BufferCB (the buffer is a copy of the original buffer)
	if(_pGrabber->SetCallback(this, 0) != S_OK) {
		webcam_dx_error("Failed to assign callback");
		return WEBCAM_CODE_NOK;
	}

	CComBSTR bstrName(getDevice());
	if (!isFlagSet(WEBCAM_FLAG_USE_DEFAULT_DEVICE)) {
		hr = FindMyCaptureDevice(&_pCap, bstrName);
		if ((hr!=S_OK) || !_pCap ) {
			return WEBCAM_CODE_NOK;
		}
	} else {
		GetDefaultCapDevice(&_pCap);
		if (!_pCap) {
			return WEBCAM_CODE_NOK;
		}
	}
	
	// initialize IAMStreamConfig
	_iam = GetIAMStreamConfig(_pCap);
	
	// add the capture filter to the graph
	hr = (_pGraph)->AddFilter(_pCap, L"");
	if(hr!=S_OK) {
		webcam_dx_error("Failed to add filter");
		return WEBCAM_CODE_NOK;
	}
	
	// Add a null renderer filter
	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&_pNull);
	hr = (_pGraph)->AddFilter(_pNull, L"NullRender");
	
	_pBuild->RenderStream(&PIN_CATEGORY_CAPTURE, NULL, _pCap, NULL, _pGrabberF);
	
	// try to assign some palette until the webcam supports it
	if( setCaps(WEBCAM_PALETTE_YUV420P,15, 176,144) != WEBCAM_CODE_OK ) {
	if( setCaps(WEBCAM_PALETTE_YUV420P,15, 160,120) != WEBCAM_CODE_OK ) {
	if( setCaps(WEBCAM_PALETTE_I420,15, 176,144) != WEBCAM_CODE_OK ) {
	if( setCaps(WEBCAM_PALETTE_I420,15, 160,120) != WEBCAM_CODE_OK ) {
	if( setCaps(WEBCAM_PALETTE_RGB32,15, 176,144) != WEBCAM_CODE_OK ) {
	if( setCaps(WEBCAM_PALETTE_RGB32,15, 160,120) != WEBCAM_CODE_OK ) {
	if( setCaps(WEBCAM_PALETTE_RGB24,15, 176,144) != WEBCAM_CODE_OK ) {
	if( setCaps(WEBCAM_PALETTE_RGB24,15, 160,120) != WEBCAM_CODE_OK ) {
	if( setCaps(WEBCAM_PALETTE_YUV422,15, 176,144) != WEBCAM_CODE_OK ) {
	if( setCaps(WEBCAM_PALETTE_YUV422,15, 160,120) != WEBCAM_CODE_OK ) {
	if( setCaps(WEBCAM_PALETTE_RGB565,15, 176,144) != WEBCAM_CODE_OK ) {
	if( setCaps(WEBCAM_PALETTE_RGB565,15, 160,120) != WEBCAM_CODE_OK ) {
	if( setCaps(WEBCAM_PALETTE_RGB555,15, 176,144) != WEBCAM_CODE_OK ) {
	if( setCaps(WEBCAM_PALETTE_RGB555,15, 160,120) != WEBCAM_CODE_OK ) {
	}}}}}}}}}}}}}}
	
	readCaps();
	
	if (getPalette() == WEBCAM_PALETTE_UNSUPPORTED) {
		return WEBCAM_CODE_NOK;
	}
	
	setFlag(WEBCAM_FLAG_OPENED);
	return WEBCAM_CODE_OK;
}

void WebcamDirectX::uninitialize() {
	CoUninitialize();
}

WebcamDirectX::~WebcamDirectX() {
}

void WebcamDirectX::readCaps() {
	HRESULT hr;
	VIDEOINFOHEADER *pvi;
	int palette;
	
	AM_MEDIA_TYPE *pmt=0;
	hr = _iam->GetFormat(&pmt);
	if(pmt->formattype == FORMAT_VideoInfo) {
		pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
		BITMAPINFOHEADER bih = (pvi)->bmiHeader;
		
		const GUID subtype = pmt->subtype;
		palette = MediaSubTypeToWPalette(subtype);
	}
	cached_palette = palette;
	cached_width = pvi->bmiHeader.biWidth;
	cached_height = pvi->bmiHeader.biHeight;
}

int WebcamDirectX::getPalette() const {
	return cached_palette;
}

int WebcamDirectX::getCurrentWidth() const {
	return cached_width;
}

int WebcamDirectX::getCurrentHeight() const {
	return cached_height;
}

void WebcamDirectX::stopCapture() {
	HRESULT hr;
	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl = _pGraph;
    if(pControl) {
        hr = pControl->Stop();
        if(hr != S_OK) {
            webcam_dx_error("Could not stop the graph");
            return;
        }
    } else {
        webcam_dx_error("Graph already closed");
    }
}

void WebcamDirectX::startCapture() {
	HRESULT hr;
	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl = _pGraph;
	hr = pControl->Run();
	if(hr != S_OK) {
		webcam_dx_error("Could not run the graph");
		return;
	}
}

int WebcamDirectX::getFrameSize() {
	return calculateFrameSize(cached_palette, cached_width, cached_height);
}

int WebcamDirectX::setCaps(int palette, int fps, int resolutionWidth, int resolutionHeight) {
	HRESULT hr;
	int iCount, iSize;
	VIDEO_STREAM_CONFIG_CAPS scc;
	VIDEOINFOHEADER *pvi;
	int wc_palette;
	AM_MEDIA_TYPE *pmt=0;
	
	if (palette==WEBCAM_PALETTE_UNSUPPORTED) {
		palette = cached_palette;
	}
	if (palette == WEBCAM_PALETTE_UNSUPPORTED) {
		return E_FAIL;
	}
	if (fps==0) {
		fps = cached_fps;
	}
	if (resolutionWidth==0) {
		resolutionWidth = cached_width;
	}
	if (resolutionHeight==0) {
		resolutionHeight = cached_height;
	}
	
	cached_fps = fps;
	setFpsTimerInter(1000 / fps);
	
	hr = _iam->GetNumberOfCapabilities(&iCount, &iSize);
	
	if (sizeof(scc) != iSize) {
			webcam_dx_error("wrong config structure");
			return WEBCAM_CODE_NOK;
	}
	
	for (int i=0; i<iCount;i++) {
		hr = _iam->GetStreamCaps(i, &pmt, reinterpret_cast<BYTE*>(&scc));
		if (hr == S_OK) {
			wc_palette = MediaSubTypeToWPalette(pmt->subtype);
			if (wc_palette != palette) {
				hr = E_FAIL;
				continue;
			}
			pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
			pvi->bmiHeader.biWidth = resolutionWidth;
			pvi->bmiHeader.biHeight = resolutionHeight;
			pvi->AvgTimePerFrame = (LONGLONG)(10000000. / (double)fps);
			hr = _iam->SetFormat(pmt);
			if( hr != S_OK) {
				hr = E_FAIL;
				continue;
			} else {
				cout << "assigned caps : (" << palette << "," << fps << "," << resolutionWidth << "," << resolutionHeight << ")" << endl;
				break;
			}
		}
	}
	
	readCaps();
	if (hr == S_OK) {
		return WEBCAM_CODE_OK;
	}
	cout << "failed caps request: (" << palette << "," << fps << "," << resolutionWidth << "," << resolutionHeight << ")" << endl;
	return WEBCAM_CODE_NOK;
}

int WebcamDirectX::setResolution(int width, int height) {
	return setCaps(WEBCAM_PALETTE_UNSUPPORTED, 0, width, height);
}

int WebcamDirectX::setPalette(int palette) {
	return setCaps(palette, 0, 0, 0);
}

int WebcamDirectX::setFps(int frames_per_second) {
	return setCaps(WEBCAM_PALETTE_UNSUPPORTED, frames_per_second, 0, 0);
}

void WebcamDirectX::setBrightness(int brightness) {
	return;
}
void WebcamDirectX::setContrast(int contrast) {
	return;
}
void WebcamDirectX::setColour(int colour) {
	return;
}

int WebcamDirectX::dumpAllCaps() {
	HRESULT hr;
	int iCount, iSize;
	int i;
	VIDEO_STREAM_CONFIG_CAPS scc;
	VIDEOINFOHEADER *pvi;
	GUID subtype;
	
	AM_MEDIA_TYPE *pmt=0;
	hr = _iam->GetNumberOfCapabilities(&iCount, &iSize);
	
	if (sizeof(scc) != iSize) {
		// This is not the structure we were expecting.
		return E_FAIL;
	}
	// Get the first format.
	for (i=0; i<iCount;i++) {
		hr = _iam->GetStreamCaps(i, &pmt, reinterpret_cast<BYTE*>(&scc));
		if (hr == S_OK) {
			cout << "------------------- format " << i << endl;
			if ((pmt->formattype == FORMAT_VideoInfo)||(pmt->formattype == FORMAT_VideoInfo2)) {
				pvi = reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat);
				cout << "resolution: " << pvi->bmiHeader.biWidth << "x" << pvi->bmiHeader.biHeight << endl;
				cout << "fps: " << (10000000/pvi->AvgTimePerFrame) << endl;
					
				subtype = pmt->subtype;
				cout << "palette: " << MediaSubTypeToWPalette(subtype) << endl;
				cout << "bitcount: " << pvi->bmiHeader.biBitCount << endl;
				cout << "InputSize: " << scc.InputSize.cx << "x" << scc.InputSize.cy << endl;
				cout << "CropGranularityX: " << scc.CropGranularityX << endl;
				cout << "CropGranularityY: " << scc.CropGranularityY << endl;
				cout << "MinCroppingSize: " << scc.MinCroppingSize.cx << "x" << scc.MinCroppingSize.cy << endl;
				cout << "MaxCroppingSize: " << scc.MaxCroppingSize.cx << "x" << scc.MaxCroppingSize.cy << endl;
				cout << "OutputGranularityX: " << scc.OutputGranularityX << endl;
				cout << "OutputGranularityY: " << scc.OutputGranularityY << endl;
				cout << "MinOutputSize: " << scc.MinOutputSize.cx << "x" << scc.MinOutputSize.cy << endl;
				cout << "MaxOutputSize: " << scc.MaxOutputSize.cx << "x" << scc.MaxOutputSize.cy << endl;
				cout << "fps shape: " << (10000000/scc.MaxFrameInterval) << "," << (10000000/scc.MinFrameInterval) << endl;
				cout << "bitrate shape: " << scc.MinBitsPerSecond << "," << scc.MaxBitsPerSecond << endl;
				cout << "fps shape2: " << (scc.MinBitsPerSecond/(pvi->bmiHeader.biBitCount * pvi->bmiHeader.biWidth * pvi->bmiHeader.biHeight)) << "," << (scc.MaxBitsPerSecond/(pvi->bmiHeader.biBitCount * pvi->bmiHeader.biWidth * pvi->bmiHeader.biHeight)) << endl;
			}
		}
	}
	
	_iam->GetFormat(&pmt);
	cout << "---- format getFormat" << endl;
	if (pmt->formattype == FORMAT_VideoInfo) {
		pvi = reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat);
		cout << "resolution: " << pvi->bmiHeader.biWidth << "x" << pvi->bmiHeader.biHeight << endl;
		cout << "fps: " << (10000000/pvi->AvgTimePerFrame) << endl;
			
		subtype = pmt->subtype;
		cout << "palette: " << MediaSubTypeToWPalette(subtype) << endl;
	}
}

void WebcamDirectX::close() {
	stopCapture();
	SAFE_RELEASE(_pGrabberF);
	SAFE_RELEASE(_pGrabber);
	SAFE_RELEASE(_pNull);
	SAFE_RELEASE(_iam);
	if(_pGraph) {
		_pGraph.Release();
		delete _pGraph;
		_pGraph = NULL;
	}
	if(_pCap) {
		_pCap.Release();
		delete _pCap;
		_pCap = NULL;
	}
	if(_pBuild) {
		_pBuild.Release();
		delete _pBuild;
		_pBuild = NULL;
	}
}

STDMETHODIMP WebcamDirectX::QueryInterface(REFIID riid, void ** ppv) {
	webcam_dx_log("CSampleGrabberCB::QueryInterface");
	
	if(riid == IID_ISampleGrabberCB || riid == IID_IUnknown) {
		*ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
		return NOERROR;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP WebcamDirectX::BufferCB(double dblSampleTime, BYTE * pBuffer, long lBufferSize) {
	uint8_t *_alignedBuf;

	if (!pBuffer) {
		return E_POINTER;
	}
	else {
		frameCopyAvailable((uint8_t *)pBuffer, cached_width, cached_height, cached_palette, lBufferSize);
	}
	return 0;
}

STDMETHODIMP WebcamDirectX::SampleCB( double SampleTime, IMediaSample * pSample ) {
	BYTE *pBuffer;
	long lBufferSize;
	if (!pSample) {
		return E_POINTER;
	}
	else {
		pSample->GetPointer(&pBuffer);
		lBufferSize = pSample->GetSize();

		if (!pBuffer) {
			return E_POINTER;
		}
		
		frameBufferAvailable((uint8_t *)pBuffer, cached_width, cached_height, cached_palette, lBufferSize);
	}
	return 0;
	
}
