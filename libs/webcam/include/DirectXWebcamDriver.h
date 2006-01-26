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
#ifndef DIRECTXWEBCAMDRIVER_H
#define DIRECTXWEBCAMDRIVER_H

#include <IWebcamDriver.h>
#include <WebcamDriver.h>

#include <dshow.h>
#include <atlbase.h>
#include <qedit.h>
#include <comutil.h>

#include <iostream>

/**
 * DirectX implementation of webcam driver
 *
 * @author Mathieu Stute
 * @author David Ferlier
 * @author Jerome Wagner
 * @author Philippe BERNERY
 */
class DirectXWebcamDriver : public IWebcamDriver, public ISampleGrabberCB {
public:

	DirectXWebcamDriver(WebcamDriver *driver, int flags);

	virtual ~DirectXWebcamDriver();

	void cleanup();

	void setFlags(int flags) {}

	void unsetFlags(int flags) {}

	bool isFlagSet(int flag) { return false;}

	StringList getDeviceList();

	std::string getDefaultDevice();

	webcamerrorcode setDevice(const std::string & deviceName);

	bool isOpened() const;

	void startCapture();

	void pauseCapture();

	void stopCapture();

	webcamerrorcode setPalette(pixosi palette);

	pixosi getPalette() const;

	webcamerrorcode setFPS(unsigned fps);

	unsigned getFPS() const;

	webcamerrorcode setResolution(unsigned width, unsigned height);

	unsigned getWidth() const;

	unsigned getHeight() const;

	void setBrightness(int brightness);

	int getBrightness() const;

	void setContrast(int contrast);

	int getContrast() const;

	void flipHorizontally(bool flip);


	STDMETHODIMP_(ULONG) AddRef() { return 2; }
	STDMETHODIMP_(ULONG) Release() { return 1; }

	// fake out any COM QI'ing
	STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);

	// callback to access the buffer - the original buffer is passed
	STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample );

	// callback to access the buffer - a copy is passed
	STDMETHODIMP BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize );

private:

	/** Pointer to main driver */
	WebcamDriver * _webcamDriver;

	CComPtr< IGraphBuilder > _pGraph;
	CComPtr< ICaptureGraphBuilder2 > _pBuild;
	CComPtr< IBaseFilter > _pCap;
	IBaseFilter * _pGrabberF;
	ISampleGrabber * _pGrabber;
	IBaseFilter * _pNull;
	IAMStreamConfig * _iam;

	/** Captured image width */
	unsigned _cachedWidth;

	/** Captured image height */
	unsigned _cachedHeight;

	/** Frame per seconds */
	unsigned _cachedFPS;

	/** Desired image palette */
	pixosi _cachedPalette;

	/** Piximage sended to callbacks */
	piximage _capturedImage;
	
	/** True if webcam is opened */
	bool _isOpened;

	void webcam_dx_error(const char *error) {
		std::cerr << "!!DirectX Webcam Driver: " << error << std::endl;
	}

	void webcam_dx_log(const char *mess) {
		std::cout << "**DirectX Webcam Driver: " << mess << std::endl;
	}

	/** Read device capabilities */
	void readCaps();

	/** Set device capabilites */
	webcamerrorcode setCaps(pixosi palette, unsigned fps, unsigned resolutionWidth, unsigned resolutionHeight);
};

#endif //DIRECTXWEBCAMDRIVER_H
