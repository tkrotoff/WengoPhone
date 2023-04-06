/*
  The oRTP library is an RTP (Realtime Transport Protocol - rfc1889) stack.
  Copyright (C) 2001  Simon MORLAT simon.morlat@linphone.org

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef RTPSESSION_H
#define RTPSESSION_H


#include "rtpport.h"
#include "rtp.h"
#include "payloadtype.h"
#include "sessionset.h"



#ifndef RTP_SESSION_RECV_POOL_SIZE
#define RTP_SESSION_RECV_POOL_SIZE  32
#endif

#ifndef RTP_SESSION_SEND_POOL_SIZE
#define RTP_SESSION_SEND_POOL_SIZE  16
#endif

/*
#define MULTI_SSRC 1
*/

#ifndef TARGET_IS_HPUXKERNEL

	#include <stdio.h>
	#include "str_utils.h"

     #ifndef _WIN32
	#include <sys/types.h>
    	#include <sys/socket.h>
    	#include <errno.h>
    	#include <netinet/in.h>
    	#include <unistd.h>
    	#include <sys/time.h>
     #else
	#include <winsock.h>
#endif   /* _WIN32 */
#else
#endif   /* TARGET_IS_HPUXKERNEL */


#include "rtcp.h"

typedef enum {
	RTP_SESSION_RECVONLY,
	RTP_SESSION_SENDONLY,
	RTP_SESSION_SENDRECV
} RtpSessionMode;


typedef enum {
	RTP_SESSION_RECV_SYNC=1,	/* the rtp session is synchronising in the incoming stream */
	RTP_SESSION_SEND_SYNC=1<<1, /* the rtp session is synchronising in the outgoing stream */
	RTP_SESSION_SCHEDULED=1<<2, /* the rtp session has to be scheduled */
	RTP_SESSION_BLOCKING_MODE=1<<3, /* in blocking mode */
	RTP_SESSION_RECV_NOT_STARTED=1<<4,	/* the application has not started to try to recv */
	RTP_SESSION_SEND_NOT_STARTED=1<<5,  /* the application has not started to send something */
	RTP_SESSION_IN_SCHEDULER=1<<6,	/* the rtp session is in the scheduler list */
	RTP_SESSION_USING_EXT_SOCKETS=1<<7, /* the session is using externaly supplied sockets */
#ifdef MULTI_SSRC
	RTP_SESSION_MULTI_PEER=1<<8, /* the session may communicate with more than 1 peer */
#endif	
	RTP_SESSION_USING_TUNNELS=1<<9

}RtpSessionFlags;

#ifdef MULTI_SSRC
#define RTP_SESSION_MULTI_SSRC(flags) (flags & RTP_SESSION_MULTI_PEER)
#endif

typedef struct _RtpTunnel 
{
	void*			h_tunnel;
        int 			(*tunnel_get_socket) (void *h_tunnel);
	int			(*tunnel_send) (void *h_tunnel, const void *buffer, int size); // tunnel send function
	int			(*tunnel_recv) (void *h_tunnel, void *buffer, int size); // tunnel recv function 
        int                     (*tunnel_close)(void *h_tunnel);   
} RtpTunnel;

	
typedef struct _RtpStream
{
	gint socket;
	gint jitt_comp;   /* the user jitt_comp in miliseconds*/
	gint jitt_comp_time; /* the jitt_comp converted in rtp time (same unit as timestamp) */
	gint max_rq_size;
	struct timeval *timeout;
	struct timeval _timeout;

	RtpTunnel  *tunnel;


#ifndef TARGET_IS_HPUXKERNEL
	queue_t _rq;  /* should not be accessed direclty, use RtpStream::rq instead */
	queue_t _wq;  /* should not be accessed direclty, use RtpStream::wq instead */
#endif
	queue_t *rq;
	queue_t *wq;
#ifdef INET6
	struct sockaddr_storage loc_addr;
	struct sockaddr_storage rem_addr;
#else
	struct sockaddr_in loc_addr;
	struct sockaddr_in rem_addr;
#endif
	guint32 snd_time_offset;/*the scheduler time when the application send its first timestamp*/	
	guint32 snd_ts_offset;	/* the first application timestamp sent by the application */
	guint32 snd_rand_offset;	/* a random number added to the user offset to make the stream timestamp*/
	guint32 snd_last_ts;	/* the last stream timestamp sended */
	/*guint32 snd_ts;	 		 to be unused */
	guint32 rcv_time_offset; /*the scheduler time when the application ask for its first timestamp*/
	guint32 rcv_ts_offset;  /* the first stream timestamp */
	guint32 rcv_query_ts_offset;	/* the first user timestamp asked by the application */
	guint32 rcv_app_ts_offset;  	/* the user timestamp that corresponds to the first timestamp of the stream*/
	guint32 rcv_diff_ts;	/* difference between the first user timestamp and first stream timestamp */
	guint32 rcv_ts;			/* to be unused */
	guint32 rcv_last_ts;	/* the last stream timestamp got by the application */
	guint32 rcv_last_app_ts; /* the last application timestamp asked by the application */	
	guint32 rcv_last_ret_ts; /* the timestamp of the last sample returned (only for continuous audio)*/
	guint16 rcv_seq;
	guint16 snd_seq;
	rtp_stats_t stats;
#ifdef BUILD_SCHEDULER
	GCond *wait_for_packet_to_be_sent_cond;
	GMutex *wait_for_packet_to_be_sent_mutex;
	GCond *wait_for_packet_to_be_recv_cond;
	GMutex *wait_for_packet_to_be_recv_mutex;
#endif
}RtpStream;

typedef struct _RtcpStream
{
	gint socket;
	gint pad;
#ifndef TARGET_IS_HPUXKERNEL
	queue_t _rq;  /* should not be accessed direclty, use RtpStream::rq instead */
	queue_t _wq;  /* should not be accessed direclty, use RtpStream::wq instead */
#endif
	queue_t *rq;
	queue_t *wq;
#ifdef INET6
	struct sockaddr_storage loc_addr;
	struct sockaddr_storage rem_addr;
#else
	struct sockaddr_in loc_addr;
	struct sockaddr_in rem_addr;
#endif
        RtpTunnel *tunnel;

} RtcpStream;

typedef struct _RtpSession RtpSession;

#include "rtpsignaltable.h"

typedef mblk_t* (*RtpDataCallback)(struct _RtpSession*, gpointer, mblk_t*);

#ifdef MULTI_SRC
typedef enum {
	RTP_SOURCE_SENDRECV
	RTP_SOURCE_RECVONLY,
	RTP_SOURCE_SENDONLY,
} RtpSourceMode;

typedef enum {
  RTP_SOURCE_ACCEPT_ALL=1, /* accept any new incoming stream */
}RtpSourceFlags;

#endif

typedef struct rtp_source_s {
	guint32 ssrc;
	guint32 max_seq;        /* highest seq. number seen */
	guint32 cycles;         /* shifted count of seq. number cycles */
	guint32 base_seq;       /* base seq number */
	guint32 bad_seq;        /* last 'bad' seq number + 1 */
	guint32 probation;      /* sequ. packets till source is valid */
	guint32 received;       /* packets received */
	guint32 expected_prior; /* packet expected at last interval */
	guint32 received_prior; /* packet received at last interval */
	gint32  transit;        /* relative trans time for prev pkt */
	guint32 jitter;         /* estimated jitter */
	guint32 lastsrstamp;
	struct timeval lastsrtime;
	struct timeval lastrrtime;
	int     statsChanged;
	int	   lastE;
	char    cname[64];
        gint markbit;
#ifdef MULTI_SRC
  RtpSession *ses;
  RtpProfile *profile;
  RtpCallback accept_src;
  gint mode;
  gint flags;
  RtpSource *next;
  RtpSource *prev;
  RtpStream rtp;
  RtcpStream rtcp;

  bool ownssrc;
  bool iscsrc;
  bool issender;
  bool receivedbye;
  u_int8_t *byereason;
  size_t byereasonlen;
  guint32 byetime;
#endif

} RtpSource;


typedef struct ssrc_queue
{
	RtpSource *q_first;
	RtpSource *q_last;
	RtpSource *q_cur;
	gint q_count;	/*number of ssrc in the RtpSources q */
} ssrc_queue_t;


struct _RtpSession
{
	RtpSession *next;	/* next RtpSession, when the session are enqueued by the scheduler */
	RtpProfile *profile;
	GMutex *lock;
  //	guint32 ssrc;
	gint payload_type;
	gint recv_payload_type;
	gint max_buf_size;
	int  terminated;

	void (*rtcp_rr_sent_cb)(RtpSession *, rtcp_t *);
	void (*rtcp_rr_received_cb)(RtpSession *, rtcp_t *);

	void (*rtcp_sr_sent_cb)(RtpSession *, rtcp_t *);
	void (*rtcp_sr_received_cb)(RtpSession *, rtcp_t *);

#ifdef TARGET_IS_HPUXKERNEL
	mblk_t *dest_mproto; 	/* a M_PROTO that contains the destination address for outgoing packets*/
#else
	gint highest_fd;

#if defined(WIN32) || defined(_WIN32_WCE)
    fd_set		scanfd;
#else
	ortp_fd_set scanfd;
#endif

#endif
	RtpSignalTable on_ssrc_changed;
	RtpSignalTable on_payload_type_changed;
	RtpSignalTable on_telephone_event_packet;
	RtpSignalTable on_telephone_event;
	RtpSignalTable on_timestamp_jump;
	RtpSignalTable on_bye_packet;
	RtpSignalTable on_cng_packet;
#ifdef MULTI_SSRC
	RtpSignalTable on_new_ssrc;
	RtpSignalTable on_ssrc_collision;
	RtpSignalTable on_cname_collision;
#endif
	RtpStream rtp;
	RtcpStream rtcp;
	RtpSessionMode mode;
#ifdef BUILD_SCHEDULER
	struct _RtpScheduler *sched;
#endif
	guint32 flags;
	rtp_stats_t stats;
	gint mask_pos;	/* the position in the scheduler mask of RtpSession */
	gpointer user_data;
	
	/* telephony events extension */
	gint telephone_events_pt;	/* the payload type used for telephony events */
	mblk_t *current_tev;		/* the pending telephony events */
	
	RtpDataCallback		on_data;
	gpointer		    on_data_info;

	mblk_t*				recvbuffer[RTP_SESSION_RECV_POOL_SIZE];
	mblk_t*				sendbuffer[RTP_SESSION_SEND_POOL_SIZE];
	struct				timeval start_time;

	guint32				base_timestamp;
	RtpSource	local;

       gint cng_pt;	/* the payload type used for CN packets */
       gint markbit;

  gint adapt_rx_rate;   /* if > 0, the number of packet to be throw if rx queue becomes full : ex.10 - each tenth packet will be thrown */
  gint rtp_rq_throw_cnt;    /* counter for throwing packets */
  gint rtp_rq_threshold;    /* number of packets when the queue is considered as becoming full */

#ifdef MULTI_SSRC
 
  #define MAX_PEERS 3
  ssrc_queue_t peers;
  gint totalcnt;
  gint sendercnt;
  gint activecnt;
#ifdef INET6
  struct sockaddr_storage loc_addr;
#else
  struct sockaddr_in loc_addr;
#endif
#else
  RtpSource	peer;
#endif
};
	



#ifdef __cplusplus
extern "C"
{
#endif


#ifdef DUMMY_USE_HTTP_TUNNEL
void rtp_session_set_sock_mode(RtpSession * session, tunnel_sock_mode_t s_mode);
#endif

void rtp_session_init(RtpSession *session, gint mode, const char *cname);
/* for use in the HPUX kernel */
#define rtp_session_set_rq(s,q) (s)->rtp.rq=(q)
#define rtp_session_set_wq(s,q) (s)->rtp.wq=(q)

#define rtp_session_lock(session) 	g_mutex_lock(session->lock)
#define rtp_session_unlock(session) g_mutex_unlock(session->lock)

#define rtp_session_set_max_rq_size(s,size) (s)->rtp.max_rq_size=(size)

RtpSession *rtp_session_new (gint mode);

void rtp_session_set_scheduling_mode(RtpSession *session, gint yesno);
void rtp_session_set_blocking_mode(RtpSession *session, gint yesno);
void rtp_session_set_profile(RtpSession *session,RtpProfile *profile);
#define rtp_session_get_profile(session)	(session)->profile

#define rtp_session_set_flag(session,flag) (session)->flags|=(flag)
#define rtp_session_unset_flag(session,flag) (session)->flags&=~(flag)
int rtp_session_signal_connect(RtpSession *session,char *signal, RtpCallback cb, gpointer user_data);
int rtp_session_signal_disconnect_by_callback(RtpSession *session,char *signal, RtpCallback cb);
void rtp_session_set_ssrc(RtpSession *session, guint32 ssrc);
void rtp_session_set_seq_number(RtpSession *session, guint16 seq);
guint16 rtp_session_get_seq_number(RtpSession *session);
void rtp_session_set_jitter_compensation(RtpSession *session, int milisec);
#ifndef TARGET_IS_HPUXKERNEL
/* setting a local address is not provided for the HPUX kernel environement */
int rtp_session_set_local_addr(RtpSession *session,gchar *addr, gint port);
#endif

#ifdef TARGET_IS_HPUXKERNEL
gint rtp_session_set_remote_addr(RtpSession *session,struct sockaddr_in *dest);
#else
gint rtp_session_set_remote_addr(RtpSession *session,gchar *addr, gint port);
#endif

/* alternatively to the set_remote_addr() and set_local_addr(), an application can give
a valid socket to be used by the RtpSession */
void rtp_session_set_sockets(RtpSession *session, gint rtpfd, gint rtcpfd);


void rtp_session_set_tunnels(RtpSession *session, RtpTunnel *tunrtp, RtpTunnel *tunrtcp);

int rtp_session_set_payload_type(RtpSession *session, int paytype);
int rtp_session_get_payload_type(RtpSession *session);

//#define rtp_session_set_cng_pt(session,pt) (session)->cng_pt = (pt)
void rtp_session_set_cng_pt(RtpSession *session,int pt);

//#define rtp_session_set_markbit(session,mark) (session)->markbit = (mark)
void  rtp_session_set_markbit(RtpSession* session, int mark);
#define rtp_source_set_markbit(src,mark) (src)->markbit = (mark)

void rtp_session_set_timeout(RtpSession *session,guint timeout);

/*low level recv and send functions */
mblk_t * rtp_session_recvm_with_ts (RtpSession * session, guint32 user_ts);
mblk_t * rtp_session_create_packet(RtpSession *session,gint header_size, char *payload, gint payload_size);
mblk_t *rtp_session_create_specific_payload_packet(RtpSession *session, gint header_size,  gint payload_type, char *payload, gint payload_size );
gint rtp_session_sendm_with_ts (RtpSession * session, mblk_t *mp, guint32 userts);

/* high level recv and send functions */
gint rtp_session_recv_with_ts(RtpSession *session, gchar *buffer, gint len, guint32 time, gint *have_more);
gint rtp_session_send_with_ts(RtpSession *session, gchar *buffer, gint len, guint32 userts);


guint32 rtp_session_get_current_send_ts(RtpSession *session);
guint32 rtp_session_get_current_recv_ts(RtpSession *session);
void rtp_session_reset(RtpSession *session);
void rtp_session_uninit(RtpSession *session);
void rtp_session_destroy(RtpSession *session);

#define rtp_session_get_stats(session) (&((session)->stats))
#define rtp_session_get_highest_fd(session) (&((session)->highest_fd))
#define rtp_session_reset_stats(session)	memset(&((session)->stats),0,sizeof(rtp_stats_t))

#define rtp_session_set_data(session,data)	(session)->user_data=(data)
#define rtp_session_get_data(session,data)	((session)->user_data)

#define rtp_session_max_buf_size_set(session,bufsize)	(session)->max_buf_size=(bufsize)

void rtp_parse(RtpSession *session, mblk_t *mp);

/* in use with the scheduler to convert a timestamp in scheduler time unit (ms) */
guint32 rtp_session_ts_to_t(RtpSession *session,guint32 timestamp);


/* packet api */
/* the first argument is a mblk_t. The header is supposed to be not splitted  */
#define rtp_set_markbit(mp,value)		((rtp_header_t*)((mp)->b_rptr))->markbit=(value)
#define rtp_get_markbit(mp)		(((rtp_header_t*)((mp)->b_rptr))->markbit)
#define rtp_set_seqnumber(mp,seq)	((rtp_header_t*)((mp)->b_rptr))->seq_number=(seq)
#define rtp_get_seqnumber(mp)	(((rtp_header_t*)((mp)->b_rptr))->seq_number)
#define rtp_set_timestamp(mp,ts)	((rtp_header_t*)((mp)->b_rptr))->timestamp=(ts)
#define rtp_get_timestamp(mp)	(((rtp_header_t*)((mp)->b_rptr))->timestamp)
#define rtp_set_ssrc(mp,_ssrc)		((rtp_header_t*)((mp)->b_rptr))->ssrc=(_ssrc)
void rtp_add_csrc(mblk_t *mp,guint32 csrc);
#define rtp_set_payload_type(mp,pt)	((rtp_header_t*)((mp)->b_rptr))->paytype=(pt)


void rtcp_send_bye_packet(RtpSession *session, gchar *reason);
#define rtp_session_send_bye(ses, res) rtcp_send_bye_packet(ses,res)

#ifdef MULTI_SSRC

RtpSource *rtp_session_add_peer(RtpSession *session, gchar *src, int outrtp, int inrtp, int outrtcp, int inrtcp);
void rtp_session_remove_peer(RtpSession *session, RtpSource *src);

RtpSource *rtp_session_find_peer(RtpSession *session, gint ssrc);
void rtp_session_update_peer_sdes(RtpSource *src, int type, char *data, int len); 
void rtp_session_join_mc(RtpSession *session, gchar *addr);
void rtp_session_leave_mc(RtpSession *session, gchar *addr);
void rtp_session_leave_all_mc(RtpSession *session);

RtpSource *rtp_session_get_first_peer(RtpSession *session);
RtpSource *rtp_session_get_next_peer(RtpSession *session);

#define rtp_session_get_peers_cnt(s) (s->totalcount)
#define rtp_session_get_sender_cnt(s) (s->sendercount)
#define rtp_session_get_avtive_cnt(s) (s->activecount)

int rtp_session_set_src_payload_type(RtpSession *session, RtpSource *src, int paytype);
int rtp_session_get_src_payload_type(RtpSession *session , RtpSource *src );

void rtp_session_set_timeout(RtpSession *session,guint timeout);

/* low level recv and send functions */
mblk_t * rtp_session_src_recvm_with_ts (RtpSession * session, RtpSource *src, guint32 user_ts);
mblk_t * rtp_session_src_create_packet(RtpSession *session, RtpSource *src, gint header_size, char *payload, gint payload_size);
gint rtp_session_src_sendm_with_ts (RtpSession * session, RtpSource *src, mblk_t *mp, guint32 user_ts);

/* high level recv and send functions */
gint rtp_session_src_recv_with_ts(RtpSession *session,  RtpSource *src, gchar *buffer, gint len, guint32 user_ts, gint *have_more);
gint rtp_session_src_send_with_ts(RtpSession *session, RtpSource *src, gchar *buffer, gint len, guint32 userts);

RtpSource *rtp_session_find_peer(RtpSession *session, guint32 ssrc);

void rtp_source_set_profile(RtpSource *src, RtpProfile *profile);
#define rtp_src_get_profile(src)	(src)->profile

#define rtp_source_set_flag(src,flag) (src)->flags|=(flag)
#define rtp_source_unset_flag(src,flag) (src)->flags&=~(flag)

#endif /* MULTI_SSRC */


void rtp_session_set_cname(RtpSession *s, const char *cname);


#ifdef RTCP
void rtp_session_add_contributing_source(RtpSession *session, guint32 csrc, 
										 gchar *cname, gchar *name, gchar *email, 
										 gchar *phone, gchar *loc, gchar *tool, gchar *note);
mblk_t *rtp_session_remove_contributing_sources(RtpSession *session, guint32 ssrc);
mblk_t* rtp_session_create_rtcp_sdes_packet(RtpSession *session);

#endif


#ifdef __cplusplus
}
#endif


#endif
