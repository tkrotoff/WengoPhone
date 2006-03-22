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

#include <directx-pixertool.h>

#include <DirectXWebcamDriver.h>
#include <DirectXWebcamUtils.h>

#include <util/Logger.h>

using namespace std;

#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

DirectXWebcamDriver::DirectXWebcamDriver(WebcamDriver *driver, int flags)
: IWebcamDriver(flags) {
	CoInitialize(NULL);
	_webcamDriver = driver;
}

DirectXWebcamDriver::~DirectXWebcamDriver() {
	stopCapture();
	CoUninitialize();
}

void DirectXWebcamDriver::cleanup() {
	_isOpened = false;
	_cachedWidth = 0;
	_cachedHeight = 0;
	_cachedFPS = 15;
	_cachedPalette = PIX_OSI_UNSUPPORTED;
	_pGrabberF = NULL;
	_pGrabber = NULL;
	_pNull = NULL;
	_iam = NULL;
	SAFE_RELEASE(_pGrabberF);
	SAFE_RELEASE(_pGrabber);
	SAFE_RELEASE(_pNull);
	SAFE_RELEASE(_iam);
	if (_pGraph) {
		_pGraph.Release();
		delete _pGraph;
		_pGraph = NULL;
	}
	if (_pCap) {
		_pCap.Release();
		delete _pCap;
		_pCap = NULL;
	}
	if (_pBuild) {
		_pBuild.Release();
		delete _pBuild;
		_pBuild = NULL;
	}
}

StringList DirectXWebcamDriver::getDeviceList() {
	StringList toReturn;
	string dev_name = "";
	HRESULT hr;

        // create an enumerator
	CComPtr< ICreateDevEnum > pCreateDevEnum;
	pCreateDevEnum.CoCreateInstance( CLSID_SystemDeviceEnum );
	if( !pCreateDevEnum ) {
		return toReturn;
	}

        // enumerate video capture devices
	CComPtr< IEnumMoniker > pEnumMoniker;
	pCreateDevEnum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0 );
	if( !pEnumMoniker ) {
		return toReturn;
	}

	pEnumMoniker->Reset( );
        // go through and find all video capture device(s)
	while(1) {
		CComPtr< IMoniker > pMoniker;
		hr = pEnumMoniker->Next( 1, &pMoniker, 0);
		if( hr != S_OK ) {
			break;
		}

                // get the property bag for this moniker
		CComPtr< IPropertyBag > pPropertyBag;
		hr = pMoniker->BindToStorage( 0, 0, IID_IPropertyBag, (void**) &pPropertyBag );
		if( hr != S_OK ) {
			continue;
		}

                // ask for the english-readable name
		CComVariant FriendlyName;
		CComVariant DevicePath;
		hr = pPropertyBag->Read( L"FriendlyName", &FriendlyName, NULL);
		hr = pPropertyBag->Read( L"DevicePath", &DevicePath, NULL);
		if (hr != S_OK) {
			continue;
		}

		if (((string)_bstr_t(DevicePath)).find("pci") == string::npos) {
			dev_name = (string)_bstr_t(FriendlyName);
			toReturn.add(dev_name);
		}
/* TODO: do we still use this variable? see lib video in classic.
 *
		else if (pci_device) {
			dev_name = (string)_bstr_t(FriendlyName);
			toReturn.add(dev_name);
		}
*/
	}

	return toReturn;
}

string DirectXWebcamDriver::getDefaultDevice() {
	string defaultDevice;
	HRESULT hr;
	CComPtr< IBaseFilter > ppCap;

	// create an enumerator
	CComPtr< ICreateDevEnum > pCreateDevEnum;
	pCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum);

	_ASSERTE(pCreateDevEnum);
	if (!pCreateDevEnum) {
		return WEBCAM_NULL;
	}

	// enumerate video capture devices
	CComPtr< IEnumMoniker > pEm;
	pCreateDevEnum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &pEm, 0 );

	//_ASSERTE(pEm);
	if (!pEm) {
		return WEBCAM_NULL;
	}

	pEm->Reset();

	// go through and find first video capture device
	while (true) {
		ULONG ulFetched = 0;
		CComPtr< IMoniker > pM;

		hr = pEm->Next(1, &pM, &ulFetched);
		if (hr != S_OK) {
			break;
		}

		// get the property bag
		CComPtr< IPropertyBag > pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void**) &pBag );
		if (hr != S_OK) {
			continue;
		}

		// ask for the english-readable name
		CComVariant var;
		var.vt = VT_BSTR;
		hr = pBag->Read(L"FriendlyName", &var, NULL);
		if (hr != S_OK) {
			continue;
		}

		defaultDevice = (const char *)_bstr_t(var);

		// ask for the actual filter
		hr = pM->BindToObject(0, 0, IID_IBaseFilter, (void**) &ppCap);
		if (ppCap) {
			break;
		}
	}

	return defaultDevice;
}

webcamerrorcode DirectXWebcamDriver::setDevice(const std::string & deviceName) {
	//TODO: test i a webcam is already opened

	HRESULT hr;

	_pBuild.CoCreateInstance(CLSID_CaptureGraphBuilder2);
	if (!_pBuild) {
		LOG_ERROR("Failed to create Capture Graph builder");
		return WEBCAM_NOK;
	}

	_pGraph.CoCreateInstance(CLSID_FilterGraph);
	if (!_pGraph) {
		LOG_ERROR("Failed to create Graph builder");
		return WEBCAM_NOK;
	}

	_pBuild->SetFiltergraph(_pGraph);
	//Create the Sample Grabber
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&(_pGrabberF));
	if (hr != S_OK) {
		LOG_ERROR("Failed to create COM instance");
		return WEBCAM_NOK;
	}

	//Add the filter to the graph
	hr = (_pGraph)->AddFilter(_pGrabberF, L"Sample Grabber");
	if (hr != S_OK) {
		LOG_ERROR("Failed to add filter");
		return WEBCAM_NOK;
	}

	//Query the Sample Grabber for the ISampleGrabber interface.
	_pGrabberF->QueryInterface(IID_ISampleGrabber, (void**)&_pGrabber);
	hr = _pGrabber->SetBufferSamples(FALSE);
	hr = _pGrabber->SetOneShot(FALSE);

	//Set the Sample Grabber callback
	// 0: SampleCB (the buffer is the original buffer, not a copy)
	// 1: BufferCB (the buffer is a copy of the original buffer)
	if(_pGrabber->SetCallback(this, 0) != S_OK) {
		LOG_ERROR("Failed to assign callback");
		return WEBCAM_NOK;
	}

	CComBSTR bstrName(deviceName.c_str());
	hr = FindMyCaptureDevice(&_pCap, bstrName);
	if ((hr != S_OK) || !_pCap ) {
		return WEBCAM_NOK;
	}

	// initialize IAMStreamConfig
	_iam = GetIAMStreamConfig(_pCap);
	if (!_iam) {
		return WEBCAM_NOK;
	}

	// add the capture filter to the graph
	hr = (_pGraph)->AddFilter(_pCap, L"");
	if(hr!=S_OK) {
		LOG_ERROR("Failed to add filter");
		return WEBCAM_NOK;
	}

	// Add a null renderer filter
	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&_pNull);
	hr = (_pGraph)->AddFilter(_pNull, L"NullRender");

	_pBuild->RenderStream(&PIN_CATEGORY_CAPTURE, NULL, _pCap, NULL, _pGrabberF);

	// try to assign some palette until the webcam supports it
	if (setCaps(PIX_OSI_YUV420P, _cachedFPS, 176, 144) != WEBCAM_OK ) {
	if (setCaps(PIX_OSI_YUV420P, _cachedFPS, 160, 120) != WEBCAM_OK ) {
	if (setCaps(PIX_OSI_I420, _cachedFPS, 176, 144) != WEBCAM_OK ) {
	if (setCaps(PIX_OSI_I420, _cachedFPS, 160, 120) != WEBCAM_OK ) {
	if (setCaps(PIX_OSI_RGB32, _cachedFPS, 176, 144) != WEBCAM_OK ) {
	if (setCaps(PIX_OSI_RGB32, _cachedFPS, 160, 120) != WEBCAM_OK ) {
	if (setCaps(PIX_OSI_RGB24, _cachedFPS, 176, 144) != WEBCAM_OK ) {
	if (setCaps(PIX_OSI_RGB24, _cachedFPS, 160, 120) != WEBCAM_OK ) {
	if (setCaps(PIX_OSI_YUV422, _cachedFPS, 176, 144) != WEBCAM_OK ) {
	if (setCaps(PIX_OSI_YUV422, _cachedFPS, 160, 120) != WEBCAM_OK ) {
	if (setCaps(PIX_OSI_RGB565, _cachedFPS, 176, 144) != WEBCAM_OK ) {
	if (setCaps(PIX_OSI_RGB565, _cachedFPS, 160, 120) != WEBCAM_OK ) {
	if (setCaps(PIX_OSI_RGB555, _cachedFPS, 176, 144) != WEBCAM_OK ) {
	if (setCaps(PIX_OSI_RGB555, _cachedFPS, 160, 120) != WEBCAM_OK ) {
	}}}}}}}}}}}}}}

	readCaps();

	if (getPalette() == PIX_OSI_UNSUPPORTED) {
		return WEBCAM_NOK;
	}

	_isOpened = true;

	return WEBCAM_OK;
}

bool DirectXWebcamDriver::isOpened() const {
	return _isOpened;
}

void DirectXWebcamDriver::startCapture() {
	HRESULT hr;
	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl = _pGraph;
	hr = pControl->Run();
	if (hr != S_OK) {
		LOG_ERROR("Could not run graph");
		return;
	}
}

void DirectXWebcamDriver::pauseCapture() {
}

void DirectXWebcamDriver::stopCapture() {
	if (!_pGraph) {
		return;
	}

	HRESULT hr;
	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl = _pGraph;
	hr = pControl->Stop();
	if (hr != S_OK) {
		LOG_ERROR("Could not stop capture");
	}
}

webcamerrorcode DirectXWebcamDriver::setPalette(pixosi palette) {
	setCaps(palette, getFPS(), getWidth(), getHeight());
	if (_cachedPalette != palette) {
		return WEBCAM_NOK;
	} else {
		return WEBCAM_OK;
	}
}

pixosi DirectXWebcamDriver::getPalette() const {
	return _cachedPalette;
}

webcamerrorcode DirectXWebcamDriver::setFPS(unsigned fps) {
	setCaps(getPalette(), fps, getWidth(), getHeight());

	//TODO: check if fps is correctly set
	return WEBCAM_NOK;
}

unsigned DirectXWebcamDriver::getFPS() const {
	return _cachedFPS;
}

webcamerrorcode DirectXWebcamDriver::setResolution(unsigned width, unsigned height) {
	setCaps(getPalette(), getFPS(), width, height);
	if ((_cachedWidth != width) || (_cachedHeight != height)) {
		return WEBCAM_NOK;
	} else {
		return WEBCAM_OK;
	}
}


unsigned DirectXWebcamDriver::getWidth() const {
	return _cachedWidth;
}

unsigned DirectXWebcamDriver::getHeight() const {
	return _cachedHeight;
}

void DirectXWebcamDriver::setBrightness(int brightness) {
}

int DirectXWebcamDriver::getBrightness() const {
	return 0;
}

void DirectXWebcamDriver::setContrast(int contrast) {
}

int DirectXWebcamDriver::getContrast() const {
	return 0;
}

void DirectXWebcamDriver::flipHorizontally(bool flip) {
	//TODO: add horizontal flip support
}

webcamerrorcode DirectXWebcamDriver::setCaps(pixosi palette, unsigned fps, unsigned resolutionWidth, unsigned resolutionHeight) {
	HRESULT hr;
	int iCount, iSize;
	VIDEO_STREAM_CONFIG_CAPS scc;
	VIDEOINFOHEADER *pvi;
	pixosi wc_palette;
	AM_MEDIA_TYPE *pmt = NULL;

	_cachedFPS = fps;

	if (!isOpened()) {
		return WEBCAM_NOK;
	}

	assert(_pCap && "webcam not initialized");
	assert(_iam && "webcam not initialized");

	hr = _iam->GetNumberOfCapabilities(&iCount, &iSize);

	if (sizeof(scc) != iSize) {
		LOG_ERROR("wrong config structure");
		return WEBCAM_NOK;
	}

	for (int i = 0; i < iCount; i++) {
		hr = _iam->GetStreamCaps(i, &pmt, reinterpret_cast<BYTE *>(&scc));
		if (hr == S_OK) {
			wc_palette = pix_directx_to_pix_osi(pmt->subtype);
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
				LOG_DEBUG("assigned caps : ("
					+ String::fromNumber(palette)
					+ "," + String::fromNumber(fps)
					+ "," + String::fromNumber(resolutionWidth)
					+ "," + String::fromNumber(resolutionHeight)
					+ ")");
				break;
			}
		}
	}

	readCaps();

	if (hr == S_OK) {
		return WEBCAM_OK;
	}

	LOG_ERROR("failed caps request: ("
		+ String::fromNumber(palette)
		+ "," + String::fromNumber(fps)
		+ "," + String::fromNumber(resolutionWidth)
		+ "," + String::fromNumber(resolutionHeight)
		+ ")");

	return WEBCAM_NOK;
}

void DirectXWebcamDriver::readCaps() {
	HRESULT hr;
	VIDEOINFOHEADER *pvi;
	pixosi palette;

	AM_MEDIA_TYPE *pmt = 0;
	hr = _iam->GetFormat(&pmt);
	if (pmt->formattype == FORMAT_VideoInfo) {
		pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
		palette = pix_directx_to_pix_osi(pmt->subtype);
	}
	_cachedPalette = palette;
	_cachedWidth = pvi->bmiHeader.biWidth;
	_cachedHeight = pvi->bmiHeader.biHeight;
}

STDMETHODIMP DirectXWebcamDriver::QueryInterface(REFIID riid, void ** ppv) {
	LOG_DEBUG("CSampleGrabberCB::QueryInterface");

	if(riid == IID_ISampleGrabberCB || riid == IID_IUnknown) {
		*ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
		return NOERROR;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP DirectXWebcamDriver::BufferCB(double dblSampleTime, BYTE * pBuffer, long lBufferSize) {
	// This method is not used but must be implemented
	LOG_ERROR("This method should not be called");
	return 0;
}

STDMETHODIMP DirectXWebcamDriver::SampleCB(double SampleTime, IMediaSample * pSample) {
	BYTE *pBuffer;
	long lBufferSize;

	if (!pSample) {
		return E_POINTER;
	} else {
		pSample->GetPointer(&pBuffer);
		lBufferSize = pSample->GetSize();

		if (!pBuffer) {
			return E_POINTER;
		}

		_capturedImage.data = (uint8_t *) pBuffer;
		_capturedImage.width = _cachedWidth;
		_capturedImage.height = _cachedHeight;
		_capturedImage.palette = _cachedPalette;

		_webcamDriver->frameBufferAvailable(&_capturedImage);
	}
	return 0;
}
