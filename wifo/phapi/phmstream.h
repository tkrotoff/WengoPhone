#ifndef __PHMSTREAM_H__
#define __PHMSTREAM_H__

#ifdef __cplusplus
extern "C"
{
#endif


#if !defined(WIN32)
#define TYPE(val) .type = (val)
#define CLOCK_RATE(val) .clock_rate = (val)
#define BYTES_PER_SAMPLE(val) .bytes_per_sample = (val)
#define ZERO_PATTERN(val)   .zero_pattern = (val)
#define PATTERN_LENGTH(val) .pattern_length = (val)
#define NORMAL_BITRATE(val) .normal_bitrate = (val)
#define MIME_TYPE(val) .mime_type = (val)
#else
#define TYPE(val) (val)
#define CLOCK_RATE(val) (val)
#define BYTES_PER_SAMPLE(val) (val)
#define ZERO_PATTERN(val)   (val)
#define PATTERN_LENGTH(val) (val)
#define NORMAL_BITRATE(val)  (val)
#define MIME_TYPE(val)  (val)
#endif

struct ph_msession_s;
struct phcall_s;
struct phmstream
{
  struct _RtpSession *rtp_session;
  struct ph_msession_s *mses;
 
  phcodec_t *codec;
  void      *encoder_ctx;
  void      *decoder_ctx;

  char  remote_ip[32];
  int	remote_port;

  int payload;
  int running;


  unsigned long rxtstamp;
  unsigned long txtstamp;
  unsigned long rxts_inc;

  struct osip_thread *media_io_thread;

  void      (*endCallback)(void *ca, int arg);


#ifdef USE_HTTP_TUNNEL
#define TUNNEL_CLOSE(t) { if(t) { (t)->tunnel_close((t)->h_tunnel); (t)->h_tunnel = 0; }}  
  struct _RtpTunnel *tunRtp;
  struct _RtpTunnel *tunRtcp;
	  
#endif

};
typedef struct phmstream phmstream_t;

extern int ph_media_use_tunnel;
phcodec_t *ph_media_lookup_codec(int payload);
int ph_media_get_clock_rate(int payload);


void ph_tvsub(register struct timeval *out, register struct timeval *in);
void ph_tvdiff(struct timeval *diff, struct timeval *out, struct timeval *in);


#ifdef USE_HTTP_TUNNEL
struct _RtpTunnel *rtptun_new(); 
void rtptun_free(struct _RtpTunnel *);
struct _RtpTunnel *rtptun_connect(const char *addr, int port); 
#endif




#ifdef WIN32
struct timespec {
	long tv_sec;
	long tv_nsec;
};
#endif

#ifndef TIMEVAL_TO_TIMESPEC
#define TIMEVAL_TO_TIMESPEC(tv, ts) {                                   \
        (ts)->tv_sec = (tv)->tv_sec;                                    \
	        (ts)->tv_nsec = (tv)->tv_usec * 1000;                           \
}
#endif /*TIMEVAL_TO_TIMESPEC*/

#ifndef TIMESPEC_TO_TIMEVAL
#define TIMESPEC_TO_TIMEVAL(tv, ts) {                                   \
        (tv)->tv_sec = (ts)->tv_sec;                                    \
	        (tv)->tv_usec = (ts)->tv_nsec / 1000;                           \
}
#endif /*TIMESPEC_TO_TIMEVAL*/

#ifdef __cplusplus
}
#endif


void ph_msession_video_stop(struct ph_msession_s *s);
int  ph_msession_video_start(struct ph_msession_s *s, const char *device);
void ph_msession_audio_stop(struct ph_msession_s *s);
int  ph_msession_audio_start(struct ph_msession_s *s, const char *device);

#if 0
int ph_media_audio_start(phcall_t *ca, int port,
				   void (*dtmfCallback)(phcall_t *ca, int event), 
				   void (*endCallback)(phcall_t *ca, int event),
			 const char * deviceId, unsigned vad, int cng, int jitter, int noaec);



void ph_media_audio_stop(phcall_t *ca);

#endif
#endif

