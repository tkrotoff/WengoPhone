/*
 * phmedia -  Phone Api media streamer (video part)
 *
 * Copyright (C) 2005 Wengo SAS
 * Copyright (C) 2004 Vadim Lebedev <vadim@mbdsys.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with dpkg; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/**
 * @author David Ferlier <david.ferlier@wengo.fr>
 */

#define _WIN32_WINNT 0x5000

#include <osip2/osip_mt.h>
#include <osip2/osip.h>
#include <ortp.h>
#include <ortp-export.h>
#include <telephonyevents.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <webcam_c/webcam_c.h>

#include <phapi.h>
#include <phcall.h>
#include <phmedia.h>
#include <phcodec.h>
#include <tonegen.h>
#include <phmbuf.h>
#include <phvstream.h>
#include <phcodec-h263.h>

#ifdef USE_HTTP_TUNNEL
#include <httptunnel.h>
#endif

#include "nowebcam.yuv"

/**
 * @brief Function called by ffmpeg when a RTP slice is available
 *
 * This is called by ffmpeg in SLICE_STRUCT mode. When a fragment of data
 * is ready to be sent, it calls this function. What we just do is send
 * the fuckin' frame on RTP :)
 *
 * @param ctx		The video stream
 * @param data		The packet's data
 * @param size		The size of the packet
 * @param ts		The timestamp to use in the rtp packet
 * @param eof		End Of File (if it's the last packet of a serie)
 *
 */

void phmedia_video_rtpsend_callback (void *ctx, void *data, int size,
				unsigned long ts, int eof)
{
	phvstream_t *video_stream = (phvstream_t *) ctx;
	mblk_t *m1 ;

	video_stream->txtstamp = ts;
	
	m1 = rtp_session_create_packet (video_stream->ms.rtp_session,
			RTP_FIXED_HEADER_SIZE, (char *)data, size);	
	if (!m1)
		return;
	
	if (eof) {
		rtp_set_markbit(m1, 1);
	}

	rtp_session_sendm_with_ts(video_stream->ms.rtp_session, m1,
			ts);

}

/**
 * @brief Initializes the webcam for a given stream
 *
 * This setups the webcam, and sets the resolution decided by the user.
 * 
 * @param vstream		The video stream
 * @return	return code of webcamOpen (WEBCAM_CODE_OK on ok, NOK on nok)
 *
 */

int ph_media_video_initialize_webcam(phvstream_t *vstream) {
	phConfig_t *cfg = phGetConfig();
	int err;

	webcamSetDevice(vstream->wt, cfg->video_config.video_device);
	err = webcamOpen(vstream->wt);

	if (err) {
		webcamSetResolution(vstream->wt,
			cfg->video_config.video_webcam_capture_width,
			cfg->video_config.video_webcam_capture_height);
	}
	return err;
}

/**
 * @brief Function called by the webcam driver
 *
 * This function is called by the webcam driver. In this function we
 * convert the frame to YUV420 Planar (which is the format required
 * by ffmpeg and H.263), resize it to a fixed size (for now 176x144),
 * encode it. The encode function calls the RTP callback when it has
 * a RTP frame to be sent
 *
 * @param userdata		The video stream
 * @param buf_frameready	Pointer to the webcam frame
 * @param width_frameready			width of the served frame
 * @param height_frameready			height of the served frame
 * @param pix_osi_frameready			palette of the served frame on the osi lib plane
 * @param len_frameready			Length (in bytes) of the served frame buffer
 * @return not relevant
 *
 */

int webcam_frame_callback(void *userdata, uint8_t *buf_frameready, int width_frameready, int height_frameready, int pix_osi_frameready, int len_frameready ) {
	phm_videoframe_t *phmvf;
	phvstream_t *video_stream = (phvstream_t *) userdata;

    if (!video_stream || !video_stream->ms.running) {
        return;
    }

	phmvf = (phm_videoframe_t*) malloc(sizeof(phm_videoframe_t));
	phmvf->buffer = buf_frameready;
	phmvf->width = width_frameready;
	phmvf->height = height_frameready;
	phmvf->pix_osi = pix_osi_frameready;
	phmvf->len = len_frameready;
	phmvf->freetype = 0;
	
#ifdef PH_VIDEO_USELOCK
	osip_mutex_lock(video_stream->mtx);
#endif
	osip_list_add(&video_stream->webcam_frames_q, phmvf, -1);
#ifdef PH_VIDEO_USELOCK
	osip_mutex_unlock(video_stream->mtx);
#endif
}

/**
 * @brief Function called to both encode and send one full picture frame on the tx path of the stream
 *
 * This function is used to encode and send a frame on the network
 *
 * @param video_stream	the overall phmedia stream structure for video streams
 * @param phmvf	this is the structure holding the framebuffer and some metadata information
 * @param cache	0/1 : the yuv frame must be cached on the video stream. This is used for local image incrustation for example. It is done here to avoid converting to YUV twice
 * @return not relevant
 *
 */
int ph_media_video_send_frame(phvstream_t *video_stream, phm_videoframe_t *phmvf, int cache) {
	ph_h263_encoder_ctx_t *video_encoder;
	int len_prepared_for_encoding, enclen;
	AVFrame *avf_prepared_for_encoding;
	ImgReSampleContext *avresampler;

	enclen = 0;
	video_encoder = (ph_h263_encoder_ctx_t *) video_stream->ms.encoder_ctx;

	// wrap the source image in an avp
	avpicture_fill(&video_stream->avp_proc_source,
		(uint8_t *)phmvf->buffer,
		webcamPixOsiToPixFmt(video_stream->wt, phmvf->pix_osi),
		phmvf->width, phmvf->height);
	
	// flipping trick with ffmpeg - avcodec. this will flip the image during the img_convert
	if (phGetConfig()->video_config.video_camera_flip_frame) {

		video_stream->avp_proc_source.data[0] += video_stream->avp_proc_source.linesize[0] * (phmvf->height-1);
		video_stream->avp_proc_source.linesize[0] *= -1;

		//HACK: Only flip other planes if the destination palette is YUV420
		if (webcamPixOsiToPixFmt(video_stream->wt, phmvf->pix_osi) == PIX_FMT_YUV420P) {
			video_stream->avp_proc_source.data[1] += video_stream->avp_proc_source.linesize[1] * (phmvf->height/2-1);
			video_stream->avp_proc_source.linesize[1] *= -1;
			video_stream->avp_proc_source.data[2] += video_stream->avp_proc_source.linesize[2] * (phmvf->height/2-1);
			video_stream->avp_proc_source.linesize[2] *= -1;
		}

	}
	
	// Convert the image to YUV420P before encoding it
	ph_media_video_check_processing_buffers(video_stream, phmvf->width, phmvf->height);
	img_convert(
				&video_stream->avp_proc_source_yuv, PIX_FMT_YUV420P,
				&video_stream->avp_proc_source, webcamPixOsiToPixFmt(video_stream->wt, phmvf->pix_osi),
				phmvf->width, phmvf->height
				);


	// We resize the frame to a predefined size (width is VIDEO_FRAME_WIDTH
	// and height is VIDEO_FRAME_HEIGHT)
	len_prepared_for_encoding = avpicture_get_size (PIX_FMT_YUV420P,
			PHMEDIA_VIDEO_FRAME_WIDTH,
			PHMEDIA_VIDEO_FRAME_HEIGHT);

	avpicture_fill (
		(AVPicture *) video_encoder->encoder_ctx.sampled_frame,
		video_stream->avp_proc_source_yuv.data[0],
		PIX_FMT_YUV420P, phmvf->width, phmvf->height);

	if (phmvf->width  == PHMEDIA_VIDEO_FRAME_WIDTH &&
	    phmvf->height == PHMEDIA_VIDEO_FRAME_HEIGHT) {
		avf_prepared_for_encoding = video_encoder->encoder_ctx.sampled_frame;

	} else {

		avresampler = phcodec_avcodec_get_resampler(&video_encoder->encoder_ctx, &video_encoder->meta, phmvf->width, phmvf->height);
		img_resample(avresampler,
			(AVPicture *) video_encoder->encoder_ctx.resized_pic,
			(AVPicture *) video_encoder->encoder_ctx.sampled_frame);
		avf_prepared_for_encoding = video_encoder->encoder_ctx.resized_pic;
		
	}

	// if asked, a copy of the frame is put in cache on the stream
	if (cache) {
		img_copy(&video_stream->avp_proc_localcapture_cache, (AVPicture *)avf_prepared_for_encoding, PIX_FMT_YUV420P,
				PHMEDIA_VIDEO_FRAME_WIDTH, PHMEDIA_VIDEO_FRAME_HEIGHT);
	}

	// Now we pass the resized picture to the codec, giving him a
	// max_frame_len of buffer that he should never overflow.
	// note that this leads to packets sent over RTP through the rtpcallback
	// that is registered on the encoder

	enclen = video_stream->ms.codec->encode(video_encoder,
		avf_prepared_for_encoding, len_prepared_for_encoding,
		video_encoder->data_enc, video_encoder->max_frame_len
	);
	
	video_stream->num_encoded_frames += 1;
	video_stream->txtstamp += 90000;

	return 1;
}


void ph_media_video_alloc_processing_buffers(phvstream_t *vstream, int width_source, int height_source) {

	vstream->width_proc_source = width_source;
	vstream->height_proc_source = height_source;
	
	// avp picture + associated buffer prepared to hold the cached local webcam picture
	avpicture_alloc(&vstream->avp_proc_localcapture_cache,
		PIX_FMT_YUV420P, PHMEDIA_VIDEO_FRAME_WIDTH, PHMEDIA_VIDEO_FRAME_HEIGHT);

	avpicture_alloc(&vstream->avp_proc_source_yuv,
		PIX_FMT_YUV420P, width_source, height_source);
	
}

void ph_media_video_check_processing_buffers(phvstream_t *vstream, int width_source, int height_source){
	
	if ( (width_source!=vstream->width_proc_source)||(height_source!=vstream->height_proc_source)) {
		
		ph_media_video_free_processing_buffers(vstream);
		ph_media_video_alloc_processing_buffers(vstream, width_source, height_source);		
		
	}
	
	
}

void ph_media_video_free_processing_buffers(phvstream_t *vstream){
	
		avpicture_free(&vstream->avp_proc_localcapture_cache);
		avpicture_free(&vstream->avp_proc_source_yuv);
	
}



/**
 * @brief Video stream start function
 *
 * This function starts the webcam, creates a rtp session (tunneled or not),
 * and setups various things like frame & rtp callbacks.
 *
 * It also starts the video thread, which is responsible for handling frames
 * coming on the rtp socket.
 *
 * @param ca		The call the stream is to be started on
 * @param video_port	The port number the rtp socket will listen on
 * @param frameDisplay	Frame callback, called when a frame is available
 * @return		0 if ok
 */

#define LOCAL_YUV_FILE	"nowebcam.yuv"


int ph_media_video_start(phcall_t *ca, int video_port, phFrameDisplayCbk frameDisplay)
{
	RtpSession *video_session;
	RtpProfile *profile;
	phvstream_t *video_stream;
	phcodec_t *codec;
	// vars the onewaycam frame server :
	int fs_x, fs_y, yuv_local_len;
	uint8_t *fs_buffer;
	FILE *f_local;

	printf("Starting video stream from port: %d to %s:%d\n",
		video_port, ca->remote_sdp_video_ip, ca->remote_sdp_video_port);

	// the function is cut into 2 branches : 
	//   - branch1: video stream is already open (RE-INVITE for example)
	//   - branch2: video stream is not already open
	
	// begin branch1
	if (ca->ph_video_stream)
	  {
		video_stream = (phvstream_t*) ca->ph_video_stream;

		if (video_stream->ms.remote_port == ca->remote_sdp_video_port)
		{ 
			if (!strcmp(video_stream->ms.remote_ip, ca->remote_sdp_video_ip))
			{
				return 0;
			}
		}

		strcpy(video_stream->ms.remote_ip, ca->remote_sdp_video_ip);
		video_stream->ms.remote_port = ca->remote_sdp_video_port;
		rtp_session_reset(video_stream->ms.rtp_session);

 #if USE_HTTP_TUNNEL
        if (video_stream->ms.tunRtp)
            {
                RtpTunnel *newTun, *old;
                
                printf("Replacing video tunnel\n"),
                newTun = rtptun_connect(video_stream->ms.remote_ip, video_stream->ms.remote_port);
                
                
                if (!newTun)
                {
                    printf("Video tunnel replacement failed\n"),
                    ph_media_video_stop(ca);
                    
                    return -PH_NORESOURCES;
                }

            	rtp_session_set_tunnels(video_stream->ms.rtp_session, newTun, NULL);
                
                old = video_stream->ms.tunRtp;
                video_stream->ms.tunRtp = newTun;
                
                TUNNEL_CLOSE(old);
                rtptun_free(old);
            }
        else
#endif
         rtp_session_set_remote_addr(video_stream->ms.rtp_session,
				  video_stream->ms.remote_ip,
				  video_stream->ms.remote_port);
        
        return 0;      
        
    }      
	// end branch1
  
	// begin branch2
	profile = get_av_profile();

	// we should be able to find a codec structure based on the negociated video payload
	if (!ca->video_payload) {
		return -1;
	}
	codec = ph_media_lookup_codec(ca->video_payload);
	if (!codec) {
		return -1;
	}
   
	// init phase : a stream structure is created and initialized
	video_stream = (phvstream_t *)osip_malloc(sizeof(phvstream_t));
	memset(video_stream, 0, sizeof(phvstream_t));

	gettimeofday(&video_stream->stat_ts_decoded_over_5s, 0);

	
	osip_list_init(&video_stream->rr_sent_q);
	osip_list_init(&video_stream->webcam_frames_q);
	osip_list_init(&video_stream->received_packets_q);

	video_stream->ms.codec = codec;
	video_stream->rtpCallback = &phmedia_video_rtpsend_callback;
	video_stream->mtx = osip_mutex_init();

	ph_media_video_alloc_processing_buffers(video_stream, PHMEDIA_VIDEO_FRAME_WIDTH, PHMEDIA_VIDEO_FRAME_HEIGHT);
    
	if (codec->encoder_init) {
		video_stream->ms.encoder_ctx = codec->encoder_init(video_stream);
	}
	
	if (codec->decoder_init) {
		video_stream->ms.decoder_ctx = codec->decoder_init(video_stream);
	}
	
#ifdef PHAPI_VIDEO_LOCAL_HACK
	strcpy(ca->remote_sdp_video_ip, "127.0.0.1");
	ca->remote_sdp_video_port = video_port;
	ph_media_use_tunnel = 0;
#endif

	video_session = rtp_session_new(RTP_SESSION_SENDRECV);
  
#ifdef USE_HTTP_TUNNEL
	if (ph_media_use_tunnel)
	{
		video_stream->ms.tunRtp = rtptun_connect(ca->remote_sdp_video_ip, ca->remote_sdp_video_port);
		if (!video_stream->ms.tunRtp)
		{
			rtp_session_destroy(video_session);
			return -PH_NORESOURCES;
		}

		rtp_session_set_tunnels(video_session, video_stream->ms.tunRtp, NULL);
	}
#endif
 
	rtp_session_set_scheduling_mode(video_session, SCHEDULING_MODE);
	rtp_session_set_blocking_mode(video_session, BLOCKING_MODE);

	rtp_session_set_profile(video_session, profile);
	rtp_session_set_jitter_compensation(video_session, 0);
	rtp_session_set_max_rq_size(video_session, 1024);

#ifdef USE_HTTP_TUNNEL
	if (!video_stream->ms.tunRtp)
	{
		rtp_session_set_local_addr(video_session, "0.0.0.0", video_port);  

		rtp_session_set_remote_addr(video_session,
				ca->remote_sdp_video_ip,
				ca->remote_sdp_video_port);
	}
#endif
	rtp_session_set_payload_type(video_session, ca->video_payload);

	// HACK
	// wt is always initialized because the webcam object holds the convertImage function...
	video_stream->wt = webcamInitialize();
	
	// choixe of the frame server and init sequence
	if (ca->user_mflags & PH_STREAM_VIDEO_TX) {
		if (ph_media_video_initialize_webcam(video_stream)) {
			video_stream->phmfs_webcam.state = 1;
		} else {
			video_stream->phmfs_onewaycam.state = 1; // init
		}
	} else {
		video_stream->phmfs_onewaycam.state = 1; // init
	}

	// start sequence of the webcam frame server
	if (video_stream->phmfs_webcam.state == 1)  {
	
			webcamSetUserData(video_stream->wt, (void *)video_stream);
			webcamSetCallback(video_stream->wt, webcam_frame_callback);
			webcamStartCapture(video_stream->wt);
			video_stream->phmfs_webcam.state = 2;
		
	}

	// start sequence of the virtual webcam frame server
	if (video_stream->phmfs_onewaycam.state == 1) {

		// note :	this "virtual" webcam may seem a little strange, but it facilitates NAT traversal right now
		//			and makes it possible to have only the tx or rx having a cam
		
		fs_buffer = (uint8_t*)av_malloc((176*144*3)/2); 

		for(fs_y=0;fs_y<144;fs_y++) {
            for(fs_x=0;fs_x<176;fs_x++) {
                fs_buffer[fs_y * 176 + fs_x] = 128;
            }
        }

        for(fs_y=0;fs_y<144/2;fs_y++) {
            for(fs_x=0;fs_x<176/2;fs_x++) {
                fs_buffer[176*144 + fs_y * 176/2 + fs_x] = 255 * fs_x * 2 / 176;
                fs_buffer[176*144 + (176*144/4) + fs_y * 176/2 + fs_x] = 255 * fs_y * 2 / 144;
            }
        }

		video_stream->phmfs_onewaycam.buffer = fs_buffer;
		
		video_stream->phmfs_onewaycam.state = 2; // start producing		

		yuv_local_len = avpicture_get_size(PIX_FMT_YUV420P, 176, 144);
		memcpy(video_stream->phmfs_onewaycam.buffer, pic_yuv, yuv_local_len);
		
	}

	video_stream->ms.running = 1;
	video_stream->frameDisplayCallback = frameDisplay;
	video_stream->ms.rtp_session = video_session;
	video_stream->ms.payload = ca->video_payload;
	video_stream->ms.ca = ca;
	video_stream->ms.codec = codec;
	video_stream->mbCounter = 0;
	video_stream->rxtstamp = 0;

	strcpy(video_stream->ms.remote_ip, ca->remote_sdp_video_ip);
	video_stream->ms.remote_port = ca->remote_sdp_video_port;

/*
   * If the user wants us to automatically adjust the bandwidth, then do it */

  if (phcfg.video_config.video_line_configuration == PHAPI_VIDEO_LINE_AUTOMATIC) {

	/* Place callbacks for RRs */
	video_session->rtcp_rr_sent_cb = ph_video_rtcp_rr_sent;
	video_session->rtcp_rr_received_cb = ph_video_rtcp_rr_received;

	/* Place callbacks for SRs */
	video_session->rtcp_sr_sent_cb = ph_video_rtcp_sr_sent;
	video_session->rtcp_sr_received_cb = ph_video_rtcp_sr_received;

	/* Start the Automatic BW control thread */
	video_stream->media_bw_control_thread = osip_thread_create(20000,
		ph_video_bwcontrol_thread, video_stream);
  }

	ph_video_bwcontrol_apply_user_params(video_stream);

	ca->ph_video_stream = video_stream;

	// start the engine !
#if defined(VIDEO_IOTHREAD_TIMERS)
	// 40 = 1000/25 = 25 frame per second
	timer_init();

	video_stream->t_impl = timer_impl_getfirst();
	video_stream->v_timer = video_stream->t_impl->timer_create();
	video_stream->t_impl->timer_set_delay(video_stream->v_timer, 40);
	video_stream->t_impl->timer_set_callback(video_stream->v_timer,
			ph_video_io_timer);
	video_stream->t_impl->timer_set_userdata(video_stream->v_timer,
			video_stream);
	video_stream->t_impl->timer_start(video_stream->v_timer);

#else
	video_stream->media_io_thread = osip_thread_create(20000,
		ph_video_io_thread, video_stream);
#endif
 

  
	return 0;
	// end branch2
}

/**
 * used to group the rtp slices falling between 2 seqnumbers and try to decode the resulting buffer.
 *
 *
 * @return 0/1 : a frame was successfully decoded and flushed to the corresponding callback
 */
int ph_media_video_flush_queue(phvstream_t *stream, unsigned long seqnumber_start, unsigned long seqnumber_end) {
	struct timeval now_time;
	ph_h263_decoder_ctx_t *video_decoder;
	AVFrame *picIn;
	phVideoFrameReceivedEvent_t *f_event;
	int len_received, len;
	int q_size, it;
	unsigned long ts_packet;
	int counter = 0;

	struct ph_video_stream_packet *phvs;
		
	phcodec_t *codec = stream->ms.codec;
	video_decoder = (ph_h263_decoder_ctx_t *) stream->ms.decoder_ctx;
	picIn = video_decoder->decoder_ctx.pictureIn;
	
	q_size = osip_list_size(&stream->received_packets_q);

	video_decoder->buf_index = 0;

	for (it = 0; it < q_size; it += 1) {
		phvs = (struct ph_video_stream_packet *) osip_list_get(
			&stream->received_packets_q, it);

		if (!phvs) {
			continue;
		}

		if (phvs->seqnumber < seqnumber_start) {
			continue;
		}

		if (phvs->seqnumber > seqnumber_end) {
			continue;
		}

		if (!phvs->mp->b_cont) {
			continue;
		}

		len_received = phvs->mp->b_cont->b_wptr - phvs->mp->b_cont->b_rptr;

#if 0

		if (video_decoder->buf_index + len_received >= MAX_DEC_BUFFER_SIZE) {
			video_decoder->buf_index = 0;
			break;
		}
#endif

		memcpy(video_decoder->data_dec + video_decoder->buf_index,
			phvs->mp->b_cont->b_rptr, len_received);

		counter +=1;
		video_decoder->buf_index += len_received;
	}

	for (it = q_size - 1; it >= 0; it -= 1) {
		phvs = (struct ph_video_stream_packet *) osip_list_get(
			&stream->received_packets_q, it);
		if (phvs && phvs->seqnumber <= seqnumber_end) {
			osip_list_remove(&stream->received_packets_q, it);
			freemsg(phvs->mp);
			free(phvs);
		}

	}

	/*
	 * Time to check if we didn't overflow the fps set on the encoder
	 *
	 */

	if (video_decoder->buf_index != 0) {

		len = codec->decode(video_decoder, video_decoder->data_dec,
			video_decoder->buf_index, picIn,
			38016);

		if (!len) {
			printf("Can't decode !\n");
			return 0;
		} else {

			stream->stat_num_decoded_total_over_5s++;

			gettimeofday(&now_time, 0);
			tvsub_phapi(&now_time, &stream->last_decode_time);
			if (picIn->pict_type != FF_I_TYPE && (now_time.tv_usec / 1000) < stream->fps_interleave_time) {
				stream->stat_num_decoded_dropped_over_5s++;
				return 0;
			}
			gettimeofday(&stream->last_decode_time, 0);

			gettimeofday(&now_time, 0);
			if ((now_time.tv_sec-stream->stat_ts_decoded_over_5s.tv_sec) >= 5) {
				printf("decoder over last 5 sec: total fps: %d, callback fps: %d\n", stream->stat_num_decoded_total_over_5s/5, (stream->stat_num_decoded_total_over_5s - stream->stat_num_decoded_dropped_over_5s)/5);
				stream->stat_num_decoded_total_over_5s = 0;
				stream->stat_num_decoded_dropped_over_5s = 0;
				gettimeofday(&stream->stat_ts_decoded_over_5s, 0);
			}
			
			f_event = (phVideoFrameReceivedEvent_t *) malloc (sizeof(phVideoFrameReceivedEvent_t));
			f_event->cam = stream->wt;
			f_event->frame_remote_width =  PHMEDIA_VIDEO_FRAME_WIDTH;
			f_event->frame_remote_height = PHMEDIA_VIDEO_FRAME_HEIGHT;
			f_event->frame_remote = picIn;
			f_event->frame_local_width = PHMEDIA_VIDEO_FRAME_WIDTH;
			f_event->frame_local_height = PHMEDIA_VIDEO_FRAME_HEIGHT;		
			f_event->frame_local = &stream->avp_proc_localcapture_cache;
		
			stream->frameDisplayCallback(stream->ms.ca->cid, f_event);
			free(f_event);
			return 1;

		}

	}

	return 0;
}

/**
 * @brief Tries to read queued video rtp packets from the socket
 *
 * This function reads all awaiting packets on the video rtp session
 * and tries to generate a complete data packet.
 *
 * We need to explain the (simple) algorithm. To send a video frame on a
 * rtp socket, we have to fragment it into multi packets. FFmpeg handles
 * this part by slicing each packet into 500 bytes chunks. The last packet
 * of a serie has its rtp markbit set to 1. We read and copy packets data
 * till we get the markbit. Once we have it, we pass the whole buffer to
 * the decoder, and call the frame display callback (typically set by
 * the user of the PHAPI stack
 *
 * @param stream		The stream to read on
 * @param timestamp		The RTP timestamp to use on recv()
 * @param ts_inc		The ts_inc, to be set to the next ts incr
 *
 */

void
ph_handle_video_network_data(phvstream_t *stream, unsigned long timestamp,
			unsigned long *ts_inc)
{
	struct ph_video_stream_packet *phvs;

	unsigned long rx_observed_timestamp;
	unsigned long rx_observed_seqnumber;
	int rx_observed_markbit;
	mblk_t *mp;
	int flushed;

	*ts_inc = 0;
	while ((stream->ms.running)&&((mp = rtp_session_recvm_with_ts(stream->ms.rtp_session,timestamp))!=NULL)) 
	{


		if (mp->b_cont) {

			rx_observed_timestamp = rtp_get_timestamp(mp);
			rx_observed_seqnumber = rtp_get_seqnumber(mp);
			rx_observed_markbit = rtp_get_markbit(mp);
			
			if (stream->rx_trace_initialize == 0) {
				stream->rx_trace_initialize = 1;
				stream->rx_current_timestamp = rx_observed_timestamp;
				stream->rx_current_seqnumber = rx_observed_seqnumber;
			}
				
			phvs = (struct ph_video_stream_packet *) malloc(sizeof(struct ph_video_stream_packet));
			phvs->mp = mp;
			phvs->timestamp = rx_observed_timestamp;
			phvs->seqnumber = rx_observed_seqnumber;
			osip_list_add(&stream->received_packets_q, phvs, -1);

			if (rx_observed_markbit) {
				flushed = ph_media_video_flush_queue(stream, stream->rx_current_seqnumber, rx_observed_seqnumber);
				stream->rx_trace_initialize = 0;
				*ts_inc = 90000;
				stream->stat_markbit_hit +=1;
				//printf("frame decode hit: (markbit: %d), (ts: %d)\n", stream->stat_markbit_hit, stream->stat_timestamp_hit);
				return;				
			}
			
			// beware of this algorithm with clients that put a different ts on each slice (!)
			if (!rx_observed_markbit && (rx_observed_timestamp > stream->rx_current_timestamp)) {
				flushed = ph_media_video_flush_queue(stream, stream->rx_current_seqnumber, rx_observed_seqnumber-1);
				stream->rx_current_timestamp = rx_observed_timestamp;
				stream->rx_current_seqnumber = rx_observed_seqnumber;
				*ts_inc = 90000;
				stream->stat_timestamp_hit +=1;
				return;
			}

		}
		
	}
		
}

void ph_video_handle_data(phvstream_t *stream) {
	unsigned long rxts_inc = 0;
	int q_size, it;
	phm_videoframe_t *phmvf;
	phm_videoframe_t *fs_phmvf;

	stream->threadcount_encoding += 1;

	if (!stream->ms.running) {
		return;
	}
	
	// lock mutex : mainly used to protect the webcam_frames_q
#ifdef PH_VIDEO_USELOCK
	osip_mutex_lock(stream->mtx);
#endif

/*	
#if USE_HTTP_TUNNEL
	// reset tunnels if ever they go down
	if (ph_media_use_tunnel && (stream->ms.rtp_session->terminated == 1))
	{
		if (stream->ms.tunRtp)
		{
			RtpTunnel *newTun, *old;
			printf("Replacing lost video tunnel\n");
			newTun = rtptun_connect(stream->ms.remote_ip, stream->ms.remote_port);
          
			if (!newTun)
			{
				printf("Video tunnel replacement failed\n");
				osip_mutex_unlock(stream->mtx);
				return;
			}
  
			rtp_session_set_tunnels(stream->ms.rtp_session, newTun, NULL);
  
                
			old = stream->ms.tunRtp;
			stream->ms.tunRtp = newTun;
                
			TUNNEL_CLOSE(old);
			rtptun_free(old);
			stream->ms.rtp_session->terminated = 0;
		}
	}
#endif
*/

	// pull data from the rx rtp stream and push them to consumers (gui,..)
	ph_handle_video_network_data(stream, stream->rxtstamp,
		&rxts_inc);

	// adjust the rtp timestamp synchro for the next pull
	stream->rxtstamp += rxts_inc;
	
	// if the 'onewaycam' frame server is activated, it must produce a frame now
	if ((stream->phmfs_onewaycam.state == 2) && (stream->threadcount_encoding%15==0)) {
		fs_phmvf = (phm_videoframe_t*) malloc(sizeof(phm_videoframe_t));
		fs_phmvf->buffer = stream->phmfs_onewaycam.buffer;
		fs_phmvf->width = 176;
		fs_phmvf->height = 144;
		fs_phmvf->pix_osi = 0;
		fs_phmvf->len = (176*144*3)/2;
		fs_phmvf->freetype = 0;

		osip_list_add(&stream->webcam_frames_q, fs_phmvf, -1);

	}

	
	// iterate over the available local frames in order to flush them into to the encoder
	q_size = osip_list_size(&stream->webcam_frames_q);
	if (q_size > 1) {
		printf("webcam_frames_q overrun - %d\n", q_size);
	}
	for (it = 0; it < q_size; it += 1) {
		phmvf = (phm_videoframe_t *) osip_list_get(&stream->webcam_frames_q, 0);
		if (!phmvf) {
			continue;
		}
		if (it == (q_size-1)) {
			ph_media_video_send_frame(stream, phmvf, 1);
		}

		// freeing the phmvf struct
		if (phmvf->freetype == 1) {
			free(phmvf->buffer);
		}
		if (phmvf->freetype == 2) {
			av_free(phmvf->buffer);
		}
		free(phmvf);

		osip_list_remove(&stream->webcam_frames_q, 0);
	}

	/*
	ph_media_video_send_frame(stream, buf);
	av_free(buf);
	*/

	// HACK : it feels like this should be done somewhere else
	// it is used to be responsive to the user 'flip' option in the gui
	if (stream->wt) {
		webcamSetCameraFlip(stream->wt, phGetConfig()->video_config.video_camera_flip_frame);
	}

	// unlock mutex that was previously locked
#ifdef PH_VIDEO_USELOCK
	osip_mutex_unlock(stream->mtx);
#endif
}


#if defined(VIDEO_IOTHREAD_TIMERS)

void ph_video_io_timer(void *userdata) {
	phvstream_t *stream = (phvstream_t *) userdata;
    if (!stream->ms.running) {
        return;
    }
	ph_video_handle_data(stream);
}
#else
/**
 * @brief Video read thread
 *
 * This thread is responsible for reading frames from the rtp socket and
 * calling the appropriate function. Since we have no real blocking-read
 * in ortp, we also have to sleep() an appropriate ammount of time.
 *
 * @param p		The video stream (type phvstream_t)
 * @return		NULL when the thread is finished
 *
 */

void *
ph_video_io_thread(void *p)
{
	phvstream_t *stream = (phvstream_t*)p;
	unsigned long rxtstamp = 0, txtstamp = 0;
	struct timeval sleeptime,start_time, end_time;
	struct timespec sleepns;
	unsigned long rxts_inc = 0;
	int needsleep, err;

	while (stream->ms.running) {
		stream->rxts_inc = 0;
		needsleep = 1;

		gettimeofday(&start_time, 0);

#ifdef WIN32
			Sleep(10);
#else
			usleep(105000);
#endif

        if (!stream->ms.running)
            break;

		ph_video_handle_data(stream);
        
		gettimeofday(&end_time, 0);
		tvsub_phapi(&end_time, &start_time);

		if (end_time.tv_usec < 10000) {		       		
			sleeptime.tv_usec = 10000; sleeptime.tv_sec = 0;
			tvsub_phapi(&sleeptime, &end_time);

            if (sleeptime.tv_sec <= -1)
                sleeptime.tv_sec = 0;
            
			TIMEVAL_TO_TIMESPEC(&sleeptime, &sleepns);
#ifdef WIN32
			Sleep(sleepns.tv_sec * 1000 + sleepns.tv_nsec / 1000000);
#else
			nanosleep(&sleepns, 0);
#endif
		}

		rxtstamp += rxts_inc;
	}

	return NULL;
}
#endif
/**
 * @brief Stops the video stream
 *
 * This is called by ph_media_stop() when the call is finished and that video
 * has been negotiated and started. It stops the webcam driver, waits for
 * thread to finish, and destroys the rtp session
 *
 * @param ca		The call on which we need to stop the video stream
 *
 */

void ph_media_video_stop(phcall_t *ca)
{
  phvstream_t *stream = (phvstream_t *) ca->ph_video_stream;
 

  if (!stream)
      return;

	stream->ms.running = 0;
#if defined(VIDEO_IOTHREAD_TIMERS)
	stream->t_impl->timer_stop(stream->v_timer);
	stream->t_impl->timer_destroy(stream->v_timer);
#else
	osip_thread_join(stream->media_io_thread);
	osip_free(stream->media_io_thread);
#endif
  
	if (phcfg.video_config.video_line_configuration == PHAPI_VIDEO_LINE_AUTOMATIC) {
		osip_thread_join(stream->media_bw_control_thread);
	}

	if (stream->phmfs_webcam.state == 2) {
		webcamClose(stream->wt);
		stream->phmfs_webcam.state = 0;
	}

	// HACK (the convertImage function is always used...)
	webcamUninitialize(stream->wt);
	stream->wt = 0;

	if (stream->phmfs_onewaycam.state == 2) {
		av_free(stream->phmfs_onewaycam.buffer);
		stream->phmfs_onewaycam.state = 0;
	}

 if (stream->ms.codec->encoder_cleanup)
    stream->ms.codec->encoder_cleanup(stream->ms.encoder_ctx);

 if (stream->ms.codec->decoder_cleanup)
    stream->ms.codec->decoder_cleanup(stream->ms.decoder_ctx);

	ph_media_video_free_processing_buffers(stream);
 
	ca->ph_video_stream = 0;

	ortp_set_debug_file("oRTP", stdout); 
	ortp_session_stats_display(stream->ms.rtp_session);
	ortp_set_debug_file("oRTP", NULL); 

  rtp_session_destroy(stream->ms.rtp_session);


#ifdef USE_HTTP_TUNNEL
  if (stream->ms.tunRtp)
  {
	  TUNNEL_CLOSE(stream->ms.tunRtp);
	  rtptun_free(stream->ms.tunRtp);
  }
#endif
  
	memset(stream, 0, sizeof(phvstream_t));
	osip_free(stream);
}
