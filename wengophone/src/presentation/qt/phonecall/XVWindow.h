/*
 * XVWindow.h - XVWindow
 * High-level class offering X-Video hardware acceleration
 *
 * Matthias Schneider <ma30002000@yahoo.de>
 * 07-May-2006: initial version
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, v2, as
 * published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Copyright (C) 2006 Matthias Schneider
 */

#ifndef XVWINDOW_H
#define XVWINDOW_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xv.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/Xvlib.h>
#include <pixertool/pixertool.h>

#define GUID_I420_PLANAR 0x30323449

#define wm_LAYER 1
#define wm_FULLSCREEN 2
#define wm_STAYS_ON_TOP 4
#define wm_ABOVE 8
#define wm_BELOW 16
#define wm_NETWM (wm_FULLSCREEN | wm_STAYS_ON_TOP | wm_ABOVE | wm_BELOW)

#define WIN_LAYER_ONBOTTOM               2
#define WIN_LAYER_NORMAL                 4
#define WIN_LAYER_ONTOP                  6
#define WIN_LAYER_ABOVE_DOCK             10

#define _NET_WM_STATE_REMOVE        0    /* remove/unset property */
#define _NET_WM_STATE_ADD           1    /* add/set property */
#define _NET_WM_STATE_TOGGLE        2    /* toggle property  */

#define MWM_HINTS_FUNCTIONS     (1L << 0)
#define MWM_HINTS_DECORATIONS   (1L << 1)
#define MWM_FUNC_RESIZE         (1L << 1)
#define MWM_FUNC_MOVE           (1L << 2)
#define MWM_FUNC_MINIMIZE       (1L << 3)
#define MWM_FUNC_MAXIMIZE       (1L << 4)
#define MWM_FUNC_CLOSE          (1L << 5)
#define MWM_DECOR_ALL           (1L << 0)
#define MWM_DECOR_MENU          (1L << 4)

#define DEFAULT_SLAVE_RATIO     5
#define DEFAULT_X               1
#define DEFAULT_Y               1

typedef struct {
    int flags;
	long functions;
	long decorations;
	long input_mode;
	long state;
} MotifWmHints;

typedef struct {
	bool fullscreen;
	bool ontop;
	bool decoration;
	int oldx;
	int oldy;
	int oldWidth;
	int oldHeight;
	int curX;
	int curY;
	int curWidth;
	int curHeight;
	int origLayer;
} State;

extern int XShmGetEventBase(Display *);
extern XvImage *XvShmCreateImage(Display *, XvPortID, int, char *, int, int, XShmSegmentInfo *);

class  XVWindow {

	public:
		XVWindow();
		~XVWindow();
		int init(Display* dp, Window rootWindow, int x, int y, int windowWidth, int windowHeight, int imageWidth, int imageHeight);
		void putFrame(piximage* frame);
		void toggleFullscreen ();
		void toggleOntop();
		bool isFullScreen();

		void resize (int x, int y, int windowWidth, int windowHeight);
		void getWindowSize(unsigned int* windowWidth, unsigned int* windowHeight);

		Window getWindow ()              { return (_XVWindow);          };
		int getYUVWidth()                { return (_XVImage->width);    };
		int getYUVHeight()               { return (_XVImage->height);   };
		void registerMaster(XVWindow* m) { if(_master) _master=m;       };
		void registerSlave(XVWindow* s)  { if(_slave) _slave=s;         };

	private:
		Display*_display;
		Window _rootWindow;
		Window _XVWindow;
		unsigned int _XVPort;
		GC _gc;
		XvImage* _XVImage;
		XShmSegmentInfo _XShmInfo;
		int _wmType;
		State _state;
		XVWindow* _master;
		XVWindow* _slave;

		Atom XA_NET_SUPPORTED;
		Atom XA_WIN_PROTOCOLS;
		Atom XA_WIN_LAYER;
		Atom XA_NET_WM_STATE;
		Atom XA_NET_WM_STATE_FULLSCREEN;
		Atom XA_NET_WM_STATE_ABOVE;
		Atom XA_NET_WM_STATE_STAYS_ON_TOP;
		Atom XA_NET_WM_STATE_BELOW;
		Atom WM_DELETE_WINDOW;

		void setLayer (int layer);                                                                              // set the layer for the window
		void setEWMHFullscreen (int action);                                                                    // fullscreen for ewmh WMs
		void setDecoration(bool d);
		void setSizeHints(int x, int y, int imageWidth, int imageHeight, int windowWidth, int windowHeight);

		int getWMType();                                                                                        // detect window manager type
		int getGnomeLayer();
		int getSupportedState(Atom atom);                                                                       // test an atom
		int getWindowProperty(Atom type, Atom ** args, unsigned long *nitems);                                  // returns the root window's

		bool _isInitialize;
};
#endif
