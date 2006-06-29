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

#include <webcam/DirectXWebcamUtils.h>

#include <iostream>
using namespace std;

HRESULT FindMyCaptureDevice(IBaseFilter * * pF, BSTR bstrName) {
	HRESULT hr = E_FAIL;
	HRESULT hr_work = E_FAIL;
	CComPtr < IBaseFilter > pFilter;
	CComPtr < ICreateDevEnum > pSysDevEnum;
	CComPtr < IEnumMoniker > pEnumCat = NULL;

	// Create the System Device Enumerator.
	pSysDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum);
	if (!pSysDevEnum) {
		return E_FAIL;
	}

	// Obtain a class enumerator for the video compressor category.
	pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, & pEnumCat, 0);
	if (!pEnumCat) {
		return E_FAIL;
	}

	pEnumCat->Reset();

	// Enumerate the monikers.
	//CComPtr<IMoniker> pMoniker;

	while (true) {
		//while(pMoniker && pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		CComPtr < IMoniker > pMoniker;
		ULONG cFetched;
		CComPtr < IPropertyBag > pProp;

		hr_work = pEnumCat->Next(1, & pMoniker, & cFetched);
		if (hr_work != S_OK) {
			break;
		}

		hr = pMoniker->BindToStorage(0, 0,
		IID_IPropertyBag, (void * *) & pProp);

		if (hr != S_OK) {
			continue;
		}

		VARIANT varName;
		VariantInit(& varName); // Try to match the friendly name.
		hr = pProp->Read(L"FriendlyName", & varName, 0);
		if (SUCCEEDED(hr) && (wcscmp(bstrName, varName.bstrVal) == 0)) {
			hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void * *) & pFilter);
			break;
		}
		VariantClear(& varName);
		pMoniker = NULL; // Release for the next loop.
	}
	if (pFilter) {
		* pF = pFilter;
		(* pF)->AddRef(); // Add ref on the way out.
	}
	return hr;
}

CComPtr < IBaseFilter > GetCaptureDevice(char * device_name) {
	CComPtr < IBaseFilter > ppCap = NULL;
	//CComPtr< IBaseFilter > pCap;
	HRESULT hr;

	// create an enumerator
	CComPtr < ICreateDevEnum > pCreateDevEnum;
	pCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum);
	if (!pCreateDevEnum) {
		return NULL;
	}

	// enumerate video capture devices
	CComPtr < IEnumMoniker > pEm;
	pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, & pEm, 0);
	if (!pEm) {
		return NULL;
	}

	pEm->Reset();

	// go through and find first video capture device
	while (true) {
		ULONG ulFetched = 0;
		CComPtr < IMoniker > pM;

		hr = pEm->Next(1, & pM, & ulFetched);
		if (hr != S_OK)
			break;

		// get the property bag
		CComPtr < IPropertyBag > pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void * *) & pBag);
		if (hr != S_OK) {
			continue;
		}

		// ask for the english-readable name
		CComVariant var;
		var.vt = VT_BSTR;
		hr = pBag->Read(L"FriendlyName", & var, NULL);
		if (hr != S_OK) {
			continue;
		}

		//QString temp = (QString)_bstr_t(var);
		char * temp = (char *) _bstr_t(var);
		if (temp == device_name) {
			hr = pM->BindToObject(0, 0, IID_IBaseFilter, (void * *) & ppCap);
			if (ppCap) {
				break;
			}
		}
	}
	return ppCap;
}

/*
 * Takes a base filter and try to return a pointer to its IAMStreamConfig
 * return a NULL pointer in the case it fails
 */
IAMStreamConfig * GetIAMStreamConfig(IBaseFilter * pFilter) {
	IAMStreamConfig * pIAMS = NULL;
	IEnumPins * pEnum = 0;
	IPin * pPin = 0;
	HRESULT hr = pFilter->EnumPins(& pEnum);
	if (FAILED(hr)) {
		return NULL;
	}

	while (pEnum->Next(1, & pPin, NULL) == S_OK) {
		hr = pPin->QueryInterface(IID_IAMStreamConfig, (void * *) & pIAMS);
		if (hr == S_OK) {
			return pIAMS;
		}
		pPin->Release();
	}
	pEnum->Release();
	return NULL;
}

HRESULT ConnectFilters(IGraphBuilder * pGraph, IPin * pOut, IBaseFilter * pDest) {
	if ((pGraph == NULL) || (pOut == NULL) || (pDest == NULL)) {
		return E_POINTER;
	}
#ifdef debug
	PIN_DIRECTION PinDir;
	pOut->QueryDirection(& PinDir);
	_ASSERTE(PinDir == PINDIR_OUTPUT);
#endif

	// Find an input pin on the downstream filter.
	IPin * pIn = 0;
	HRESULT hr = GetUnconnectedPin(pDest, PINDIR_INPUT, & pIn);
	if (FAILED(hr)) {
		return hr;
	}
	// Try to connect them.
	hr = pGraph->Connect(pOut, pIn);
	pIn->Release();
	return hr;
}

HRESULT ConnectFilters(IGraphBuilder * pGraph, IBaseFilter * pSrc, IBaseFilter * pDest) {
	if ((pGraph == NULL) || (pSrc == NULL) || (pDest == NULL)) {
		return E_POINTER;
	}

	// Find an output pin on the first filter.
	IPin * pOut = 0;
	HRESULT hr = GetUnconnectedPin(pSrc, PINDIR_OUTPUT, & pOut);
	if (FAILED(hr)) {
		return hr;
	}

	hr = ConnectFilters(pGraph, pOut, pDest);
	pOut->Release();
	return hr;
}

/*
 * GetUnconnectedPin
 *
 * Get filter's unconnected pin if any
 */
HRESULT GetUnconnectedPin(IBaseFilter * pFilter, PIN_DIRECTION PinDir, IPin * * ppPin) {
	* ppPin = 0;
	IEnumPins * pEnum = 0;
	IPin * pPin = 0;
	HRESULT hr = pFilter->EnumPins(& pEnum);
	if (FAILED(hr)) {
		return hr;
	}
	while (pEnum->Next(1, & pPin, NULL) == S_OK) {
		PIN_DIRECTION ThisPinDir;
		pPin->QueryDirection(& ThisPinDir);
		if (ThisPinDir == PinDir) {
			IPin * pTmp = 0;
			hr = pPin->ConnectedTo(& pTmp);
			// Already connected, not the pin we want.
			if (SUCCEEDED(hr)) {
				pTmp->Release();
			}
			// Unconnected, this is the pin we want.
			else {
				pEnum->Release();
				* ppPin = pPin;
				return S_OK;
			}
		}
		pPin->Release();
	}
	pEnum->Release();
	// Did not find a matching pin.
	return E_FAIL;
}

void GetDefaultCapDevice(IBaseFilter * * ppCap) {
	HRESULT hr;

	_ASSERTE(ppCap);
	if (!ppCap) {
		return;
	}

	* ppCap = NULL;

	// create an enumerator
	CComPtr < ICreateDevEnum > pCreateDevEnum;
	pCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum);

	_ASSERTE(pCreateDevEnum);
	if (!pCreateDevEnum) {
		return;
	}

	// enumerate video capture devices
	CComPtr < IEnumMoniker > pEm;
	pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, & pEm, 0);

	//_ASSERTE(pEm);
	if (!pEm) {
		return;
	}

	pEm->Reset();

	// go through and find first video capture device
	while (true) {
		ULONG ulFetched = 0;
		CComPtr < IMoniker > pM;

		hr = pEm->Next(1, & pM, & ulFetched);
		if (hr != S_OK) {
			break;
		}

		// get the property bag
		CComPtr < IPropertyBag > pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void * *) & pBag);
		if (hr != S_OK) {
			continue;
		}

		// ask for the english-readable name
		CComVariant var;
		var.vt = VT_BSTR;
		hr = pBag->Read(L"FriendlyName", & var, NULL);
		if (hr != S_OK) {
			continue;
		}

		// ask for the actual filter
		hr = pM->BindToObject(0, 0, IID_IBaseFilter, (void * *) ppCap);
		if (* ppCap) {
			break;
		}
	}
}
