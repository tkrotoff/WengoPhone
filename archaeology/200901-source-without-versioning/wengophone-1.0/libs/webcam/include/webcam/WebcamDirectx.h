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

#ifndef WEBCAM_DIRECTX_H
#define WEBCAM_DIRECTX_H

/**
 * DirectX Webcam class
 *
 * @author Mathieu Stute
 */

#include <webcam/webcam.h>

#include <dshow.h>
#include <atlbase.h>
#include <qedit.h>
#include <comutil.h>

class Webcam;

class WEBCAM_DLLEXPORT WebcamDirectX : public Webcam , public ISampleGrabberCB {
public:
	WebcamDirectX();
	virtual ~WebcamDirectX();
	
	void stopCapture();
	virtual void startCapture();
	void close();
	int  open();
	void uninitialize();
	
	void readCaps();
	int setResolution(int width, int height);
	void setBrightness(int);
	void setContrast(int);
	void setColour(int);

	int setPalette(int palette);
	int setCaps(int palette, int fps, int resolutionWidth, int resolutionHeight);
	
	virtual int getCurrentWidth() const;
	virtual int getCurrentHeight() const;
	virtual int getPalette() const;
	int setFps(int);
	int getFrameSize();
	int dumpAllCaps();
	
	STDMETHODIMP_(ULONG) AddRef() { return 2; }
	STDMETHODIMP_(ULONG) Release() { return 1; }
	
	// fake out any COM QI'ing
	STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);
	
	// callback to access the buffer - a copy is passed
	STDMETHODIMP BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize );
	
	// callback to access the buffer - the original buffer is passed
	STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample );
	
private:
	CComPtr< IGraphBuilder > _pGraph;
	CComPtr< ICaptureGraphBuilder2 > _pBuild;
	CComPtr< IBaseFilter > _pCap;
	IBaseFilter * _pGrabberF;
	ISampleGrabber * _pGrabber;
	IBaseFilter * _pNull;
	IAMStreamConfig * _iam;
	
	int cached_width;
	int cached_height;
	int cached_palette;
	int cached_fps;
};
#endif
