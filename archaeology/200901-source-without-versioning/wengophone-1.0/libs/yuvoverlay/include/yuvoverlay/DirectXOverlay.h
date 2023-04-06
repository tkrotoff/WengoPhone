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


#ifndef YUV_OVERLAY_DIRECTX_H
#define YUV_OVERLAY_DIRECTX_H

#include <yuvoverlay/yuvoverlay.h>
#include <windows.h>
#include <mmsystem.h>
#include <ddraw.h>

struct private_yuvdata {
	int pitches[3];
	int8_t *pixels[3];
};

class DirectXOverlay : public YUVOverlay {
    public:
        DirectXOverlay(int width, int height, int format);
		int create();
		int lock();
		int unlock();
		int8_t ** getPixels();
		void display();

		void updateOverlayPosition(int, int);
		void updateOverlaySize(int, int);
		void hideOverlay();
		void showOverlay();

		struct private_yuvdata *_yuv_data;
		HWND window_id;
		HINSTANCE hInst;
		HINSTANCE DDrawDLL;
		LPDIRECTDRAW lpDraw;
		LPDIRECTDRAW2 lpDraw2;
		LPDIRECTDRAWSURFACE dd_surface_primary;
		LPDIRECTDRAWSURFACE dd_surface_overlay;
		LPDIRECTDRAWCLIPPER lpClipper;
		RECT rectOverlaySource, rectOverlayDest;
		DDSURFACEDESC ddsd;

		int m_xWinPos;
		int m_yWinPos;
		int m_WinWidth;
		int m_WinHeight;
		int showFlags;
};

#endif
