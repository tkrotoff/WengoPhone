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


#ifndef YUV_OVERLAY_H
#define YUV_OVERLAY_H

#define YUV_OVERLAY_IYUV    0x01

typedef signed char  int8_t;

class YUVOverlay {
    public:
        YUVOverlay() {};
        YUVOverlay(int width, int height, int format);

		virtual int create() = 0;
		virtual int lock() = 0;
		virtual int unlock() = 0;
		virtual int8_t **getPixels() = 0;
		virtual void display() = 0;

		int getWidth() {
			return width;
		}

		int getHeight() {
			return height;
		}

		void setWidth(int w) {
			width = w;
		}

		void setHeight(int h) {
			height = h;
		}

		void setParentWindowId(long id) {
			parent_window_id = id;
		}

		long getParentWindowId() {
			return parent_window_id;
		}

    private:
        int width, height;
        int format;
		long parent_window_id;
};

#endif
