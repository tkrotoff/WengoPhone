/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2005  Wengo
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

/*
 * @author David Ferlier (david.ferlier@wengo.fr)
 *
 */


#include <yuvoverlay/DirectXOverlay.h>
#include <iostream>
#include <stdio.h>

using namespace std;

HRESULT (WINAPI *DDrawCreate)( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );

DirectXOverlay * overlayObj;

LRESULT CALLBACK MainWndProc(
    HWND hwnd,        // handle to window
    UINT uMsg,        // message identifier
    WPARAM wParam,    // first message parameter
    LPARAM lParam)    // second message parameter
{ 

	RECT rectClient;
	RECT rectOverlay;
	CREATESTRUCT cStruct;

    switch (uMsg) 
    { 
		case WM_DISPLAYCHANGE:
			break;

        case WM_CREATE: 
            return 0; 
 
        case WM_SIZE: 
		//overlayObj->updateOverlaySize((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
            return 0; 
 
        case WM_DESTROY: 
            return 0; 

	case WM_MOVE:
		if (!overlayObj || !overlayObj->dd_surface_overlay) {
			break;
		}

		//overlayObj->updateOverlayPosition((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
		return 0;
 
        default: 
            return DefWindowProc(hwnd, uMsg, wParam, lParam); 
	}
	
	return 0;
}


void SetDDerror(const char *function, int code)
{
	static char *error;
	static char  errbuf[1024];

	errbuf[0] = 0;
	switch (code) {
		case DDERR_GENERIC:
			error = "Undefined error!";
			break;
		case DDERR_EXCEPTION:
			error = "Exception encountered";
			break;
		case DDERR_INVALIDOBJECT:
			error = "Invalid object";
			break;
		case DDERR_INVALIDPARAMS:
			error = "Invalid parameters";
			break;
		case DDERR_NOTFOUND:
			error = "Object not found";
			break;
		case DDERR_INVALIDRECT:
			error = "Invalid rectangle";
			break;
		case DDERR_INVALIDCAPS:
			error = "Invalid caps member";
			break;
		case DDERR_INVALIDPIXELFORMAT:
			error = "Invalid pixel format";
			break;
		case DDERR_OUTOFMEMORY:
			error = "Out of memory";
			break;
		case DDERR_OUTOFVIDEOMEMORY:
			error = "Out of video memory";
			break;
		case DDERR_SURFACEBUSY:
			error = "Surface busy";
			break;
		case DDERR_SURFACELOST:
			error = "Surface was lost";
			break;
		case DDERR_WASSTILLDRAWING:
			error = "DirectDraw is still drawing";
			break;
		case DDERR_INVALIDSURFACETYPE:
			error = "Invalid surface type";
			break;
		case DDERR_NOEXCLUSIVEMODE:
			error = "Not in exclusive access mode";
			break;
		case DDERR_NOPALETTEATTACHED:
			error = "No palette attached";
			break;
		case DDERR_NOPALETTEHW:
			error = "No palette hardware";
			break;
		case DDERR_NOT8BITCOLOR:
			error = "Not 8-bit color";
			break;
		case DDERR_EXCLUSIVEMODEALREADYSET:
			error = "Exclusive mode was already set";
			break;
		case DDERR_HWNDALREADYSET:
			error = "Window handle already set";
			break;
		case DDERR_HWNDSUBCLASSED:
			error = "Window handle is subclassed";
			break;
		case DDERR_NOBLTHW:
			error = "No blit hardware";
			break;
		case DDERR_IMPLICITLYCREATED:
			error = "Surface was implicitly created";
			break;
		case DDERR_INCOMPATIBLEPRIMARY:
			error = "Incompatible primary surface";
			break;
		case DDERR_NOCOOPERATIVELEVELSET:
			error = "No cooperative level set";
			break;
		case DDERR_NODIRECTDRAWHW:
			error = "No DirectDraw hardware";
			break;
		case DDERR_NOEMULATION:
			error = "No emulation available";
			break;
		case DDERR_NOFLIPHW:
			error = "No flip hardware";
			break;
		case DDERR_NOTFLIPPABLE:
			error = "Surface not flippable";
			break;
		case DDERR_PRIMARYSURFACEALREADYEXISTS:
			error = "Primary surface already exists";
			break;
		case DDERR_UNSUPPORTEDMODE:
			error = "Unsupported mode";
			break;
		case DDERR_WRONGMODE:
			error = "Surface created in different mode";
			break;
		case DDERR_UNSUPPORTED:
			error = "Operation not supported";
			break;
		case E_NOINTERFACE:
			error = "Interface not present";
			break;
		default:
			sprintf(errbuf, "%s: Unknown DirectDraw error: 0x%x",
								function, code);
			break;
	}
	if ( ! errbuf[0] ) {
		sprintf(errbuf, "%s: %s", function, error);
	}
	printf("Error: %s\n", errbuf);
	return;
}

static char *PrintFOURCC(int code)
{
	static char buf[5];

	buf[3] = code >> 24;
	buf[2] = (code >> 16) & 0xFF;
	buf[1] = (code >> 8) & 0xFF;
	buf[0] = (code & 0xFF);
	return(buf);
}


DirectXOverlay::DirectXOverlay(int width, int height, int format) {
	DWORD err;

    hInst = (HINSTANCE) GetModuleHandle(NULL);
	DDrawDLL = LoadLibrary(TEXT("DDRAW.DLL"));
	DDrawCreate = (HRESULT (WINAPI *)( GUID FAR *, LPDIRECTDRAW FAR *, IUnknown FAR *)) GetProcAddress(DDrawDLL, TEXT("DirectDrawCreate"));

	WNDCLASS wc; 
 
    wc.style = CS_HREDRAW | CS_VREDRAW; 
    wc.lpfnWndProc = (WNDPROC) MainWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = hInst; 
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); 
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); 
    wc.hbrBackground = NULL;
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = "overlay"; 

	dd_surface_overlay = NULL;
 
    RegisterClass(&wc);

	_yuv_data = (struct private_yuvdata *)malloc(
			sizeof(struct private_yuvdata));

	setWidth(width);
	setHeight(height);

	showFlags = DDOVER_SHOW;
	m_WinWidth = 0;
	m_WinHeight = 0;
	m_xWinPos = 0;
	m_yWinPos = 0;


	overlayObj = this;
}

int DirectXOverlay::create() {
	HRESULT result;
	DDCAPS caps;

	int bHasOverlay, bHasColorKey, bCanStretch;

	result = DDrawCreate(NULL, &lpDraw, NULL);

	if (result != DD_OK) {
		return -1;
	}

	result = lpDraw->QueryInterface(IID_IDirectDraw2, (LPVOID *)&lpDraw2);
	lpDraw->Release();

	if (result != DD_OK) {
		return -1;
	}

	caps.dwSize = sizeof(caps);
	result = lpDraw2->GetCaps(&caps, NULL);

	bHasOverlay = ((caps.dwCaps & DDCAPS_OVERLAY) == DDCAPS_OVERLAY) ? 1 : 0;
	bHasColorKey = ((caps.dwCaps & DDCAPS_COLORKEY) == DDCAPS_COLORKEY) ? 1 : 0;
	bCanStretch = ((caps.dwCaps & DDCAPS_OVERLAYSTRETCH) == DDCAPS_OVERLAYSTRETCH) ? 1 : 0;


    window_id = CreateWindow("overlay", "overlay",
            (WS_OVERLAPPEDWINDOW),
            //(WS_OVERLAPPED|WS_POPUP),
            CW_USEDEFAULT, CW_USEDEFAULT, getWidth(), getHeight(),
		(HWND) getParentWindowId(), NULL, hInst, (LPVOID) this);

	if (window_id == NULL) {
		return -1;
	}

	window_id = (HWND) getParentWindowId();

	result = lpDraw2->SetCooperativeLevel((HWND) getParentWindowId(), DDSCL_NORMAL);

	if (result != DD_OK) {
		SetDDerror("SetCooperative", result);
		return -1;
	}

#define DEBUG_YUV
#ifdef DEBUG_YUV
	DWORD numcodes;
	DWORD *codes;

	IDirectDraw2_GetFourCCCodes(lpDraw2, &numcodes, NULL);
	if ( numcodes ) {
		DWORD i;
		codes = (DWORD *) malloc(numcodes*sizeof(*codes));
		if ( codes ) {
			IDirectDraw2_GetFourCCCodes(lpDraw2, &numcodes, codes);
			for ( i=0; i<numcodes; ++i ) {
				fprintf(stderr, "Code %d: 0x%x\n", i, PrintFOURCC(codes[i]));
			}
			free(codes);
		}
	} else {
		fprintf(stderr, "No FOURCC codes supported\n");
	}
#endif

	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	result = lpDraw2->CreateSurface(&ddsd, &dd_surface_primary, NULL); 

	if (result != DD_OK) {
		return -1;
	}

	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_BACKBUFFERCOUNT | DDSD_PIXELFORMAT;
	ddsd.dwBackBufferCount = 1;

	ddsd.dwWidth = getWidth();
	ddsd.dwHeight= getHeight();

	ddsd.ddsCaps.dwCaps |= DDSCAPS_OVERLAY;
	ddsd.ddsCaps.dwCaps |= DDSCAPS_FLIP;
	ddsd.ddsCaps.dwCaps |= DDSCAPS_COMPLEX;

	ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
	ddsd.ddpfPixelFormat.dwFourCC = mmioFOURCC('Y', 'U', 'Y', '2');

	result = lpDraw2->CreateSurface(&ddsd, &dd_surface_overlay, NULL); 
	if ( result != DD_OK ) {
		SetDDerror("CreateSurface", result);
		return(-1);
	}

	SetRect(&rectOverlaySource, 0, 0, getWidth(), getHeight());

	result = lpDraw2->CreateClipper(0, &lpClipper, NULL);
	if (result != DD_OK) {
		SetDDerror("CreateClipper", result);
		return -1;
	}
	result = IDirectDrawClipper_SetHWnd(lpClipper, 0, window_id);
	dd_surface_overlay->SetClipper(lpClipper);

	ShowWindow(window_id, SW_SHOW);
}

int DirectXOverlay::lock() {
	HRESULT result;
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	result = IDirectDrawSurface_Lock(dd_surface_overlay, NULL,
					  &ddsd, DDLOCK_NOSYSLOCK, NULL);
	if (result != DD_OK) {
		SetDDerror("Lock", result);
	}
	return 0;
}

int DirectXOverlay::unlock() {
	IDirectDrawSurface3_Unlock(dd_surface_overlay, NULL);
	return 0;
}

int8_t ** DirectXOverlay::getPixels() {
	_yuv_data->pitches[0] = (int) getWidth();
	_yuv_data->pitches[1] = _yuv_data->pitches[0] / 2;
	_yuv_data->pitches[2] = _yuv_data->pitches[0] / 2;

	_yuv_data->pixels[0] = (int8_t *) ddsd.lpSurface;
	_yuv_data->pixels[1] = _yuv_data->pixels[0] + _yuv_data->pitches[0] * getHeight();
	_yuv_data->pixels[2] = _yuv_data->pixels[1] + _yuv_data->pitches[1] * getHeight() / 2;

	return (int8_t **) _yuv_data->pixels;
}

void DirectXOverlay::display() {
	HRESULT result;
	RECT src, dst;
	DDBLTFX ddbfx;

	SetRect(&src, 0, 0, getWidth(), getHeight());
	SetRect(&dst, m_xWinPos, m_yWinPos,
		      m_xWinPos + m_WinWidth,
		      m_yWinPos + m_WinHeight);

	ddbfx.dwSize = sizeof(DDBLTFX);
	ddbfx.dwFillColor = 0;

	dd_surface_primary->Blt(&src, dd_surface_overlay, &dst,
			   DDBLT_COLORFILL | DDBLT_WAIT,
			   &ddbfx);
}

void DirectXOverlay::updateOverlayPosition(int x, int y) {
	HRESULT result;
	RECT src, dst;

	m_xWinPos = x;
	m_yWinPos = y;

	SetRect(&src, 0, 0, getWidth(), getHeight());
	SetRect(&dst, m_xWinPos, m_yWinPos,
		      m_xWinPos + m_WinWidth,
		      m_yWinPos + m_WinHeight);

	result = dd_surface_overlay->UpdateOverlay(&src,
		dd_surface_primary, &dst, showFlags, NULL);

	if (result == DDERR_SURFACELOST) {
		dd_surface_overlay->Restore();
	}
}

void DirectXOverlay::updateOverlaySize(int width, int height) {
	HRESULT result;
	RECT src, dst;

	m_WinWidth = width;
	m_WinHeight = height;

	SetRect(&src, 0, 0, getWidth(), getHeight());
	SetRect(&dst, m_xWinPos, m_yWinPos,
		      m_xWinPos + m_WinWidth,
		      m_yWinPos + m_WinHeight);

	result = dd_surface_overlay->UpdateOverlay(&src,
		dd_surface_primary, &dst, showFlags, NULL);
	if (result == DDERR_SURFACELOST) {
		dd_surface_overlay->Restore();
	}
}

void DirectXOverlay::hideOverlay() {
	showFlags = DDOVER_HIDE;
	dd_surface_overlay->UpdateOverlay(NULL,
		dd_surface_primary, NULL, showFlags, NULL);
}

void DirectXOverlay::showOverlay() {
	showFlags = DDOVER_SHOW;
	dd_surface_overlay->UpdateOverlay(NULL,
		dd_surface_primary, NULL, showFlags, NULL);
}
