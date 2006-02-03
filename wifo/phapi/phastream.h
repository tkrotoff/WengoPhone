#ifndef __PHASTREAM_H__
#define __PHASTREAM_H__ 1


# define MAX_IN_BUFFERS 4
# define USED_IN_BUFFERS 4
# define MAX_OUT_BUFFERS 4
# define USED_OUT_BUFFERS 4
# define AUDIO_INBUF_SIZE  4096
# define AUDIO_OUTBUF_SIZE 4096



struct circbuf
{
	int  cb_rdx;
	int  cb_wrx;
	int  cb_cnt;
	int  cb_siz;
	char *cb_buf;
};


struct recording
{
	short *samples;
	int chunksize;
	int nchannels;
	int position;
	FILE *fd;
};
typedef struct recording recording_t;

#define DTMF_LOCK(s) g_mutex_lock(s->dtmfi.dtmfg_lock)
#define DTMF_UNLOCK(s) g_mutex_unlock(s->dtmfi.dtmfg_lock)


struct dtmf_info
{
#define DTMFQ_MAX 32
  unsigned short   dtmfq_buf[DTMFQ_MAX];
  int   dtmfq_wr;
  int   dtmfq_rd;
  int   dtmfq_cnt;

  enum { DTMF_IDLE, DTMF_GEN, DTMF_SILENT } dtmfg_phase;
  int   dtmfg_len;
  struct dtmfgen dtmfg_ctx;
  GMutex *dtmfg_lock;
};



/* 

     DTX decision is made up after SIL_SETUP ms of low mean magnitude. The mean magnitude is calculated using sliding window of PWR_WINDOW ms
     PWR_WINDOW should be short - like 32ms
     The long mean magnitude used to generate the CNG frame is the sum of LONG_PWR_WINDOW mean magnitudes, calculated using also the sliding window 
     LONG_PWR_WINDOW is set to 64 according to dynamic range (0-127dB) of signal to be sent in CNG frame
     Each PWR_WINDOW ms this sum of mean magnitudes is updated.
     
*/
struct vadcng_info
{


#define NOISE_START_DELAY   4000 /* in ms, the delay to start noise generation */  
#define SIL_SETUP       500   /* max time in ms  before stopping transfer */
#define PWR_WINDOW      128    //32    /* power calculate window in ms, for 8kHz il will be 32*8 samples, to optimize should be power of 2 */
#define LONG_PWR_WINDOW 64    /* long mean power window, to optimize should be power of 2 */
#define DTX_RETRANSMIT  10  /* timeout in seconds to retransmit cng packet */
#define RTP_RETRANSMIT  3  /* timeout in seconds to retransmit rtp or dummy CNG  packet if VAD activated */
  unsigned int vad;         /* VAD/DTX are activated or not, if b31 = 1,  the lower 31 bits indicates the wanted level */
  int cng;         /* CNG is activated or not for this stream */
  int cng_pt;       /* CNG payload type */
  struct timeval last_dtx_time;      /* time of last cng packet transfer */
  char got_cng;
  char lastsil;
  
  unsigned int long_pwr[LONG_PWR_WINDOW];     /* long power table */
  unsigned int long_mean_pwr;
  int long_pwr_shift;     /* shift  to optimize long mean magnitude calculation */
  int long_pwr_pos;      /* current power index */

  unsigned int *pwr;         /* power table */
  unsigned int pwr_threshold;   /* power threshold */
  unsigned int mean_pwr;     /* mean power of PWR_WINDOW ms*/
  int pwr_pos;      /* current power index */
  int pwr_size;     /* size of magnitude window */
  int pwr_shift;     /* shift  to optimize mean magnitude calculation */
  int sil_cnt;      /* current number of silence samples */
  int sil_max;      /* number of silence samples before stopping transfer */ 
  char *noise;      /* pointer to noise pattern */
  unsigned int noise_max;
  int nidx;      /* index for noise pattern */

#if 0
  GMutex *cng_lock;

#define CNG_LOCK(s) g_mutex_lock(s->cng_lock)
#define CNG_UNLOCK(s) g_mutex_unlock(s->cng_lock)
#else
#define CNG_LOCK(s)
#define CNG_UNLOCK(s)
#endif

  struct timeval last_noise_sent_time;  /* time of last sent noise packet */
};



#define MAX_FRAME_SIZE 160
#define PCM_TRACE_LEN (50*MAX_FRAME_SIZE)   /* data are stored for 50 frames of 10ms */ 
#define MAX_OUTPUT_LATENCY 250  /* (milliseconds) */

enum EC_PHASE { EC_PRETRAINING=1, EC_TRAINING };

struct ph_audio_stream
{
  struct phmstream ms;
  void   *drvinfo;

  ph_mediabuf_t      *mixbuf;
  struct dtmf_info   dtmfi;
  struct vadcng_info cngi;

  int    hdxmode;
  int    hdxsilence;

#ifdef DO_ECHO_CAN
  void  *ec;
  struct circbuf pcmoutbuf;
  unsigned long sent_cnt; /* total char* sent to the echo canceller circbuf */
  unsigned long recv_cnt; /* total char* asked from the echo canceller circbuf */
  unsigned long read_cnt; /* total char* actually read out of the echo canceller circbuf */
  unsigned long mic_current_sample; /* sample number of the last sample that the microphone recorded. 0 is based on the first chunk received from the network */
  unsigned long spk_current_sample; /* sample number of the last sample that the speaker played. 0 is based on the first chunk received from the network */

  int      ec_phase;
  int      audio_loop_latency;
  int      underrun;
  GMutex   *ecmux;
#endif
 
  void   (*dtmfCallback)(void *ca, int dtmf);
  int clock_rate;
  int    actual_rate;                       /* actual sampling rate */

  void  *rec_resample_ctx;
  void  *play_resample_ctx;

  struct timeval last_rtp_recv_time;  /* time of last received data packet */
  struct timeval last_rtp_sent_time;  /* time of last sent data packet */
  struct timeval now;                /* time of thread's current execution */

  int activate_recorder; /** 0/1 will init a recorder on "recording.raw" */
  int record_send_stream; /** 0/1 will init a recorder on "recording.raw" */
  int record_recv_stream; /** 0/1 will init a recorder on "recording.raw" */
  recording_t recorder; /** recording structure associated with the activate_recorder int */
  recording_t send_stream_recorder;
  recording_t recv_stream_recorder;


  int using_out_callback; /** used to choose the location where network data is recorded in the echo canceller circular buffer */
  void *lastframe;   /** last played frame to be reused in the case of underflow */

};

typedef struct ph_audio_stream phastream_t;


void ph_gen_noice();
void pg_gen_silence();

//void ph_telephone_event(RtpSession *rtp_session, int event, phcall_t *ca);
void ph_telephone_event(RtpSession *rtp_session, int event, struct ph_msession_s *s);

#if 0
int ph_media_audio_start(phcall_t *ca, int port,
				   void (*dtmfCallback)(phcall_t *ca, int event), 
				   void (*endCallback)(phcall_t *ca, int event),
			 const char * deviceId, unsigned vad, int cng, int jitter, int noaec);
#endif

void ph_generate_out_dtmf(phastream_t *stream, short *signal, int siglen, long timestamp);

/** first version of recodring functions (mainly used for audio processing debugging purpose) */
void ph_media_audio_recording_init(recording_t *recording, const char *filename, int nchannels, int chunksize);
void ph_media_payload_recording_init(recording_t *recording, const char *filename);
void ph_media_audio_recording_close(recording_t *recording);
static void ph_media_audio_recording_dump(recording_t *recording);
void ph_media_audio_recording_record_one(recording_t *recording, short c1, short c2, short c3);
void ph_media_payload_record(recording_t *recording, const void *payload, int size);

#endif
