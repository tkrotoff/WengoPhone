/*
 * psimng.cpp - QImageFormat for loading MNG animations at once
 * Copyright (C) 2003  Michail Pishchagin
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "psimng.h"

#include <qdatetime.h>

#include <qimage.h>
#include <qasyncimageio.h>
#include <qiodevice.h>

// Define XMD_H prohibits the included headers of libmng.h to typedef INT32.
// This is needed for Borland with STL support, since in that case, INT32 is
// already defined by some Borland header.
#define XMD_H
#if defined(Q_OS_UNIXWARE)
#  define HAVE_BOOLEAN  // libjpeg under Unixware seems to need this
#endif
#include <libmng.h>
#include <stdlib.h>

class PsiMNGFormat : public QImageFormat {
public:
	PsiMNGFormat();
	~PsiMNGFormat();

	int decode(QImage& img, QImageConsumer *cons, const uchar *buf, int length);

	bool       openstream();
	bool       closestream();
	bool       readdata(mng_ptr pBuf, mng_uint32 iBuflen, mng_uint32p pRead);
	bool       errorproc(mng_int32 iErrorcode, mng_int8 /*iSeverity*/, mng_chunkid iChunkname, mng_uint32 /*iChunkseq*/, mng_int32 iExtra1, mng_int32 iExtra2, mng_pchar zErrortext);
	bool       processheader(mng_uint32 iWidth, mng_uint32 iHeight);
	mng_ptr    getcanvasline(mng_uint32 iLinenr);
	mng_bool   refresh(mng_uint32 x, mng_uint32 y, mng_uint32 w, mng_uint32 h);
	mng_uint32 gettickcount();
	bool       settimer(mng_uint32 iMsecs);

private:
	// Animation-level information
	enum {
		MovieStart,
		Data,
	} state;

	// Image-level information
	mng_handle handle;

	// Timing
	int time;

	// Temporary locals during single data-chunk processing
	const uchar *data;
	int dataSize, processedData;

	QImageConsumer *consumer;
	QImage *image;
};

//----------------------------------------------------------------------------
// C-callback to C++-member-function conversion
//----------------------------------------------------------------------------

static mng_bool openstream( mng_handle handle )
{
	return ((PsiMNGFormat*)mng_get_userdata(handle))->openstream();
}
static mng_bool closestream( mng_handle handle )
{
	return ((PsiMNGFormat*)mng_get_userdata(handle))->closestream();
}
static mng_bool readdata( mng_handle handle, mng_ptr pBuf, mng_uint32 iBuflen, mng_uint32p pRead )
{
	return ((PsiMNGFormat*)mng_get_userdata(handle))->readdata(pBuf,iBuflen,pRead);
}
static mng_bool errorproc( mng_handle handle,
			   mng_int32   iErrorcode,
			   mng_int8    iSeverity,
			   mng_chunkid iChunkname,
			   mng_uint32  iChunkseq,
			   mng_int32   iExtra1,
			   mng_int32   iExtra2,
			   mng_pchar   zErrortext )
{
	return ((PsiMNGFormat*)mng_get_userdata(handle))->errorproc(iErrorcode,
								    iSeverity,iChunkname,iChunkseq,iExtra1,iExtra2,zErrortext);
}
static mng_bool processheader( mng_handle handle,
			       mng_uint32 iWidth, mng_uint32 iHeight )
{
	return ((PsiMNGFormat*)mng_get_userdata(handle))->processheader(iWidth,iHeight);
}
static mng_ptr getcanvasline( mng_handle handle, mng_uint32 iLinenr )
{
	return ((PsiMNGFormat*)mng_get_userdata(handle))->getcanvasline(iLinenr);
}
static mng_bool refresh( mng_handle handle,
			 mng_uint32  iTop,
			 mng_uint32  iLeft,
			 mng_uint32  iBottom,
			 mng_uint32  iRight	)
{
	return ((PsiMNGFormat*)mng_get_userdata(handle))->refresh(iTop,iLeft,iBottom,iRight);
}
static mng_uint32 gettickcount( mng_handle handle )
{
	return ((PsiMNGFormat*)mng_get_userdata(handle))->gettickcount();
}
static mng_bool settimer( mng_handle handle, mng_uint32  iMsecs )
{
	return ((PsiMNGFormat*)mng_get_userdata(handle))->settimer(iMsecs);
}

static mng_ptr memalloc( mng_size_t iLen )
{
	return calloc(1,iLen);
}
static void memfree( mng_ptr iPtr, mng_size_t /*iLen*/ )
{
	free(iPtr);
}

//----------------------------------------------------------------------------

PsiMNGFormat::PsiMNGFormat()
{
	state = MovieStart;
	handle = 0;
	time = 0;
}

PsiMNGFormat::~PsiMNGFormat()
{
}

bool PsiMNGFormat::openstream()
{
	return true;
}

bool PsiMNGFormat::closestream()
{
	return true;
}

bool PsiMNGFormat::readdata(mng_ptr pBuf, mng_uint32 iBuflen, mng_uint32p pRead)
{
	if ( iBuflen <= (uint)dataSize ) {
		memcpy(pBuf, data + processedData, iBuflen);
		*pRead = iBuflen;
	}

	processedData += iBuflen;

	return true;
}

bool PsiMNGFormat::errorproc( mng_int32 iErrorcode, mng_int8 /*iSeverity*/, mng_chunkid iChunkname, mng_uint32 /*iChunkseq*/, mng_int32 iExtra1, mng_int32 iExtra2, mng_pchar zErrortext )
{
	qWarning("MNG error %d: %s; chunk %c%c%c%c; subcode %d:%d",
		 iErrorcode,zErrortext,
		 (iChunkname>>24)&0xff,
		 (iChunkname>>16)&0xff,
		 (iChunkname>>8)&0xff,
		 (iChunkname>>0)&0xff,
		 iExtra1,iExtra2);
	return TRUE;
}

bool PsiMNGFormat::processheader(mng_uint32 iWidth, mng_uint32 iHeight)
{
	image->create(iWidth, iHeight, 32);
	image->setAlphaBuffer(TRUE);
	memset(image->bits(), 0, iWidth*iHeight*4);
	consumer->setSize(iWidth, iHeight);
	mng_set_canvasstyle(handle,
			    QImage::systemByteOrder() == QImage::LittleEndian
			    ? MNG_CANVAS_BGRA8 : MNG_CANVAS_ARGB8 );
	return TRUE;
}

mng_ptr PsiMNGFormat::getcanvasline(mng_uint32 iLinenr)
{
	return image->scanLine(iLinenr);
}

mng_bool PsiMNGFormat::refresh(mng_uint32 x, mng_uint32 y, mng_uint32 w, mng_uint32 h)
{
	QRect r(x,y,w,h);
	consumer->changed(r);
	consumer->setFramePeriod(0);
	consumer->frameDone();
	return TRUE;
}

mng_uint32 PsiMNGFormat::gettickcount()
{
	return time;
}

bool PsiMNGFormat::settimer(mng_uint32 iMsecs)
{
	consumer->setFramePeriod(iMsecs);
	consumer->frameDone();
	time += iMsecs;
	return TRUE;
}

int PsiMNGFormat::decode(QImage &img, QImageConsumer *cons, const uchar *buf, int length)
{
	//if ( !length ) // there's no more data then
	//	return 0;

	consumer = cons;
	image = &img;

	data = buf;
	dataSize = length;
	processedData = 0;

	if ( state == MovieStart ) {
		handle = mng_initialize( (mng_ptr)this, ::memalloc, ::memfree, 0 );
		//mng_set_suspensionmode( handle, MNG_FALSE );
		mng_setcb_openstream( handle, ::openstream );
		mng_setcb_closestream( handle, ::closestream );
		mng_setcb_readdata( handle, ::readdata );
		mng_setcb_errorproc( handle, ::errorproc );
		mng_setcb_processheader( handle, ::processheader );
		mng_setcb_getcanvasline( handle, ::getcanvasline );
		mng_setcb_refresh( handle, ::refresh );
		mng_setcb_gettickcount( handle, ::gettickcount );
		mng_setcb_settimer( handle, ::settimer );
		state = Data;

		mng_readdisplay(handle);
	}
	else
		mng_display_resume(handle);

	image = 0;

	return processedData;
}

//----------------------------------------------------------------------------

class PsiMNGFormatType : public QImageFormatType
{
	QImageFormat *decoderFor(const uchar *buffer, int length)
	{
		if (length < 8)
			return 0;

		if (buffer[0]==138 // MNG signature
		 && buffer[1]=='M'
		 && buffer[2]=='N'
		 && buffer[3]=='G'
		 && buffer[4]==13
		 && buffer[5]==10
		 && buffer[6]==26
		 && buffer[7]==10) {
			return new PsiMNGFormat;
		}

		return 0;
	}

	const char *formatName() const
	{
		return "PsiMNG";
	}
};

static PsiMNGFormatType *globalMngFormatTypeObject = 0;

void cleanupPsiMngIO()
{
	if ( globalMngFormatTypeObject ) {
		delete globalMngFormatTypeObject;
		globalMngFormatTypeObject = 0;
	}
}

void initPsiMngIO()
{
	static bool done = FALSE;
	if ( !done ) {
		done = TRUE;
		globalMngFormatTypeObject = new PsiMNGFormatType;
		qAddPostRoutine( cleanupPsiMngIO );
	}
}
