/* Keramik Style for KDE3
   Copyright (c) 2002 Malte Starostik <malte@kde.org>
                  (c) 2002 Maksim Orlovich <mo002j@mail.rochester.edu>

   based on the KDE3 HighColor Style

   Copyright (C) 2001-2002 Karol Szwed      <gallium@kde.org>
             (C) 2001-2002 Fredrik H�glund  <fredrik@kde.org>

   Drawing routines adapted from the KDE2 HCStyle,
   Copyright (C) 2000 Daniel M. Duley       <mosfet@kde.org>
             (C) 2000 Dirk Mueller          <mueller@kde.org>
             (C) 2001 Martijn Klingens      <klingens@kde.org>

    Progressbar code based on KStyle, Copyright (C) 2001-2002 Karol Szwed <gallium@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#ifndef THINKERAMIK_IMAGE_H
#define THINKERAMIK_IMAGE_H

struct ThinKeramikEmbedImage
{
	bool haveAlpha;
	int width;
	int height;
	int id;
	const unsigned char* data;
};

extern ThinKeramikEmbedImage* ThinKeramikGetDbImage(int id);
extern void ThinKeramikDbCleanup();

enum ThinKeramikTileType
{
	ThinKeramikTileTL = 0,
	ThinKeramikTileTC = 1,
	ThinKeramikTileTR = 2,
	ThinKeramikTileCL = 3,
	ThinKeramikTileCC = 4,
	ThinKeramikTileCR = 5,
	ThinKeramikTileRL = 6,
	ThinKeramikTileRC = 7,
	ThinKeramikTileRR = 8,
	ThinKeramikTileSeparator = 16,
	ThinKeramikSlider1 = 32,
	ThinKeramikSlider2 = 48,
	ThinKeramikSlider3 = 64,
	ThinKeramikSlider4 = 80,
	ThinKeramikGroove1 = 96,
	ThinKeramikGroove2 = 112
};

#endif
