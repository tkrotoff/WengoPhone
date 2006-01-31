/*
 * phmedia -  Phone Api media streamer
 *
 * Copyright (C) 2005 Wengo SAS
 * Copyright (C) 2004 Vadim Lebedev <vadim@mbdsys.com>
 * Copyright (C) 2002,2003   Aymeric Moizard <jack@atosc.org>
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

#include "phglobal.h"
#include <osip2/osip_mt.h>
#include <osip2/osip.h>
#ifdef OS_POSIX
#include <sys/ioctl.h>
#include <sys/time.h>
#endif
#include <fcntl.h>
#include <stdlib.h>
#include <ortp.h>
#include <ortp-export.h>
#include <telephonyevents.h>
#include <assert.h>

#include "phapi.h"
//#include "phcall.h"
#include "phmedia.h"
#include "phcodec.h"
#include "tonegen.h"
#include "phmbuf.h"

#include "phmstream.h"
#include "phastream.h"
#include "phaudiodriver.h"

#ifdef USE_SPXEC
#include "spxec.h"
#endif

#ifdef PH_USE_RESAMPLE
#include <samplerate.h>
#endif

static void no_printf(const char *x, ...) { }
 
#define ph_printf  printf

#define RTP_SESSION_LOCK(x)
#define RTP_SESSION_UNLOCK(x)

#ifdef __GCC__
#define DBG(x...)  ph_printf(x)
#define DBG_SILOK(x...)
#else
#define DBG   no_printf
#define DBG_SILOK  no_printf
#define DBG_DYNA_RX  no_printf
#define DBG_ECHO  no_printf
#endif


#ifdef DO_ECHO_CAN

/*
#define ECHO_SYNC_LOCK(x) if (s->ec) g_mutex_lock(s->ecmux)
#define ECHO_SYNC_UNLOCK(x) if (s->ec) g_mutex_unlock(s->ecmux)
*/
#define ECHO_SYNC_LOCK(x)
#define ECHO_SYNC_UNLOCK(x)

#define NO_ECHO__SUPPRESSOR 1	
#define abs(x) ((x>=0)?x:(-x))

void *create_AEC(int softboost, int smaplingrate);
short do_AEC(void *ec, short x, short y);
void kill_AEC(void *ec);
#endif


#define CNG_TBL_SIZE 128
/* table for CNG generation */

const unsigned int tab_tx_cng[CNG_TBL_SIZE]={
2238721,1995262,1778279,1584893,1412538,1258925,1122018,1000000,
891251,794328,707946,630957,562341,501187,446684,398107,
354813,316228,281838,251189,223872,199526,177828,158489,
141254,125893,112202,100000,89125,79433,70795,63096,
56234,50119,44668,39811,35481,31623,28184,25119,
22387,19953,17783,15849,14125,12589,11220,10000,
8913,7943,7079,6310,5623,5012,4467,3981,
3548,3162,2818,2512,2239,1995,1778,1585,
1413,1259,1122,1000,891,794,708,631,
562,501,447,398,355,316,282,251,
224,200,178,158,141,126,112,100,
89,79,71,63,56,50,45,40,
35,32,28,25,22,20,18,16,
14,13,11,10,9,8,7,6,
6,5,4,4,4,3,3,3,
2,2,2,2,1,1,1,0
};

/* table for CNG applying */
unsigned int tab_rx_cng[CNG_TBL_SIZE];
static short sil_pkt[MAX_FRAME_SIZE];

static int ph_speex_hook_pt = -1;  /* payload code to be replaced by SPEEX WB */ 
static int ph_trace_mic = 0;       /* when nonzero show mean MIC signal level each sec */ 

static int ph_audio_play_cbk(phastream_t *stream, void *playbuf, int playbufsize);
static int ph_generate_comfort_noice(phastream_t *stream, void *buf);

int ph_ortp_session_object_size = sizeof(RtpSession);

static void cb_put(struct circbuf *cb, char *data, int len);
static void cb_get(struct circbuf *cb, char **chunk1, int *chunk1len,  char **chunk2, int *chhunk2len, int len);
static void cb_init(struct circbuf *cb, int size);
static void cb_clean(struct circbuf *cb);




static void 
cb_put(struct circbuf *cb, char *data, int len)
{
    int free = cb->cb_siz - cb->cb_cnt;
    int chunk1max, chunk2max;

    if (len > cb->cb_siz)
        len = cb->cb_siz;

    if (len > free)
    {
        /* 
            not enough free space in the buffer,
            we need to free it
        */
        int needmore = len - free;

        /* advance the read pointer over data we're going to override */
        cb->cb_cnt -= needmore;
        cb->cb_rdx += needmore;
        if (cb->cb_rdx >= cb->cb_siz)
            cb->cb_rdx -= cb->cb_siz;
    }

    chunk1max = cb->cb_siz - cb->cb_wrx;
    if (len < chunk1max)
        chunk1max = len;

    chunk2max = len - chunk1max;

    
    memcpy(cb->cb_buf+cb->cb_wrx, data, chunk1max);

    cb->cb_cnt += chunk1max;
    cb->cb_wrx += chunk1max;
    if (cb->cb_wrx == cb->cb_siz)
        cb->cb_wrx = 0;

    
    if (chunk2max <= 0)
        return;

    memcpy(cb->cb_buf+cb->cb_wrx, data+chunk1max, chunk2max);

    cb->cb_cnt += chunk2max;
    cb->cb_wrx += chunk2max;
    return;
}


static void 
cb_get(struct circbuf *cb, char **chunk1, int *chunk1len,  char **chunk2, int *chunk2len, int len)
{
    int chunk1max = cb->cb_siz - cb->cb_rdx;

    if (len > cb->cb_cnt)
        len = cb->cb_cnt;

    *chunk2 = 0;
    *chunk2len = 0;


    *chunk1 = cb->cb_buf + cb->cb_rdx;
    if (len <= chunk1max)
    {
        *chunk1len = len;
        cb->cb_cnt -= len;
        cb->cb_rdx += len;
        if (cb->cb_rdx == cb->cb_siz)
            cb->cb_rdx = 0;
        return;
    }
    
    *chunk1len = chunk1max;
    cb->cb_rdx = 0;
    cb->cb_cnt -= chunk1max;
    
    len -= chunk1max;


    *chunk2 = cb->cb_buf;
    *chunk2len = len;
    cb->cb_cnt -= len;
    cb->cb_rdx += len;
}	



static char zeroes[] =
  {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  };

void 
cb_zfill(struct circbuf *cb, int len)
{
  int x;
  const int mx = sizeof(zeroes);

  while(len > 0)
    {
      x = (len > mx) ? mx : len;

      cb_put(cb, zeroes, mx);
      len -= x;
    }
}



static void 
cb_init(struct circbuf *cb, int size)
{
    memset(cb, 0, sizeof(0));
    cb->cb_buf = (char *) g_malloc(size);
    cb->cb_siz = size;
}

static void 
cb_clean(struct circbuf *cb)
{
    g_free(cb->cb_buf);
}





phcodec_t *ph_media_lookup_codec(int payload);
void *ph_audio_io_thread(void *_p);



void ph_gen_silence()
{
    int i;
    short *p = sil_pkt;
    for(i=0; i < MAX_FRAME_SIZE; i++)
      {
    *p++ = -32767;      /* lowest value */
      } 
}

#define NOISE_LEN       0x4000
#define NOISE_B_LEN     (NOISE_LEN*2)

static short noise_pattern[NOISE_LEN];
static unsigned short noise_max;

static unsigned int normalize(unsigned int maxval)
  {
    int i=31;
    if(maxval < 0x7FFF)
      return 0;
    while((1<<i & maxval) == 0)
      i--;
    return(i-15);
  }

/* read file to choose 'friendly' white noise */ 

void ph_gen_noise()
{
  int i;
  unsigned int norm;
   unsigned long sum=0;

  if(1)
    {
      norm = normalize(RAND_MAX);
      DBG_SILOK("no NOISE file, using random normalized %u\n", norm);
      for(i=0; i<NOISE_LEN; i++)
      noise_pattern[i] =  rand()>>norm;
    }
  for(i=0; i<NOISE_LEN; i++)
    {
      if(noise_max < abs(noise_pattern[i]))
    noise_max = abs(noise_pattern[i]);
      sum += abs(noise_pattern[i]);
    }
  DBG_SILOK("max noise %u mean %u\n", noise_max, sum/NOISE_LEN);
}


static void
ph_on_cng_packet(RtpSession *rtp_session, mblk_t *mp, struct ph_msession_s *s)
{
  short *noise;
  char *p;
  int i;
  unsigned short factor;
  unsigned int cng_level;
  phastream_t *stream = (phastream_t *)s->streams[PH_MSTREAM_AUDIO1].streamerData;
  if(stream->ms.running && stream->cngi.cng && mp != NULL)
    {
      /* first byte contains level, following fields (if any) are discarded */
      p = mp->b_rptr + RTP_FIXED_HEADER_SIZE;
      cng_level =  tab_tx_cng[*p]>>stream->cngi.long_pwr_shift;
      if(!cng_level)
    cng_level = 1;
      factor = noise_max/cng_level;
      if(!factor)
    factor=1;

      CNG_LOCK(stream);
      if(stream->ms.running && stream->cngi.noise)
    {
      noise = (short *)stream->cngi.noise;
      for(i=0; i<NOISE_LEN; i++)
        {
          noise[i] =  noise_pattern[i]/factor;
        }
    }
      CNG_UNLOCK(stream);
      DBG_SILOK("PHMEDIA:got CNG %u -> %u -> %u factor %u\n",*p, tab_tx_cng[*p], cng_level, factor); 
      stream->cngi.got_cng =  1;
    }else{
      DBG_SILOK("PHMEDIA:got CNG, discarding\n");
    } 
}
void 
dbgbpt1(void)
{
}
void 
dbgbpt2(void)
{
}


#ifdef DO_ECHO_CAN

void do_echo_update(phastream_t *s, char *data, int len);
void store_pcm(phastream_t *s, char *buf, int len);


void store_pcm(phastream_t *s, char *buf, int len)
{

  if (!s->ec)
      return;

  ECHO_SYNC_LOCK(s);
  if (s->underrun)
    {
      int used;
      
 //     audio_stream_get_out_space(s, &used);
 //     printf("Detected Underrun: used = %d lat = %d\n", used, s->audio_loop_latency);
      cb_zfill(&s->pcmoutbuf, s->audio_loop_latency);
      s->underrun = 0;
    }

  cb_put(&s->pcmoutbuf, buf, len);
  s->sent_cnt += len;
  ECHO_SYNC_UNLOCK(s);    
  DBG_ECHO("PUT read, recv, sent: %d, %d, %d\n", s->read_cnt, s->recv_cnt, s->sent_cnt);
}



#define AEC do_AEC

void do_echo_update(phastream_t *s, char *micdata, int length)
{
  char *spkchunk1, *spkchunk2;
  int  spklen1, spklen2;
  short *spkpcm1, *spkpcm2;
  short *micpcm;
  int total = 0;
  int savedlen = length/2;
  short tmp;
  int used = 0;
#ifdef USE_SPXEC
  char tmpspk[2048];
  char tmpmic[2048];
#endif
	
  if (!s->ec)
    return;

  DBG_ECHO("echo pointers: %d, %d, %d\n", 2*s->spk_current_sample - s->read_cnt, length, s->sent_cnt - 2*s->mic_current_sample);
	// echo critical section : recovering the data that was previously saved from the speaker
    ECHO_SYNC_LOCK(s);
	s->recv_cnt += length;
	//DBG_ECHO("length: %d\n", length);
    cb_get(&s->pcmoutbuf, &spkchunk1, &spklen1, &spkchunk2, &spklen2, length);
    s->read_cnt += (spklen1 + spklen2);
    ECHO_SYNC_UNLOCK(s);
    DBG_ECHO("GET read (just read) - recv, sent (diff): %d (%d), - %d, %d (%d)\n", s->read_cnt, (spklen1 + spklen2), s->recv_cnt, s->sent_cnt, s->recv_cnt - s->sent_cnt);
    if (spklen1 + spklen2 < length)
      {
	s->underrun = 1;
#if 0
	audio_stream_get_out_space(s, &used); 
	DBG_ECHO("UNDERRUN: current out queue length: %d \n", used);
#endif
      }


    
  
    if (spklen1 <= 0)
      return;

#if PSEUDO_AEC
    return;
#endif


#ifdef USE_SPXEC
    if (spklen1)
      {
	memcpy(tmpspk, spkchunk1, spklen1);
	if (spklen2)
	  memcpy(tmpspk+spklen1, spkchunk1, spklen2);
      }


    if (spklen1 + spklen2 < length)
      memset(tmpspk+spklen1+spklen2, 0, length - (spklen1 + spklen2));
#endif


    micpcm = (short *) micdata;
    length /= 2;

    spkpcm1 = (short *) spkchunk1;
    spklen1 /= 2;

    spkpcm2 = (short *) spkchunk2;
    spklen2 /= 2;

    
#ifdef USE_SPXEC

    total = spklen1 + spklen2;
    spxec_echo_cancel(s->ec, (short *) tmpspk, micpcm, (short *) tmpmic, 0);
    if (s->activate_recorder)
      {
	short *cleansignal = (short *) tmpmic;
	int N = length;

	spkpcm1 = (short *) tmpspk;
	while(N--)
	  {
	    ph_media_audio_recording_record_one(&s->recorder, *spkpcm1++, *micpcm++,  *cleansignal++);
	  }

	
      }
	
#else 
    
    // adjust mic through echo cancellation. reference signal is the first chunk of the stored circular buffer
    // (this is the regular, common case)
    length -= spklen1;
    while(spklen1--) 
      {
	tmp = AEC(s->ec, *spkpcm1, *micpcm);
	
	if (s->activate_recorder)
	  ph_media_audio_recording_record_one(&s->recorder, *spkpcm1, *micpcm,  tmp);
	*micpcm++ = tmp;
	spkpcm1++;
	total++;
      }

	// adjust mic through echo cancellation. reference signal is the second chunk of the stored circular buffer
    length -= spklen2;
    while(spklen2--) 
      {
	tmp = AEC(s->ec, *spkpcm2, *micpcm);
	
	if (s->activate_recorder)
	  ph_media_audio_recording_record_one(&s->recorder, *spkpcm2, *micpcm,  tmp);
	*micpcm++ = tmp;
	spkpcm2++;
	total++;
      }

	// adjust mic through echo cancellation. reference signal is set to 0
	// (this is the unwanted worst case scenario found if there is a speaker underrun)
    while(length--) 
      {
	tmp = AEC(s->ec, 0, *micpcm);
	
	if (s->activate_recorder)
	  ph_media_audio_recording_record_one(&s->recorder, 0, *micpcm,  tmp);
	*micpcm++ = tmp;
	total++;
      }

#endif
    if (total > savedlen)
      {
	DBG("do_echo_update: total=%d savedlen=%d\n", total, savedlen);
      }

    

}

#endif /* DO_ECHO_CAN */

#define TRACE_POWER 1

#ifdef TRACE_POWER
static unsigned int min_pwr;
static unsigned int max_pwr;
static int max_sil;
void print_pwrstats(phastream_t *s)
{
  if(s->cngi.pwr_size)
    printf("\nPWR SUM: min %x max %x mean %x max_sil_cnt %d\n", min_pwr/s->cngi.pwr_size, max_pwr/s->cngi.pwr_size,  
       s->cngi.mean_pwr/s->cngi.pwr_size, max_sil); 
}
#endif

static int
calc_shift(int val)
{
  int ret=0;
  while((val=(val/2)))
    ret++;

  return ret;
}

static int
ph_vad_update(phastream_t *as, char *data, int len)
{
  int i;
  unsigned int power;
  short *p = (short *)data; 
  struct vadcng_info *s = &as->cngi;
  static int tracecnt = 0;
  
  /* calc mean power */
  for(i = 0; i<len/2; i++)
    {
      s->mean_pwr -= s->pwr[s->pwr_pos];
      //      s->pwr[s->pwr_pos] = p[i]*p[i];   
      s->pwr[s->pwr_pos] = abs(p[i]);          // only magnitude, not power 
      s->mean_pwr += s->pwr[s->pwr_pos];       // no overflow as long as pwr size is less then 65536 */
   
#ifdef TRACE_POWER
      if(s->mean_pwr > max_pwr)
	max_pwr = s->mean_pwr;

      if(s->mean_pwr < min_pwr)
	min_pwr = s->mean_pwr;
#endif
      s->pwr_pos++;
      if(s->pwr_pos >= s->pwr_size)
	{
	  s->pwr_pos = 0;
	  if(s->cng)
	    {
	      /* now update the "long" mean power, which is the sum of 64 mean powers calculated each pwr_size samples */
	      s->long_mean_pwr -= s->long_pwr[s->long_pwr_pos];
	      s->long_pwr[s->long_pwr_pos] =  s->mean_pwr>>s->pwr_shift;  
	      s->long_mean_pwr += s->long_pwr[s->long_pwr_pos];
	      s->long_pwr_pos++;
	      if(s->long_pwr_pos >= LONG_PWR_WINDOW)
		s->long_pwr_pos = 0;
	    }
	}
    }
  /* mean power in last PWR_WINDOW ms */
  power = s->mean_pwr>>s->pwr_shift;
  /* compare with threshold */
  if(power > s->pwr_threshold)
    {
    s->sil_cnt = 0;
  }
  else
    {
      s->sil_cnt += len/2;
    }
#ifdef TRACE_POWER
  if(s->sil_cnt > max_sil)
    max_sil = s->sil_cnt;
  
  if (ph_trace_mic && (tracecnt++ == 50))
  {
      ph_printf("ph_media_audiuo: mean MIC signal: %d\n", power);
      tracecnt = 0;
  }
#endif
  
  if(s->sil_cnt > s->sil_max)
    return 1;
  else
    return 0;
}

char find_level(unsigned int pwr)
{
  int index=-1;
  int left=1;
  int right=CNG_TBL_SIZE;
  int mid;
  
  while(1)
    {
      if(left > right)
    break;
      mid = (left+right)>>1;
      if(pwr >= tab_tx_cng[mid] && pwr < tab_tx_cng[mid-1])
    {
      index = mid;
      break;
    }
      else
    {
      if(pwr > tab_tx_cng[mid])
        right = mid-1;
      else
        left = mid+1;
    }
    }
  return index;
}

static void
ph_send_cng(phastream_t *stream, unsigned long timestamp)
{
  mblk_t *mp;
  char level;
  
  /* calculate level in dBov */ 
  level = find_level(stream->cngi.long_mean_pwr);
  if (level < 0)
     {
       ph_printf("cng db invalid\n");
       return;
     }
   DBG_SILOK("PHMEDIA:send CNG %d\n", level);
  /* send CNG packet */
   mp = rtp_session_create_specific_payload_packet(stream->ms.rtp_session, RTP_FIXED_HEADER_SIZE, stream->cngi.cng_pt, &level, 1);
   if (mp != NULL)
       {
       rtp_session_sendm_with_ts(stream->ms.rtp_session, mp, timestamp);
       }
}

static int
ph_generate_comfort_noice(phastream_t *stream, void *buf)
{
    struct timeval now, diff;
    phcodec_t *codec = stream->ms.codec;
    
    gettimeofday(&now, 0);
    ph_tvdiff(&diff, &now, &stream->last_rtp_recv_time);
    if (diff.tv_usec > NOISE_START_DELAY)
       { 
       int lg,lg2,ret; // length in shorts
       int used = 0;
       
       ret = audio_stream_get_out_space(stream, &used);
       if (ret < 0)
           {
            no_printf("IOCTL error");
            return 0;
           }
        /* if less than 200ms of voice, send noise */
        if (used < codec->decoded_framesize * 10)
            {
            /* leave place for 2 voice frame */
            //		lg = info.bytes - 2*codec->decoded_framesize;
            lg = 2*codec->decoded_framesize;
            if (ret < lg)
                lg = used;

            if (lg > NOISE_B_LEN)
                lg = NOISE_B_LEN;

            if (lg <= (NOISE_B_LEN - stream->cngi.nidx))
                {
                 memcpy(buf, &stream->cngi.noise[stream->cngi.nidx], lg);  
                 stream->cngi.nidx += lg;

                 if (stream->cngi.nidx >=  NOISE_B_LEN)
                     stream->cngi.nidx = 0;
                }
            else
                {
                 lg2 = lg - (NOISE_B_LEN - stream->cngi.nidx);
                 memcpy(buf, &stream->cngi.noise[stream->cngi.nidx], lg-lg2);  
                 memcpy(lg - lg2 + (char*) buf, stream->cngi.noise, lg2);
                 stream->cngi.nidx = lg2;
                }
            }
            return lg;
        }
  
    return 0;
}

static void
ph_handle_network_data(phastream_t *stream)
{
  char data_in_dec[1024];
  int len;
  phcodec_t *codec = stream->ms.codec;
  int played = 0;  
  int freespace;
  int usedspace;
  struct timeval now, now2;
	  
  DBG_DYNA_RX("ph_handle_network_data :: start\n");
  DBG_ECHO("echo cirbuf size %d\n", stream->sent_cnt - stream->read_cnt);

#if 0
  freespace = audio_stream_get_out_space(stream, &used); 
  if (used*2 >= codec->decoded_framesize)
    return;

 freespace = audio_stream_get_out_space(stream, &usedspace);
 if (!freespace)
     return;
#endif
  
	// we try to read and write on the speaker packets from the network
 while (stream->ms.running)
   {
     int used;
	  
     gettimeofday(&now, 0);
     // read pack
     len = ph_audio_play_cbk(stream, data_in_dec, codec->decoded_framesize);
     DBG_DYNA_RX("ph_handle_network_data:%u.%u :: read %d full size packets\n", now.tv_sec, now.tv_usec, len/codec->decoded_framesize);
     
     if (!len)
       break;
     
     played += len;
     //freespace -= len;
     
     // write the decoded audio onto the speaker(out) device
     len = audio_stream_write(stream, data_in_dec, len);
     if (!len)
       break;
#ifdef DO_ECHO_CAN
     if (!stream->using_out_callback)
       store_pcm(stream, data_in_dec, len);
#endif
     
     // exit loop if we've played 4 full size packets
     if (played >= codec->decoded_framesize * 4)
       break;
     
     // exit loop if we waited too much in the audio_stream_write
     gettimeofday(&now2, 0);
     if (now2.tv_sec > now.tv_sec || (now2.tv_usec - now.tv_usec) >= 10000)
       break;
     
     // while loop: we try to get more data from the RX path
     
   }
 
 DBG_DYNA_RX("ph_handle_network_data :: end\n");
      
} 


#ifdef PH_USE_RESAMPLE
void ph_audio_resample(void *ctx, void *inbuf, int inbsize, void *outbuf, int *outbsize)
{
  float finbuf[2048];
  float foutbuf[2048];
  SRC_DATA *sd = (SRC_DATA *)ctx;
  static int cnt = 20;
  
  
  
  
  sd->data_in = finbuf;
  sd->input_frames = inbsize / 2;

  sd->data_out = foutbuf;
  sd->output_frames = 2048;

  src_short_to_float_array((short *)inbuf, finbuf, sd->input_frames);
  src_simple(sd, SRC_LINEAR, 1);
  src_float_to_short_array(finbuf, (short *)outbuf, sd->output_frames_gen);
  *outbsize =  sd->output_frames_gen * 2;


  if (cnt <= 2)
     ph_printf("resample: ratio=%f in=%d out=%d\n", sd->src_ratio, 
  	sd->input_frames, sd->output_frames_gen);

  cnt--;
  if (cnt <= 0)
	  cnt = 20; 

}

#endif

  

static int
ph_audio_play_cbk(phastream_t *stream, void *playbuf, int playbufsize)
{
  int len;
  mblk_t *mp;
  rtp_header_t *rtp;
  phcodec_t *codec = stream->ms.codec;
  int framesize = codec->decoded_framesize;
  struct timeval now;
  int iter = 0;
  int ts_inc = 0;
  int played = 0;

#ifdef PH_USE_RESAMPLE
  unsigned char resampleBuf[2000];
  int resampledSize;
  char *savedPlayBuf = (char *) playbuf;
  int savedBufSize = playbufsize;
  int needResample;
  int resampledLen;
  
  if ((needResample = (stream->clock_rate != stream->actual_rate)))
    {
      playbuf = resampleBuf;
      playbufsize = sizeof(resampleBuf);
      //recordbuf = resampledBuf;
    }
#endif


  // while start :
  //		read packets from the network
  //		the best case scenario is that one packet is ready to go
  //		we do not try to read more than 4 packets in a row
  while (stream->ms.running && (playbufsize >= framesize))
    {
    mp=rtp_session_recvm_with_ts(stream->ms.rtp_session,stream->ms.rxtstamp);
    if (!mp)
        break;


    len=mp->b_cont->b_wptr-mp->b_cont->b_rptr;
    rtp=(rtp_header_t*)mp->b_rptr;
    if ( rtp->paytype != stream->ms.payload )
      {
	ph_printf("wrong audio payload: %d expecting %d\n", rtp->paytype, stream->ms.payload);
	freemsg(mp);
	continue;
      }

     
    no_printf("got %d bytes from net(%d)\n", len, ++iter);
    if (!stream->ms.running)
        break;
    
    if (len)
        len = codec->decode(stream->ms.decoder_ctx, mp->b_cont->b_rptr, len, playbuf, playbufsize);

    
    if (len > 0)
      {
	no_printf("Playing %d bytes from net\n", len);

	/* if we're in Half Duplex mode, attenuate speaker signal */
	if (stream->hdxmode && !stream->hdxsilence)
	  {
	    short *samples = (short *) playbuf;
	    int nsamples = len >> 1;
	    const int HDXSHIFT = 7;

	    while(nsamples--)
	      {
		*samples = *samples >> HDXSHIFT;
		samples++;
	      }
	  }


#ifdef DO_ECHO_CAN
	if (stream->using_out_callback)
	  store_pcm(stream, playbuf, len);
#endif
	
        /* save played data */    
        if (stream->lastframe)
	  memcpy(stream->lastframe, playbuf, len);
            
        ts_inc += len/2;
        played += len;
        }
     
    freemsg(mp);

      
    /* exit loop if we've played 4 full size packets */
    if (ts_inc >= codec->decoded_framesize * 4 / 2)
       break;

#ifdef PH_USE_RESAMPLE
	if (needResample)
	{
	  ph_audio_resample(stream->play_resample_ctx, playbuf, len, savedPlayBuf, &resampledLen);
	  savedPlayBuf += resampledLen;
	  savedBufSize -= resampledLen;
	  if (savedBufSize <= 0)
	    break;
	}
	else
#endif
	{
	  // updating the placeholder for the next decode loop
	  playbuf = len + (char *) playbuf;
	  playbufsize -= len;
	}
	}
	// while end : read packets from the network
   
    gettimeofday(&now, 0);
    if (ts_inc == 0)
        {
      /* if no data for at least NOISE_START_DELAY ms, start sending noise audio data */
        if (stream->ms.running && stream->cngi.cng)
            {
            int len;
                
            len = ph_generate_comfort_noice(stream, playbuf);
            if (len)
                {
#ifdef DO_ECHO_CAN
		  if (stream->using_out_callback)
		    store_pcm(stream, playbuf, len);
#endif
                 }
            played += len;
            playbufsize -= len;
            }
        }
    else
        {
        stream->last_rtp_recv_time = now;
        }
     
    stream->ms.rxts_inc = ts_inc;
    stream->ms.rxtstamp += ts_inc;

#ifdef PH_USE_RESAMPLE
    if (needResample)
      {
	ph_audio_resample(stream->play_resample_ctx, playbuf, len, savedPlayBuf, &resampledLen);
      }
#endif
    
    if (stream->lastframe != 0 && playbufsize)
    {
        /* we did no fill the buffer completely */
        int morebytes = playbufsize;
        
        if (morebytes > codec->decoded_framesize)
            morebytes = codec->decoded_framesize;
        
        
#ifdef DO_ECHO_CAN
	if (stream->using_out_callback)
	  store_pcm(stream, playbuf, morebytes);
#endif

        
        played += morebytes;
    }        
    return played;    
} 




int ph_audio_rec_cbk(phastream_t *stream, void *recordbuf, int recbufsize)
{
    phcodec_t *codec = stream->ms.codec;
    int framesize = codec->decoded_framesize;
    int processed = 0;
    int silok = 0;
    char data_out_enc[1000];
    struct timeval diff;
    int wakeup = 0;
#ifdef PH_USE_RESAMPLE
    unsigned char resampleBuf[1000];
    int resampledSize;

    if (stream->clock_rate != stream->actual_rate)
      {
	ph_audio_resample(stream->rec_resample_ctx, recordbuf, recbufsize, resampleBuf, &resampledSize);
	recbufsize = resampledSize;
	recordbuf = resampleBuf;
      }
#endif

    while(recbufsize >= framesize)
        {
        gettimeofday(&stream->now,0);
           
#ifdef DO_ECHO_CAN
        do_echo_update(stream, recordbuf, framesize);
#endif
        if (stream->cngi.vad)
            {
            stream->hdxsilence = silok = ph_vad_update(stream, recordbuf, framesize);
            if (!stream->cngi.cng && silok)
                {
                /* resend dummy CNG packet only if CNG was not negotiated */
                ph_tvdiff(&diff, &stream->now, &stream->last_rtp_sent_time);
                wakeup = (diff.tv_sec > RTP_RETRANSMIT);
                }
            }
	else if (stream->hdxmode)
	  {
	    int hdxsil = ph_vad_update(stream, recordbuf, framesize);
	    if (hdxsil != stream->hdxsilence)
	      ph_printf("phmedia_audio: HDXSIL=%d\n", hdxsil);

	    stream->hdxsilence = hdxsil;
	  }
            
        if ((stream->dtmfi.dtmfg_phase != DTMF_IDLE) || (stream->dtmfi.dtmfq_cnt != 0))
            {
            ph_generate_out_dtmf(stream, (short *) recordbuf, framesize/2, stream->ms.txtstamp);
            silok = 0;
            }


        if (stream->mixbuf)
            {   
            int n = ph_mediabuf_mixaudio(stream->mixbuf, (short *)recordbuf, framesize/2);
            if (!n)
                {
                ph_mediabuf_free(stream->mixbuf);
                stream->mixbuf =  0;
                }
            else
                stream->hdxsilence = silok = 0;
            }

        if (!silok)
            {
            int enclen = codec->encode(stream->ms.encoder_ctx, recordbuf, framesize, data_out_enc, sizeof(data_out_enc));
            if (silok != stream->cngi.lastsil || wakeup)
                {
                rtp_session_set_markbit(stream->ms.rtp_session, 1);
                rtp_session_send_with_ts(stream->ms.rtp_session, data_out_enc, enclen, stream->ms.txtstamp);
                rtp_session_set_markbit(stream->ms.rtp_session, 0);
                }
            else
                {
                rtp_session_send_with_ts(stream->ms.rtp_session, data_out_enc, enclen, stream->ms.txtstamp);
                }
            stream->last_rtp_sent_time = stream->now;
            }
        else if (stream->cngi.cng)
            {
            ph_tvdiff(&diff, &stream->now, &stream->cngi.last_dtx_time);
            if (diff.tv_sec >= DTX_RETRANSMIT)
                {
                ph_tvdiff(&diff, &stream->now, &stream->last_rtp_sent_time);
                if (diff.tv_sec >= DTX_RETRANSMIT)
                    {
                    ph_send_cng(stream, stream->ms.txtstamp);
                    stream->cngi.last_dtx_time = stream->now;
                    }
                }
            
            if (wakeup)
                {
                /* send cng packet with -127dB */ 
                ph_send_cng(stream, stream->ms.txtstamp);
                stream->last_rtp_sent_time = stream->now;
                }
            }
            
        stream->cngi.lastsil = silok;
        stream->ms.txtstamp += framesize/2;
        recbufsize -= framesize;
        processed += framesize;
        recordbuf = framesize + (char *)recordbuf;
        }   
        
    return processed;
}


static int 
ph_audio_callback(phastream_t *stream, void *recbuf, int recbufsize, void *playbuf, int *playbufsize)
{
    int i = 0;
    
    if (recbuf && recbufsize > 0)
        ph_audio_rec_cbk(stream, recbuf, recbufsize);

    if ((playbuf != 0) && playbufsize && (*playbufsize > 0))
        {
        i = ph_audio_play_cbk(stream, playbuf, *playbufsize);
        *playbufsize = i;
        }
        
    return 0;

}

static int
ph_handle_audio_data(phastream_t *stream)
{
  char data_out[1000];
  phcodec_t *codec = stream->ms.codec;
  const int framesize = codec->decoded_framesize;
  int i;

    
  no_printf("Reading Got %d bytes from mic\n", framesize);
  i=audio_stream_read(stream, data_out, framesize);
  no_printf("Got %d bytes from mic\n", i);
  if (i>0)
    {        
      i = ph_audio_rec_cbk(stream, data_out, i);
    }
  return (i>0);
}


#define SLEEP_TIME_US  10000

void *
ph_audio_io_thread(void *p)
{
  phastream_t *stream = (phastream_t*)p;
  struct timeval sleeptime, start_time, end_time;
  struct timespec sleepns;
  unsigned long rxts_inc = 0;
  int needsleep;

  if (stream->ms.media_io_thread)
	  osip_thread_set_priority(stream->ms.media_io_thread, -19);

  ph_printf("new media io thread started\n");



  while (stream->ms.running)
    {
     needsleep = 1;

     gettimeofday(&start_time, 0);

     no_printf("audio looop\n");
     if (!audio_driver_has_play_callback())
        ph_handle_network_data(stream);
     if (!audio_driver_has_rec_callback())
         ph_handle_audio_data(stream);

     gettimeofday(&end_time, 0);
     ph_tvsub(&end_time, &start_time);
     if (end_time.tv_usec < SLEEP_TIME_US)
       {		       		
     sleeptime.tv_usec = SLEEP_TIME_US; sleeptime.tv_sec = 0;
     ph_tvsub(&sleeptime, &end_time);

     TIMEVAL_TO_TIMESPEC(&sleeptime, &sleepns);


     if (stream->ms.running)
#ifdef OS_WINDOWS
       Sleep(sleeptime.tv_usec / 1000);
#else
       nanosleep(&sleepns, 0);
#endif     
       }
    }
  ph_printf("media io thread stopping\n");
  dbgbpt2();
  return NULL;
}


/* 
 * mix a DTMF signal into the given signal buffer
 *
*/
void ph_generate_out_dtmf(phastream_t *stream, short *signal, int siglen, long timestamp)
{
  int mixlen, mixn;
  unsigned short  dtmfDigit;
  const int DTMF_MSECS = 240;
  int samples, rate;
  struct dtmf_info *dtmfp = &stream->dtmfi;


  rate = stream->clock_rate;
  samples = rate/1000;
dtmf_again:      
  switch (dtmfp->dtmfg_phase)
    {
    case DTMF_IDLE:
      /* if the DTMF queue is empty we do nothing */
      if (!dtmfp->dtmfq_cnt)
    break;

      dtmfDigit =  dtmfp->dtmfq_buf[dtmfp->dtmfq_rd++];

      /* start generating the requested tone */
      if (dtmfDigit & (PH_DTMF_MODE_INBAND << 8))
    tg_dtmf_init(&dtmfp->dtmfg_ctx, dtmfDigit & 0x0ff, rate, 0);

      if (dtmfDigit & (PH_DTMF_MODE_RTPPAYLOAD << 8))
    rtp_session_send_dtmf2(stream->ms.rtp_session,  dtmfDigit & 0x0ff, timestamp, samples*DTMF_MSECS);

      /* update queue pointers and state */
      DTMF_LOCK(stream);
      if (dtmfp->dtmfq_rd >= DTMFQ_MAX)
    dtmfp->dtmfq_rd = 0;
      dtmfp->dtmfq_cnt--;
      if (dtmfDigit & (PH_DTMF_MODE_INBAND << 8))
    dtmfp->dtmfg_phase = DTMF_GEN;
      DTMF_UNLOCK(stream);

      /* we're going to generate DMTMF_MSEC of DTMF tone */
      dtmfp->dtmfg_len = samples*DTMF_MSECS;

      /* if we're not using INBAND mode we're done */
      if (!(dtmfDigit & (PH_DTMF_MODE_INBAND << 8)))
    return;

      // fall down

    case DTMF_GEN:

      /* how much data we can stuff in the current signal buffer */
      mixlen = (siglen > dtmfp->dtmfg_len) ? dtmfp->dtmfg_len : siglen;

      /* mix in the generated tone */
      for( mixn = 0;  mixn < mixlen; mixn++)
    signal[mixn] += tg_dtmf_next_sample(&dtmfp->dtmfg_ctx);

      /* keep track of the amount of the still ungenerated samples */
      dtmfp->dtmfg_len -= mixlen;

      /* if we didn't finish with the current dtmf digit yet,
     we'll stay in the GEN state 
      */
      if (dtmfp->dtmfg_len)
    break;

      /* we've done with the current digit, ensure we have 50msec before the next
     DTMF digit 
      */
      dtmfp->dtmfg_phase = DTMF_SILENT;
      dtmfp->dtmfg_len =  50*samples; 


      /* skip past processed part of signal */
      siglen -= mixlen;
      signal += mixlen;

      /* fall down */
    case DTMF_SILENT:

      mixlen = (siglen > dtmfp->dtmfg_len) ? dtmfp->dtmfg_len : siglen;
      dtmfp->dtmfg_len -= mixlen;
      
      /* if we have more silence to generate, keep the SILENT state */
      if (dtmfp->dtmfg_len)
    break;


      /* we can handle a next DTMF digit now */
      dtmfp->dtmfg_phase = DTMF_IDLE;
      if (dtmfp->dtmfq_cnt)
    {
      signal += mixlen;
      siglen -= mixlen;
      goto dtmf_again;
    }
      break;

    }
}
  


#if 0
int ph_media_set_recvol(phcall_t *ca, int level)
{
#if 0
  int mixer= open(MIXER_DEVICE, O_RDWR|O_NONBLOCK);
  if(mixer<0)
    return -1;

  ioctl(mixer, SOUND_MIXER_RECLEV, &level);
  close(mixer);
#endif
  return 0;
}

int ph_media_set_spkvol(phcall_t *ca, int level)
{
#if 0
  int mixer= open(MIXER_DEVICE, O_RDWR|O_NONBLOCK);
  if(mixer<0)
    return -1;

  ioctl(mixer, SOUND_MIXER_WRITE_VOLUME, &level);
  close(mixer);
#endif
  return 0;
}
#endif


void ph_audio_init_vad(phastream_t *stream)
{
  int samples = (stream->clock_rate)/1000;
  struct vadcng_info *cngp = &stream->cngi;

  
  cngp->pwr_size = PWR_WINDOW * samples;
  cngp->pwr_shift = calc_shift(cngp->pwr_size);
  cngp->pwr = osip_malloc(cngp->pwr_size * sizeof(int));
  if(cngp->pwr)
    {
      memset(cngp->pwr, 0, cngp->pwr_size * sizeof(int));
      DBG_SILOK(" DTX/VAD PWR table of %d ints allocated \n", cngp->pwr_size);
    }else{
      ph_printf("No memory for DTX/VAD !: %d \n", cngp->pwr_size*2);
      cngp->vad = cngp->cng = 0;
    }
  
  cngp->pwr_pos = 0;
  cngp->sil_max = SIL_SETUP * samples;
  cngp->long_pwr_shift = calc_shift(LONG_PWR_WINDOW);
  cngp->long_pwr_pos = 0;
#ifdef TRACE_POWER
  min_pwr = 0x80000001;
  max_pwr = 0;
  max_sil = 0;
#endif      

}


void ph_audio_init_cng(phastream_t *stream)
{
  struct vadcng_info *cngp = &stream->cngi;

  cngp->noise = osip_malloc(NOISE_B_LEN);
  if(!cngp->noise)
    {
      cngp->cng = 0;
      ph_printf("No memory for NOISE ! \n");
    }
  else
    {
#if 0
      cngp->cng_lock = g_mutex_new();
#endif
      cngp->nidx = 0;
    }

}


#ifdef PH_USE_RESAMPLE

void ph_resample_init(phastream_t *stream)
{
  SRC_DATA *resCtx = calloc(sizeof(SRC_DATA), 1);
  double playRatio, recRatio;

  playRatio = (1.0 * stream->actual_rate) / stream->clock_rate;  
  recRatio = (1.0 * stream->clock_rate) / stream->actual_rate;
  ph_printf("ph_resample_init: wanted = %d actual = %d play=%f rec=%f\n",
  	stream->clock_rate, stream->actual_rate, playRatio, recRatio);
	

  resCtx->src_ratio = playRatio;
  
  stream->play_resample_ctx = resCtx;

  resCtx = calloc(sizeof(SRC_DATA), 1);
  resCtx->src_ratio = recRatio;
  

  stream->rec_resample_ctx = resCtx;
}


void ph_resample_clean(phastream_t *stream)
{
  if (stream->rec_resample_ctx)
    free(stream->rec_resample_ctx);
  if (stream->play_resample_ctx)
    free(stream->play_resample_ctx);

}

#endif


void *ph_ec_init(int framesize, int clock_rate)
{
  int frame_samples = framesize/2;
  int frame_msecs =  frame_samples / (clock_rate/1000);
  void *ctx;
  const char *len = getenv("PH_ECHO_LENGTH");

  if (!len)
    len = "120";


#ifdef USE_SPXEC
  ctx = spxec_echo_state_init(frame_samples, (atoi(len)/frame_msecs) * frame_samples);
#else
  ctx = create_AEC(0, clock_rate);
#endif

  return ctx;
}
 

void ph_ec_cleanup(void *ctx)
{
#ifdef USE_SPXEC
  spxec_echo_state_destroy(ctx);
#else
  kill_AEC(ctx);
#endif

}



int ph_msession_audio_start(struct ph_msession_s *s, const char* deviceId)
{
  int fd;
  RtpSession *session;
  phastream_t *stream;
  phcodec_t *codec;
  char *forcedDeviceId;
  int codecpt;
  RtpProfile *rprofile = &av_profile;
  RtpProfile *sprofile = &av_profile;
  ph_mstream_params_t *sp = &s->streams[PH_MSTREAM_AUDIO1];


  ph_printf("ph_msession_audio_start: deviceId:%s\n", deviceId);

  if (!(s->newstreams & (1 << PH_MSTREAM_AUDIO1)))
    return 0;

  if (!sp->localport || !sp->remoteport)
    return 0;


  /* 
	Audio device selection:
	if we have 	PH_FORCE_AUDIO_DEVICE env var it overrides everything else
	otherwise we try to use the device specified by the UI....
	if UI didn't specify anything we try to use content of PH_AUDIO_DEVICE env var (if it is nonempty)
	and in the last resort we use PoartAudio default device
  */
  forcedDeviceId = getenv("PH_FORCE_AUDIO_DEVICE");

  if (forcedDeviceId) 
    deviceId = forcedDeviceId;
 
  if (!deviceId || !deviceId[0])
    deviceId = getenv("PH_AUDIO_DEVICE");

  if (!deviceId)
    deviceId = "pa:";

  if (ph_activate_audio_driver(deviceId))
    {
      return -PH_NORESOURCES;
    }



  codecpt = sp->ipayloads[0].number;

  /* check if this payload should by replaced by SPEEX WB */
  if (codecpt == ph_speex_hook_pt)
    {
      codecpt = PH_MEDIA_SPEEXWB_PAYLOAD;
      ph_printf("ph_msession_audio_start: replacing payload %d by %d\n", ph_speex_hook_pt, codecpt);
    }

  if (!sp->jitter)
    sp->jitter = 60;



  if (sp->flags & PH_MSTREAM_FLAG_RUNNING)
    {
      // begin branch 1 : an audio stream is already running.
      // we should process a media change if it is necessary on the (ip, port, payload)
      
      stream = (phastream_t*) sp->streamerData;
      assert(stream);
 
      ph_printf("ph_msession_audio_start: current=%08x(rip=<%s:%u> pt=%d)=>(rip=<%s:%u> pt=%d)\n", 
		stream, stream->ms.remote_ip, stream->ms.remote_port, stream->ms.payload,
		sp->remoteaddr, sp->remoteport, sp->ipayloads[0].number);
      
      if (stream->ms.remote_port == sp->remoteport)
	{
	  if ((stream->ms.payload ==  sp->ipayloads[0].number) &&  !strcmp(stream->ms.remote_ip, sp->remoteaddr))
	    {
	      ph_printf("ph_msession_audio_start: reusing current stream\n");
	      return 0;
	    }
	}
      
      /* either IP's or payloads are different */ 
      if (stream->ms.payload ==  sp->ipayloads[0].number)
	{
	  strcpy(stream->ms.remote_ip, sp->remoteaddr);
	  stream->ms.remote_port = sp->remoteport;
	  
	  rtp_session_reset(stream->ms.rtp_session);
	  
#if USE_HTTP_TUNNEL
	  if (stream->ms.tunRtp)
	    {
	      RtpTunnel *newTun, *old;
	      RtpTunnel *newTun2, *old2;
	      
	      ph_printf("ph_msessio_audio_start: Replacing audio tunnel\n");
	      newTun = rtptun_connect(stream->ms.remote_ip, stream->ms.remote_port);
	      
	      if (!newTun)
		{
		  ph_printf("ph_msession_audio_start: Audio tunnel replacement failed\n");
		  sp->flags |= ~PH_MSTREAM_FLAG_RUNNING;
		  return -PH_NORESOURCES;
		}
	      
	      newTun2 = rtptun_connect(stream->ms.remote_ip, stream->ms.remote_port+1);
	      rtp_session_set_tunnels(stream->ms.rtp_session, newTun, newTun2);
	      
	      
	      old = stream->ms.tunRtp;
	      stream->ms.tunRtp = newTun;
	      old2 = stream->ms.tunRtcp;
	      stream->ms.tunRtcp = newTun2;
	      
	      TUNNEL_CLOSE(old);
	      rtptun_free(old);
	      TUNNEL_CLOSE(old2);
	      rtptun_free(old2);
	      
	      
	    }
	  else
#endif
	    rtp_session_set_remote_addr(stream->ms.rtp_session,
					stream->ms.remote_ip,
					stream->ms.remote_port);
	  
	  DBG("audio stream reset done\n");
	  return 0;
	}
      
      /* new payload is differrent from the old one */
      ph_printf("ph_mession_audio_start: Replacing audio session\n");
      ph_msession_audio_stop(s);
	// end branch 1
      
    }

  codec = ph_media_lookup_codec(codecpt);
  if (!codec)
    {
      ph_printf("ph_media_audio_start: found NO codec\n");
      return -1;
    }

  stream = (phastream_t *)osip_malloc(sizeof(phastream_t));
  memset(stream, 0, sizeof(*stream));

  ph_printf("new audiostream = %08x\n", stream);

	// recording activity
  stream->activate_recorder = 0;
  if (getenv("PH_USE_RECORDER"))
    stream->activate_recorder = atoi(getenv("PH_USE_RECORDER"));
  
  if (stream->activate_recorder)
    {
      char *rname = getenv("PH_RECORD_FILE");
      char fname[128];
      static int fnindex = 1;
      if (!rname)
	rname = "recording%d.sw";
		
      snprintf(fname, 128, rname, fnindex++);
      ph_media_audio_recording_init(&stream->recorder, fname, 3, 4000);
    }
  
  gettimeofday(&stream->last_rtp_sent_time, 0);
  stream->cngi.last_noise_sent_time = stream->last_rtp_recv_time = stream->cngi.last_dtx_time = stream->last_rtp_sent_time;

    

  if ((sp->flags & PH_MSTREAM_FLAG_HDX) || getenv("PH_FORCE_HDX"))
    {
      char*  fhdx =  getenv("PH_FORCE_HDX");
      
      stream->hdxmode = 1;
      stream->hdxsilence = 1;
      if (fhdx)
	sp->vadthreshold = atoi(fhdx);

      ph_printf("ph_mession_audio_start: HDX mode level=%d\n",  sp->vadthreshold);

    }

  stream->ms.mses = s;
  stream->clock_rate = stream->actual_rate = ph_media_get_clock_rate(sp->ipayloads[0].number);
  stream->cngi.vad = (sp->flags & PH_MSTREAM_FLAG_VAD)?1:0;
  stream->cngi.pwr_threshold = sp->vadthreshold;
  stream->cngi.cng = (sp->flags & PH_MSTREAM_FLAG_CNG)?1:0;
  stream->ms.codec = codec;
  stream->ms.payload =  sp->ipayloads[0].number;
  stream->cngi.cng_pt = (stream->clock_rate > 8000) ? PH_MEDIA_CN_16000_PAYLOAD : PH_MEDIA_CN_PAYLOAD; /* FIXME: we need to handle separate directions too */

  ph_printf("ph_mession_audio_start: DTX/VAD %x\n", stream->cngi.pwr_threshold);
  ph_printf("ph_msession_audio_start: clock rate %d\n", stream->clock_rate);
  ph_printf("ph_msession_audio_start: CNG %s\n", stream->cngi.cng ? "activating" : "desactivating");

  ph_printf("ph_msession_audio_start: opening AUDIO device %s\n", deviceId);

  fd = audio_stream_open(stream, deviceId, stream->clock_rate, codec->decoded_framesize, ph_audio_callback); 
  if (fd < 0)
    {
//	  phcb->errorNotify(PH_NOMEDIA);
      ph_printf("ph_msession_audio_start: can't open  AUDIO device\n");
      free(stream);
      return -PH_NORESOURCES;
    }

#ifdef PH_USE_RESAMPLE
  if (stream->clock_rate != stream->actual_rate)
    ph_resample_init(stream);
#endif

  if (codec->encoder_init)
    stream->ms.encoder_ctx = codec->encoder_init(0);

  if (codec->decoder_init)
    stream->ms.decoder_ctx = codec->decoder_init(0);


  strcpy(stream->ms.remote_ip, sp->remoteaddr);
  stream->ms.remote_port = sp->remoteport;
  
  
  session = rtp_session_new(RTP_SESSION_SENDRECV);
#ifdef USE_HTTP_TUNNEL
  if (sp->flags & PH_MSTREAM_FLAG_TUNNEL)
{
	RtpTunnel *tun, *tun2;

	ph_printf("ph_mession_audio_start: Creating audio tunnel\n");
		
		
	tun = rtptun_connect(sp->remoteaddr, sp->remoteport);

	if (!tun)
	{
		return -PH_NORESOURCES;
	}

	tun2 = rtptun_connect(sp->remoteaddr, sp->remoteport+1);
	 
	 
	rtp_session_set_tunnels(session, tun, tun2);
	stream->ms.tunRtp = tun;
	stream->ms.tunRtcp = tun2;

	}
#endif  


  
  rtp_session_set_scheduling_mode(session, SCHEDULING_MODE); /* yes */
  rtp_session_set_blocking_mode(session, BLOCKING_MODE);

  if (codecpt != stream->ms.payload)
    {
      rprofile = sprofile = rtp_profile_clone_full(sprofile);
      rtp_profile_move_payload(sprofile, codecpt, ph_speex_hook_pt);
    }
  else if (sp->ipayloads[0].number != sp->opayloads[0].number)
    {
      sprofile = rtp_profile_clone_full(sprofile);
      rtp_profile_move_payload(sprofile, sp->ipayloads[0].number,  sp->opayloads[0].number);
    }

  rtp_session_set_profiles(session, sprofile, rprofile);
  rtp_session_set_jitter_compensation(session, sp->jitter);
  rtp_session_set_local_addr(session, "0.0.0.0", sp->localport);

  
  rtp_session_set_remote_addr(session, sp->remoteaddr,sp->remoteport);

  rtp_session_set_payload_types(session, sp->opayloads[0].number, sp->ipayloads[0].number);
  rtp_session_set_cng_pt(session, stream->cngi.cng_pt);
  rtp_session_signal_connect(session, "telephone-event",
                 (RtpCallback)ph_telephone_event, s);
  rtp_session_signal_connect(session, "cng_packet",
                 (RtpCallback)ph_on_cng_packet, s);

  sp->flags |= PH_MSTREAM_FLAG_RUNNING;
  sp->streamerData = stream;
  s->activestreams |= (1 << PH_MSTREAM_AUDIO1);

  ph_printf("ph_mession_audio_start: s=%08x.stream=%08x\n", s, stream);

  stream->ms.running = 1;
  stream->ms.rtp_session = session;
  stream->dtmfCallback = s->dtmfCallback;

  if (stream->cngi.vad || stream->hdxmode)
    {
      ph_audio_init_vad(stream);
    }

  if (stream->cngi.cng)
    {
      ph_audio_init_cng(stream);
    }
  

  if ( !(sp->flags & PH_MSTREAM_FLAG_AEC) )
    {
#ifdef DO_ECHO_CAN  
      ph_printf("ph_media_audio_start: Echo CAN desactivated\n");
      stream->ec = 0;
    }
  else
    {
      stream->ec = ph_ec_init(codec->decoded_framesize, stream->clock_rate);
      if (stream->ec)
	{
	  const char *lat = getenv("PH_ECHO_LATENCY");

#if 0
	  if (!lat)
	    lat = "120"; 
#endif
      
	  stream->audio_loop_latency = 0;
      
	  if (lat)
	    stream->audio_loop_latency = atoi(lat) * 2 * stream->clock_rate/1000;

        
	  /* 
	     circular buffer must be able to accomodate MAX_OUTPUT_LATENCY millisecs in output direction
	     and the same amount in input direction 
	  */
	  cb_init(&stream->pcmoutbuf, 2 * sizeof(short) * MAX_OUTPUT_LATENCY * stream->clock_rate/1000);
	  stream->sent_cnt = stream->recv_cnt = 0;
	  stream->ecmux = g_mutex_new();
          
	}
      ph_printf("ph_msession_audio_start: Echo CAN created OK\n");
#endif
    }

  stream->dtmfi.dtmfg_lock = g_mutex_new();
  stream->dtmfi.dtmfq_cnt = 0;
  stream->dtmfi.dtmfg_phase = DTMF_IDLE;


  audio_stream_start(stream);

  stream->using_out_callback = audio_driver_has_play_callback();
  
  /* 
     replay of last frame is only interesting when we're working in callback mode 
     if activated in non callback mode it feeds the audio playback queue with one audio frame 
     each 10 msecs, which is  
  */

  if (0 && stream->using_out_callback)
    stream->lastframe = calloc(codec->decoded_framesize, 1);

	
  if (!audio_driver_has_rec_callback() || !audio_driver_has_play_callback())      
    stream->ms.media_io_thread = osip_thread_create(20000,
                        ph_audio_io_thread, stream);



  ph_printf("ph_msession_audio_start: audio stream init OK\n");
  return 0;
  // end branch 2




}

#if 0
int ph_media_audio_start(phcall_t *ca, int port,
                   void (*dtmfCallback)(phcall_t *ca, int event), 
                   void (*endCallback)(phcall_t *ca, int event),
           const char * deviceId, unsigned vad, int cng, int jitter, int noaec)

{
  int fd;
  RtpSession *session;
  phastream_t *stream;
  phcodec_t *codec;
  char *forcedDeviceId;
  int codecpt;
  RtpProfile *profile = &av_profile;

#ifdef WORDS_BIGENDIAN
  ph_printf("ph_media_audio_start: BIGENDIAN\n");
#endif
  ph_printf("ph_media_audio_start: deviceId:%s\n", deviceId);

  ph_printf("Starting audio stream from port: %d to %s:%d payload=%d ca=%08x.stream=%08x\n", 
        port, ca->remote_sdp_audio_ip, ca->remote_sdp_audio_port, ca->audio_payload, ca, ca->ph_audio_stream);

  
  /* 
	Audio device selection:
	if we have 	PH_FORCE_AUDIO_DEVICE env var it overrides everything else
	otherwise we try to use the device specified by the UI....
	if UI didn't specify anything we try to use content of PH_AUDIO_DEVICE env var (if it is nonempty)
	and in the last resort we use PoartAudio default device
  */
  forcedDeviceId = getenv("PH_FORCE_AUDIO_DEVICE");

  if (forcedDeviceId) 
    deviceId = forcedDeviceId;
 
  if (!deviceId || !deviceId[0])
    deviceId = getenv("PH_AUDIO_DEVICE");

  if (!deviceId)
    deviceId = "pa:";

  if (ph_activate_audio_driver(deviceId))
    {
      return -PH_NORESOURCES;
    }



  if (!jitter)
    jitter = 60;

  if (port == 0)
    return -1;

  codecpt = ca->audio_payload;
#ifdef SPEEX_OVER_G729_HACK
  if (codecpt == PH_MEDIA_G729_PAYLOAD)
	codecpt = PH_MEDIA_SPEEXWB_PAYLOAD;
#endif


  if (ca->hasaudio)
    {
      // begin branch 1 : an audio stream is already running.
      // we should process a media change if it is necessary on the (ip, port, payload)
      
     stream = (phastream_t*) ca->ph_audio_stream;
     assert(stream);
 
      ph_printf("ph_media_audio_start: current=%08x(rip=<%s:%u> pt=%d)=>(rip=<%s:%u> pt=%d)\n", 
		stream, stream->ms.remote_ip, stream->ms.remote_port, stream->ms.payload,
		ca->remote_sdp_audio_ip, ca->remote_sdp_audio_port, ca->audio_payload);
      
      if (stream->ms.remote_port == ca->remote_sdp_audio_port)
	{
	  if ((stream->ms.payload ==  ca->audio_payload) &&  !strcmp(stream->ms.remote_ip, ca->remote_sdp_audio_ip))
	    {
	      ph_printf("ph_media_audio_start: reusing current stream\n");
	      return 0;
	    }
	}
      
      /* either IP's or payloads are different */ 
      if ((stream->ms.payload ==  ca->audio_payload))
	{
	  strcpy(stream->ms.remote_ip, ca->remote_sdp_audio_ip);
	  stream->ms.remote_port = ca->remote_sdp_audio_port;
	  
	  rtp_session_reset(stream->ms.rtp_session);
	  
#if USE_HTTP_TUNNEL
	  if (stream->ms.tunRtp)
	    {
	      RtpTunnel *newTun, *old;
	      RtpTunnel *newTun2, *old2;
	      
	      ph_printf("ph_media_audio_start: Replacing audio tunnel\n");
	      newTun = rtptun_connect(stream->ms.remote_ip, stream->ms.remote_port);
	      
	      if (!newTun)
		{
		  ph_printf("ph_media_audio_start: Audio tunnel replacement failed\n");
		  return -PH_NORESOURCES;
		}
	      
	      newTun2 = rtptun_connect(stream->ms.remote_ip, stream->ms.remote_port+1);
	      rtp_session_set_tunnels(stream->ms.rtp_session, newTun, newTun2);
	      
	      
	      old = stream->ms.tunRtp;
	      stream->ms.tunRtp = newTun;
	      old2 = stream->ms.tunRtcp;
	      stream->ms.tunRtcp = newTun2;
	      
	      TUNNEL_CLOSE(old);
	      rtptun_free(old);
	      TUNNEL_CLOSE(old2);
	      rtptun_free(old2);
	      
	      
	    }
	  else
#endif
	    rtp_session_set_remote_addr(stream->ms.rtp_session,
					stream->ms.remote_ip,
					stream->ms.remote_port);
	  
	  DBG("audio stream reset done\n");
	  return 0;
	}
      
      /* new payload is differrent from the old one */
      ph_printf("ph_media_audio_start: Replacing audio session\n");
      ph_media_audio_stop(ca);
      
	// end branch 1
    }

	// begin branch 2: a new stream must be created from scratch

  codec = ph_media_lookup_codec(codecpt);
  if (!codec)
    {
      ph_printf("ph_media_audio_start: found NO codec\n");
      return -1;
    }

  stream = (phastream_t *)osip_malloc(sizeof(phastream_t));
  memset(stream, 0, sizeof(*stream));

  ph_printf("new audiostream = %08x\n", stream);

	// recording activity
  stream->activate_recorder = 0;
  if (getenv("PH_USE_RECORDER"))
    stream->activate_recorder = atoi(getenv("PH_USE_RECORDER"));
  
  if (stream->activate_recorder)
    {
      char *rname = getenv("PH_RECORD_FILE");
      char fname[128];
      static int fnindex = 1;
      if (!rname)
	rname = "recording%d.sw";
		
      snprintf(fname, 128, rname, fnindex++);
      ph_media_audio_recording_init(&stream->recorder, fname, 3, 4000);
    }
  
  gettimeofday(&stream->last_rtp_sent_time, 0);
  stream->cngi.last_noise_sent_time = stream->last_rtp_recv_time = stream->cngi.last_dtx_time = stream->last_rtp_sent_time;

    
    
  stream->ms.ca = ca;
  stream->clock_rate = stream->actual_rate = ph_media_get_clock_rate(ca->audio_payload);
  stream->cngi.vad = (vad & VAD_VALID_MASK)?1:0;
  stream->cngi.pwr_threshold = vad & VAD_THRESHOLD_MASK;
  stream->cngi.cng = cng;
  stream->ms.codec = codec;
  stream->ms.payload =  ca->audio_payload;
  stream->cngi.cng_pt = (stream->clock_rate > 8000) ? PH_MEDIA_CN_16000_PAYLOAD : PH_MEDIA_CN_PAYLOAD;

  ph_printf("ph_media_audio_start: DTX/VAD %x\n", vad);
  ph_printf("ph_media_audio_start: clock rate %d\n", stream->clock_rate);
  ph_printf(cng ? "ph_media_audio_start: CNG activating \n" : "ph_media_audio_start: CNG desactivated \n");

  ph_printf("ph_media_audio_start: opening AUDIO device %s\n", deviceId);

  fd = audio_stream_open(stream, deviceId, stream->clock_rate, codec->decoded_framesize, ph_audio_callback); 
  if (fd < 0)
    {
//	  phcb->errorNotify(PH_NOMEDIA);
	  ph_printf("ph_media_audio_start: can't open  AUDIO device\n");
      free(stream);
      return -PH_NORESOURCES;
    }

#ifdef PH_USE_RESAMPLE
  if (stream->clock_rate != stream->actual_rate)
    ph_resample_init(stream);
#endif

  if (codec->encoder_init)
    stream->ms.encoder_ctx = codec->encoder_init(0);

  if (codec->decoder_init)
    stream->ms.decoder_ctx = codec->decoder_init(0);


  strcpy(stream->ms.remote_ip, ca->remote_sdp_audio_ip);
  stream->ms.remote_port = ca->remote_sdp_audio_port;
  
  
  session = rtp_session_new(RTP_SESSION_SENDRECV);
#ifdef USE_HTTP_TUNNEL
  if (ph_media_use_tunnel)
	{
	RtpTunnel *tun, *tun2;

	ph_printf("ph_media_audio_start: Creating audio tunnel\n");
		
		
	tun = rtptun_connect(ca->remote_sdp_audio_ip, ca->remote_sdp_audio_port);

	if (!tun)
	{
		return -PH_NORESOURCES;
	}

	tun2 = rtptun_connect(ca->remote_sdp_audio_ip, ca->remote_sdp_audio_port+1);
	 
	 
	rtp_session_set_tunnels(session, tun, tun2);
	stream->ms.tunRtp = tun;
	stream->ms.tunRtcp = tun2;

	}
#endif  


  
  rtp_session_set_scheduling_mode(session, SCHEDULING_MODE); /* yes */
  rtp_session_set_blocking_mode(session, BLOCKING_MODE);


#ifdef SPEEX_OVER_G729_HACK
  
  profile = rtp_profile_clone_full(&av_profile);

  if (codecpt == PH_MEDIA_SPEEXWB_PAYLOAD && ca->payload == PH_MEDIA_G729_PAYLOAD)
	  rtp_profile_move_payload(profile,PH_MEDIA_SPEEXWB_PAYLOAD, PH_MEDIA_G729_PAYLOAD);

#endif

  rtp_session_set_profile(session, profile);
  rtp_session_set_jitter_compensation(session, jitter);
  rtp_session_set_local_addr(session, "0.0.0.0", port);

  
  rtp_session_set_remote_addr(session,
                  ca->remote_sdp_audio_ip,
                  ca->remote_sdp_audio_port);

  rtp_session_set_payload_type(session, ca->audio_payload);
  rtp_session_set_cng_pt(session, stream->cngi.cng_pt);
  rtp_session_signal_connect(session, "telephone-event",
                 (RtpCallback)ph_telephone_event, ca);
  rtp_session_signal_connect(session, "cng_packet",
                 (RtpCallback)ph_on_cng_packet, ca);

  ca->hasaudio = 1;
  ca->ph_audio_stream = stream;

  ph_printf("ph_media_audio_start: ca=%08x.stream=%08x\n", ca, ca->ph_audio_stream);

  stream->ms.running = 1;
  stream->ms.rtp_session = session;
  stream->dtmfCallback = dtmfCallback;

  if (stream->cngi.vad)
    {
      ph_audio_init_vad(stream);
    }

  if (stream->cngi.cng)
    {
      ph_audio_init_cng(stream);
    }
  

  if ( noaec )
    {
#ifdef DO_ECHO_CAN  
      ph_printf("ph_media_audio_start: Echo CAN desactivated\n");
      stream->ec = 0;
    }
  else
    {
      stream->ec = ph_ec_init(codec->decoded_framesize, stream->clock_rate);
	
	if (stream->ec) {
		const char *lat = getenv("PH_ECHO_LATENCY");         
		cb_init(&stream->pcmoutbuf, PCM_TRACE_LEN*(stream->clock_rate/8000)); 
		stream->sent_cnt = stream->recv_cnt = 0;
		stream->ecmux = g_mutex_new();
		stream->audio_loop_latency = 0;
		if (lat) {
			stream->audio_loop_latency = atoi(lat) * 2 * stream->clock_rate/1000;
		}
	}
	ph_printf("ph_media_audio_start: Echo CAN created OK\n");
#endif //DO_ECHO_CAN
    }

  stream->dtmfi.dtmfg_lock = g_mutex_new();
  stream->dtmfi.dtmfq_cnt = 0;
  stream->dtmfi.dtmfg_phase = DTMF_IDLE;


  audio_stream_start(stream);

  stream->using_out_callback = audio_driver_has_play_callback();
  
  /* 
     replay of last frame is only interesting when we're working in callback mode 
     if activated in non callback mode it feeds the audio playback queue with one audio frame 
     each 10 msecs, which is  
  */

  if (0 && stream->using_out_callback)
    stream->lastframe = calloc(codec->decoded_framesize, 1);

	
  if (!audio_driver_has_rec_callback() || !audio_driver_has_play_callback())      
    stream->ms.media_io_thread = osip_thread_create(20000,
                        ph_audio_io_thread, stream);



  ph_printf("ph_media_audio_start: audio stream init OK\n");
  return 0;
  // end branch 2
}
#endif


void ph_audio_vad_cleanup(phastream_t *stream)
{
  if ( stream->cngi.pwr )
    osip_free(stream->cngi.pwr);

  stream->cngi.pwr = 0;

  if ( stream->cngi.cng )
    {
      CNG_LOCK(stream);
      
      if(stream->cngi.noise)
	osip_free(stream->cngi.noise);

      stream->cngi.noise = 0;

      CNG_UNLOCK(stream);
#if 0
      if ( stream->cngi.cng_lock )
    g_mutex_free(stream->cng_lock);
#endif
    }
}


void ph_msession_audio_stop(struct ph_msession_s *s)
{ 
  struct ph_mstream_params_s *msp = &s->streams[PH_MSTREAM_AUDIO1];
  phastream_t *stream = (phastream_t *) msp->streamerData;


  s->activestreams &= ~(1 << PH_MSTREAM_AUDIO1);
 
  stream->ms.running = 0;

  if (stream->ms.media_io_thread)
    {
      osip_thread_join(stream->ms.media_io_thread);
      osip_free(stream->ms.media_io_thread);
    }

#ifdef PH_USE_RESAMPLE
  if (stream->actual_rate != stream->clock_rate)
    ph_resample_clean(stream);
#endif

  msp->streamerData = 0;
  msp->flags &= ~PH_MSTREAM_FLAG_RUNNING;

  audio_stream_close(stream); /* close the sound card */



  if (stream->mixbuf)
    ph_mediabuf_free(stream->mixbuf);

#ifdef TRACE_POWER
  print_pwrstats(stream);
#endif


  //  RTP_SESSION_LOCK(stream->rtp_session);

  rtp_session_signal_disconnect_by_callback(stream->ms.rtp_session, "telephone-event",
                        (RtpCallback)ph_telephone_event);
  rtp_session_signal_disconnect_by_callback(stream->ms.rtp_session, "cng_packet",
                        (RtpCallback)ph_on_cng_packet);

  ortp_set_debug_file("oRTP", stdout); 
  //  ortp_global_stats_display();
  ortp_session_stats_display(stream->ms.rtp_session);
  ortp_set_debug_file("oRTP", NULL); 

 {
   /* free non default profiles */
   RtpProfile *rprofile, *sprofile;
   
   sprofile = rtp_session_get_send_profile(stream->ms.rtp_session);
   if (sprofile != &av_profile)
     rtp_profile_destroy(sprofile);

   rprofile = rtp_session_get_recv_profile(stream->ms.rtp_session);
   if (rprofile != &av_profile && rprofile != sprofile)
     rtp_profile_destroy(rprofile);

 }


  rtp_session_destroy(stream->ms.rtp_session);

  if (stream->ms.codec->encoder_cleanup)
    stream->ms.codec->encoder_cleanup(stream->ms.encoder_ctx);
  if (stream->ms.codec->decoder_cleanup)
    stream->ms.codec->decoder_cleanup(stream->ms.decoder_ctx);

  ph_audio_vad_cleanup(stream);

#ifdef DO_ECHO_CAN
  if(stream->ec)
    {
      //   DBG("\nbytes echoed %d %d\n", stream->sent_cnt, stream->recv_cnt);
      cb_clean(&stream->pcmoutbuf);
      if(stream->ec)
	ph_ec_cleanup(stream->ec);

    g_mutex_free(stream->ecmux);
    }
#endif

  g_mutex_free(stream->dtmfi.dtmfg_lock);

  if (stream->activate_recorder)
    {
      ph_media_audio_recording_close(&stream->recorder);
    }
  
  DBG("\naudio stream closed\n");

  if (stream->lastframe)
    free(stream->lastframe);
  
  osip_free(stream);
}


#if 0
void ph_media_audio_stop(phcall_t *ca)
{
  phastream_t *stream = (phastream_t *) ca->ph_audio_stream;


  ca->hasaudio = 0;
  ca->ph_audio_stream = 0;
  stream->ms.running = 0;

  if (stream->ms.media_io_thread)
    {
      osip_thread_join(stream->ms.media_io_thread);
      osip_free(stream->ms.media_io_thread);
    }

#ifdef PH_USE_RESAMPLE
  if (stream->actual_rate != stream->clock_rate)
    ph_resample_clean(stream);
#endif

  audio_stream_close(stream); /* close the sound card */



  if (stream->mixbuf)
    ph_mediabuf_free(stream->mixbuf);

#ifdef TRACE_POWER
  print_pwrstats(stream);
#endif


  //  RTP_SESSION_LOCK(stream->rtp_session);

  rtp_session_signal_disconnect_by_callback(stream->ms.rtp_session, "telephone-event",
                        (RtpCallback)ph_telephone_event);
  rtp_session_signal_disconnect_by_callback(stream->ms.rtp_session, "cng_packet",
                        (RtpCallback)ph_on_cng_packet);

  ortp_set_debug_file("oRTP", stdout); 
  //  ortp_global_stats_display();
  ortp_session_stats_display(stream->ms.rtp_session);
  ortp_set_debug_file("oRTP", NULL); 

#ifdef SPEEX_OVER_G729_HACK
 {
   RtpProfile *profile = rtp_session_get_profile(stream->ms.rtp_session);
   rtp_profile_destroy(profile);
 }
#endif

  //  RTP_SESSION_UNLOCK(stream->rtp_session);
  rtp_session_destroy(stream->ms.rtp_session);

  if (stream->ms.codec->encoder_cleanup)
    stream->ms.codec->encoder_cleanup(stream->ms.encoder_ctx);
  if (stream->ms.codec->decoder_cleanup)
    stream->ms.codec->decoder_cleanup(stream->ms.decoder_ctx);

  ph_audio_vad_cleanup(stream);

#ifdef DO_ECHO_CAN
  if(stream->ec)
    {
      //   DBG("\nbytes echoed %d %d\n", stream->sent_cnt, stream->recv_cnt);
      cb_clean(&stream->pcmoutbuf);
      if(stream->ec)
	ph_ec_cleanup(stream->ec);

    g_mutex_free(stream->ecmux);
    }
#endif

  g_mutex_free(stream->dtmfi.dtmfg_lock);

  if (stream->activate_recorder)
    {
      ph_media_audio_recording_close(&stream->recorder);
    }
  
  DBG("\naudio stream closed\n");

  if (stream->lastframe)
    free(stream->lastframe);
  
  osip_free(stream);
}
#endif




void 
ph_media_audio_init()
{
  static int first_time = 1;

#if defined(OS_LINUX)
  void ph_oss_driver_init();
  void ph_alsa_driver_init();

#ifdef ENABLE_ARTS
  void ph_audio_arts_driver_init();
#endif

#ifdef ENABLE_ESD
  void ph_audio_esd_driver_init();
#endif

#elif defined(OS_WINDOWS)
  void ph_winmm_driver_init();
#endif
  void ph_pa_driver_init();
  void ph_null_driver_init();

#ifdef ENABLE_OPENAL
  void ph_openal_driver_init();
#endif /* ENABLE_OPENAL */

#ifdef OS_MACOSX
  void ph_ca_driver_init();
#endif
  
  if (!first_time)
    return;

#ifdef OS_WINDOWS
  ph_winmm_driver_init();
#endif

#ifdef OS_MACOSX
  ph_ca_driver_init();
#endif

#ifdef OS_LINUX

#ifdef ENABLE_ALSA 
  ph_alsa_driver_init();
#endif

#ifdef ENABLE_ARTS
  ph_audio_arts_driver_init();
#endif

#ifdef ENABLE_ESD
  ph_audio_esd_driver_init();
#endif

#endif

#ifdef ENABLE_PORTAUDIO
  ph_pa_driver_init();
#endif


#ifdef ENABLE_OPENAL
  ph_openal_driver_init();
#endif 

 
  ph_null_driver_init();


  tg_init_sine_table();
  ph_gen_noise();
  ph_gen_silence();

  /* retrieve payload code to be replaced by SPEEX WB */ 
  {
    char *speexhook = getenv("PH_SPEEX_HOOK");
    char *mictrace = getenv("PH_TRACE_MIC");
      
    if (speexhook)
      ph_speex_hook_pt = atoi(speexhook);
    
    /* enbale MIC signal level tracing */
    if (mictrace)
      ph_trace_mic = atoi(mictrace);
  }


  first_time = 0;
  return;
}


#if 0
int ph_media_send_dtmf(phcall_t *ca, int dtmf, int mode)
{
  phastream_t *stream = (phastream_t *)(ca->ph_audio_stream);
  unsigned long ts;

  if (!stream)
    return -1;


  if (!mode || mode > 3)
    mode = 3;

  DTMF_LOCK(stream);
  if (stream->dtmfi.dtmfq_cnt < DTMFQ_MAX)
    {
      stream->dtmfi.dtmfq_buf[stream->dtmfi.dtmfq_wr++] = (unsigned short) (dtmf | (mode << 8));
      if (stream->dtmfi.dtmfq_wr == DTMFQ_MAX)
    stream->dtmfi.dtmfq_wr = 0;

      stream->dtmfi.dtmfq_cnt++;
      DTMF_UNLOCK(stream);
      return 0;
    }

  DTMF_UNLOCK(stream);
  return -1;
}
#endif

int ph_msession_send_dtmf(struct ph_msession_s *s, int dtmf, int mode)
{
  phastream_t *stream = (phastream_t *)(s->streams[PH_MSTREAM_AUDIO1].streamerData);
  unsigned long ts;

  if (!stream)
    return -1;


  if (!mode || mode > 3)
    mode = 3;

  DTMF_LOCK(stream);
  if (stream->dtmfi.dtmfq_cnt < DTMFQ_MAX)
    {
      stream->dtmfi.dtmfq_buf[stream->dtmfi.dtmfq_wr++] = (unsigned short) (dtmf | (mode << 8));
      if (stream->dtmfi.dtmfq_wr == DTMFQ_MAX)
    stream->dtmfi.dtmfq_wr = 0;

      stream->dtmfi.dtmfq_cnt++;
      DTMF_UNLOCK(stream);
      return 0;
    }

  DTMF_UNLOCK(stream);
  return -1;
}


#if 0
int ph_media_send_sound_file(phcall_t *ca, const char *filename)
{
  phastream_t *stream = (phastream_t *)(ca->ph_audio_stream);
  ph_mediabuf_t *mb;

  if (stream->mixbuf)  /* we're already mixing a something? */
    return -PH_NORESOURCES;

  mb = ph_mediabuf_load(filename, stream->clock_rate);
  if (!mb)
    return -PH_NORESOURCES;

  stream->mixbuf = mb;
  return 0;
}
#endif

int ph_msession_send_sound_file(struct ph_msession_s *s, const char *filename)
{
  phastream_t *stream = (phastream_t *)(s->streams[PH_MSTREAM_AUDIO1].streamerData);
  ph_mediabuf_t *mb;

  if (!stream)
    return -PH_NOMEDIA;

  if (stream->mixbuf)  /* we're already mixing a something? */
    return -PH_NORESOURCES;

  mb = ph_mediabuf_load(filename, stream->clock_rate);
  if (!mb)
    return -PH_NORESOURCES;

  stream->mixbuf = mb;
  return 0;
}


/* we're called by rtp to announce reception of DTMF event */
void 
ph_telephone_event(RtpSession *rtp_session, int event, struct ph_msession_s *s)
{
  static const char evttochar[] = "0123456789*#ABCD!";
  phastream_t *stream = (phastream_t *)s->streams[PH_MSTREAM_AUDIO1].streamerData;

  if (stream->dtmfCallback)
    stream->dtmfCallback(s->cbkInfo, evttochar[event]);

}

void
ph_media_audio_recording_init(recording_t *recording, const char *filename, int nchannels, int chunksize) {
	
  recording->samples = (short *) malloc(nchannels * chunksize * sizeof(short));
  recording->chunksize = chunksize;
  recording->nchannels = nchannels;
  recording->position = 0;
  recording->fd = fopen(filename,"wb");

}

void
ph_media_audio_recording_record_one(recording_t *recording, short c1, short c2, short c3) 
{

  short *samples = recording->samples+recording->nchannels*recording->position;
	
  *samples++ = c1;
  if (recording->nchannels > 1)
    *samples++ = c2;
  if (recording->nchannels > 2)
    *samples++ = c3;
  
  
  recording->position++;
  if (recording->position == recording->chunksize) 
    {
      ph_media_audio_recording_dump(recording);
      recording->position = 0;
    }
	
}

static void
ph_media_audio_recording_dump(recording_t *recording) 
{

  if (recording->position > 0) 
    {
      fwrite(recording->samples, recording->nchannels*sizeof(short), recording->position, recording->fd);
    }
	
}
void
ph_media_audio_recording_close(recording_t *recording) 
{
		
  ph_media_audio_recording_dump(recording);
  fclose(recording->fd);
  free(recording->samples);
	
}
