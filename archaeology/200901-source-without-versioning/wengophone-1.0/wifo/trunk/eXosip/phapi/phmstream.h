#ifndef __PHMSTREAM_H__
#define __PHMSTREAM_H__

#ifdef __cplusplus
extern "C"
{
#endif

#if 0
#define PH_MEDIA_DTMF_PAYLOAD 101
#define PH_MEDIA_ILBC_PAYLOAD 97
#define PH_MEDIA_AMR_PAYLOAD 123
#define PH_MEDIA_AMR_WB_PAYLOAD 124
#define PH_MEDIA_G72632_PAYLOAD 2
#define PH_MEDIA_G729_PAYLOAD 18
#define PH_MEDIA_H263_PAYLOAD 34
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


struct phmstream
{
  struct _RtpSession *rtp_session;
  phcall_t  *ca;
  phcodec_t *codec;
  void      *encoder_ctx;
  void      *decoder_ctx;

  char  remote_ip[32];
  int	remote_port;

  int payload;
  int running;
  int is_terminated;

  unsigned long rxtstamp;
  unsigned long txtstamp;
  unsigned long rxts_inc;

  struct osip_thread *media_io_thread;

  void      (*endCallback)(phcall_t *ca, int arg);


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
void
ph_tvdiff(struct timeval *diff, struct timeval *out, struct timeval *in);


#ifdef USE_HTTP_TUNNEL
struct _RtpTunnel *rtptun_new(); 
void rtptun_free(struct _RtpTunnel *);
struct _RtpTunnel *rtptun_connect(const char *addr, int port); 
#endif

void
ph_tvdiff(struct timeval *diff, struct timeval *out, struct timeval *in);



#ifdef WIN32
struct timespec {
	long tv_sec;
	long tv_nsec;
};
#endif

# define TIMEVAL_TO_TIMESPEC(tv, ts) {                                   \
        (ts)->tv_sec = (tv)->tv_sec;                                    \
	(ts)->tv_nsec = (tv)->tv_usec * 1000;                           \
}
# define TIMESPEC_TO_TIMEVAL(tv, ts) {                                   \
        (tv)->tv_sec = (ts)->tv_sec;                                    \
	        (tv)->tv_usec = (ts)->tv_nsec / 1000;                           \

#ifdef __cplusplus
}
#endif


int ph_media_audio_start(phcall_t *ca, int port,
				   void (*dtmfCallback)(phcall_t *ca, int event), 
				   void (*endCallback)(phcall_t *ca, int event),
			 const char * deviceId, unsigned vad, int cng, int jitter, int noaec);



void ph_media_audio_stop(phcall_t *ca);

#endif

