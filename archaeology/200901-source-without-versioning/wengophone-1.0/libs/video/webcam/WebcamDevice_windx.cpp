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

#include "WebcamDevice.h"
#include <windows.h>
#include <dshow.h>
#include <atlbase.h>
#include <comutil.h> // for _bstr_t

#include <iostream>
using namespace std;

/**
 * @author Mathieu Stute
 */

WebcamDevice::WebcamDevice() {
}

WebcamDevice::~WebcamDevice() {
}

QStringList WebcamDevice::getWebcamList(bool pci_device) {
	QStringList toReturn;
	QString dev_name = "";
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
		if( hr != S_OK ) {
			continue;
		}

		if(!((QString)_bstr_t(DevicePath)).contains("pci", false)) {
			dev_name = (QString)_bstr_t(FriendlyName);
			toReturn.prepend(dev_name);
		}
		else {
			dev_name = (QString)_bstr_t(FriendlyName);
			toReturn.append(dev_name);
		}
	}
	return toReturn;
}

QString WebcamDevice::getDefaultDevice() {
	QStringList temp = getWebcamList();
	if(temp.count() > 0) {
		return temp.first();
	}
	else {
		return "";
	}
}
