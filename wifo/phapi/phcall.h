#ifndef __PHCALL_H__
#define __PHCALL_H__ 1

#define PH_MAX_CALLS  32

#ifdef __cplusplus
extern "C"
{
#endif

struct phConfig;

struct ph_msession_s;

struct phcall {
  int cid;
  int did;
  int txcid;    /* call id to which we're trasferring this call */
  int vlid;     /* virtual line id */
  int redirs;
  char remote_sdp_audio_ip[64];
  int  remote_sdp_audio_port;
  char remote_sdp_video_ip[64];
  int  remote_sdp_video_port;
  char audio_payload_name[32];
  int  audio_payload;
  char video_payload_name[32];
  int video_payload;
  int wanted_streams;
  int local_sdp_audio_port;
  int local_sdp_video_port;
  struct phConfig *cfg;
  int  _hasaudio;
  int  isringing;
  int  remotehold;
  int  localhold;
  int  localresume;
  int  localrefer;
  int  remoterefer;
#if 0
  void *ph_audio_stream;
#endif
  void *ph_video_stream; /* structure holding specific information on the video stream. NULL when the stream is not activated */
  int  rcid;    /* Call ID for the call created by REFER */
  int  rdid;    /* REFER did */
  int  closereq;  /*  close requested by lowerl layer */
  char cng_name[32];
  char cname[64]; /* CNAME Field for RTCP reports */
  int  cng;
  int  nego_mflags;   /* current negociated media flags for the call */
  int  user_mflags;   /* user media flags for the call (flags given by the user when placing or accepting the call) */
  struct ph_msession_s *mses;
};

typedef struct phcall phcall_t;

/*
 * phcall functions
 *
 */

phcall_t *ph_locate_call_by_cid(int cid);
phcall_t *ph_locate_call_by_rcid(int cid);
phcall_t *ph_locate_call_by_rdid(int did);

#ifdef __cplusplus
}
#endif

#endif
