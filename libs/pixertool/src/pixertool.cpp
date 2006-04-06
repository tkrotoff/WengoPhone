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

#include <pixertool/pixertool.h>

#include <pixertool/ffmpeg-pixertool.h>

#include <stdlib.h>
#include <string.h>

/*
 * Private functions
 */

/**
 * Convert data from NV12 to YUV420P.
 *
 * @param data data to convert
 * @param width image widht
 * @param height image height
 * @return converted data
 */
static uint8_t * _nv12_to_yuv420p(uint8_t *data, unsigned width, unsigned height);

piximage * pix_alloc(pixosi pix, unsigned width, unsigned height) {
	piximage *image = (piximage *) malloc(sizeof(piximage));

	avcodec_init();

	image->width = width;
	image->height = height;
	image->palette = pix;

	image->data = (uint8_t *)av_malloc(pix_size(pix, width, height)  * sizeof(uint8_t));

	return image;
}

void pix_free(piximage *ptr) {
	if (ptr) {
		if (ptr->data)
			av_free(ptr->data);
		free(ptr);
	}
}

unsigned pix_size(pixosi pix, unsigned width, unsigned height) {
	return avpicture_get_size(pix_ffmpeg_from_pix_osi(pix), width, height);
}

pixerrorcode pix_convert(int flags, piximage *img_dst, piximage *img_src) {
	AVPicture avp_source, avp_target, avp_tmp_target;
	int len_target = 0;
	int pix_fmt_source;
	int pix_fmt_target;
	int need_avfree = 0;
	int need_resize = 0;
	ImgReSampleContext *resample_context = NULL;
	pixosi pix_osi_source = img_src->palette;
	unsigned width = img_src->width;
	unsigned height = img_src->height;
	uint8_t *buf_source = img_src->data;

	if (pix_osi_source == PIX_OSI_NV12) {
		buf_source = _nv12_to_yuv420p(buf_source, width, height);
		need_avfree = 1;
		pix_osi_source = PIX_OSI_YUV420P;
	}

	if ((width != img_dst->width) || (height != img_dst->height)) {
		need_resize = 1;
	}

	len_target = pix_size(img_dst->palette, width, height);

	pix_fmt_source = pix_ffmpeg_from_pix_osi(pix_osi_source);
	pix_fmt_target = pix_ffmpeg_from_pix_osi(img_dst->palette);

	avpicture_fill(&avp_source,  buf_source, pix_fmt_source, width, height);
	avpicture_fill(&avp_target, img_dst->data, pix_fmt_target, width, height);

	/* Only flip other planes if the destination palette is YUV420
	 * FIXME DUDE
	 */
	if ((flags & PIX_FLIP_HORIZONTALLY) && (pix_osi_source == PIX_OSI_YUV420P)) {
		avp_source.data[0] += avp_source.linesize[0] * (height - 1);
		avp_source.linesize[0] *= -1;

		if (pix_fmt_source == PIX_FMT_YUV420P) {
			avp_source.data[1] += avp_source.linesize[1] * (height / 2 - 1);
			avp_source.linesize[1] *= -1;
			avp_source.data[2] += avp_source.linesize[2] * (height / 2 - 1);
			avp_source.linesize[2] *= -1;
		}
	}

	// Resizing picture if needed. Needs test
	/*
	if (need_resize) {
		avpicture_fill(&avp_tmp_target, img_dst->data, pix_fmt_target, width, height);
		//TODO: optimize this part
		resample_context = img_resample_init(img_dst->width, img_dst->height,
			img_src->width, img_src->height);
		if (resample_context);
			img_resample(resample_context, avp_tmp_target, avp_source);
			img_resample_close(resample_context);
		}

		if (img_convert(&avp_target, pix_fmt_target,
			&avp_tmp_target, pix_fmt_source,
			width, height) == -1) {
			return PIX_NOK;
		}
	}
	*/
	////

	// Converting palette
	if (img_convert(&avp_target, pix_fmt_target,
		&avp_source, pix_fmt_source,
		width, height) == -1) {
		return PIX_NOK;
	}
	////

	if (need_avfree) {
		av_free(buf_source);
	}

	return PIX_OK;
}

static uint8_t * _nv12_to_yuv420p(uint8_t *data, unsigned width, unsigned height) {
	uint8_t *buf_source;
	uint8_t *buf_target;
	int len_target;
	register unsigned i;

	buf_source = data;
	len_target = (width * height * 3) / 2;
	buf_target = (uint8_t *) av_malloc(len_target * sizeof(uint8_t));

	memcpy(buf_target, buf_source, width * height);

	for (i = 0 ; i < (width * height / 4) ; i++) {
		buf_target[(width * height) + i] = buf_source[(width * height) + 2 * i];
		buf_target[(width * height) + (width * height / 4) + i]
			= buf_source[(width * height) + 2 * i + 1];
	}

	return buf_target;
}

piximage * pix_copy(piximage *src) {
	piximage *result = pix_alloc(src->palette, src->width, src->height);
	memcpy(result->data, src->data, pix_size(src->palette, src->width, src->height));

	return result;
}
