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

#include "ortp.h"
#ifdef BUILD_SCHEDULER
#include "sessionset.h"
#include "scheduler.h"

/**
 *session_set_init:
 *@ss:	 a SessionSet statically allocated.
 *
 *	Initializes a session set. It is unusefull to call this function on a session set object
 *	returned by session_set_new().
 *
**/


/**
 *session_set_new:
 *
 * Allocates and initialize a new empty session set.
 *
 *Returns: the session set.
**/
SessionSet * session_set_new()
{
	SessionSet *set=g_malloc(sizeof(SessionSet));
	session_set_init(set);
	return set;
}


/**
 *session_set_destroy:
 *@set:	a SessionSet
 * Destroys a session set.
 *
**/

void session_set_destroy(SessionSet *set)
{
	g_free(set);
}

gint session_set_and(SessionSet *sched_set, gint maxs, SessionSet *user_set, SessionSet *result_set)
{
	guint32 *mask1,*mask2,*mask3;
	gint i=0;
	gint j,ret=0;
	mask1=(guint32*)&sched_set->rtpset;
	mask2=(guint32*)&user_set->rtpset;
	mask3=(guint32*)&result_set->rtpset;
	while(i<maxs+1){
		*mask3=(*mask1) & (*mask2);	/* computes the AND between the two masks*/
		/* and unset the sessions that have been found from the sched_set */
		*mask1=(*mask1) & (~(*mask3));
		if ((*mask3)!=0){
			for (j=0;j<32;j++){
				if ( ((*mask3)>>j) & 1){
					ret++;
				}
			}
		}
		i+=32;
		mask1++;
		mask2++;
		mask3++;
	}
	//printf("session_set_and: ret=%i\n",ret);
	return ret;
}

/**
 *session_set_select:
 *@recvs:		a set of rtp sessions to be watched for read events
 *@sends:		a set of rtp sessions to be watched for write events
 *@errors:		a set of rtp sessions to be watched for errors
 *
 *	This function performs similarly as libc select() function, but performs on #RtpSession 
 *	instead of file descriptors.
 *	session_set_select() suspends the calling process until some events arrive on one of the
 *	three sets passed in argument. Two of the sets can be NULL.
 *	The first set @recvs is interpreted as a set of RtpSession waiting for receive events:
 *	a new buffer (perhaps empty) is availlable on one or more sessions of the set, or the last
 *	receive operation with rtp_session_recv_with_ts() would have finished if it were in 
 *	blocking mode.
 *	The second set is interpreted as a set of RtpSession waiting for send events, i.e. the last
 *	rtp_session_send_with_ts() call on a session would have finished if it were in blocking mode.
 *	
 *	When some events arrived on some of sets, then the function returns and sets are changed
 *	to indicate the sessions where events happened.
 *	Sessions can be added to sets using session_set_set(), a session has to be tested to be 
 *	part of a set using session_set_is_set().
 *
 *Returns: the number of sessions on which the selected events happened.
**/
int session_set_select(SessionSet *recvs, SessionSet *sends, SessionSet *errors)
{
	gint ret=0,bits;
	SessionSet temp;
	RtpScheduler *sched=ortp_get_scheduler();
	
	/*lock the scheduler to not read the masks while they are being modified by the scheduler*/
	rtp_scheduler_lock(sched);
	
	while(1){
		/* computes the SessionSet intersection (in the other words mask intersection) between
		the mask given by the user and scheduler masks */
		if (recvs!=NULL){
			bits=session_set_and(&sched->r_sessions,sched->all_max,recvs,&temp);
			if (bits>0){
				ret+=bits;
				/* copy the result set in the given user set */
				session_set_copy(recvs,&temp);
			}
		}
		if (sends!=NULL){
			bits=session_set_and(&sched->w_sessions,sched->all_max,sends,&temp);
			if (bits>0){
				ret+=bits;
				/* copy the result set in the given user set */
				session_set_copy(sends,&temp);
			}
		}
		if (errors!=NULL){
			bits=session_set_and(&sched->e_sessions,sched->all_max,errors,&temp);
			if (bits>0){
				ret+=bits;
				/* copy the result set in the given user set */
				session_set_copy(errors,&temp);
			}
		}
		if (ret>0){
			/* there are set file descriptors, return immediately */
			//printf("There are %i sessions set, returning.\n",ret);
			rtp_scheduler_unlock(sched);
			return ret;
		}
		//printf("There are %i sessions set.\n",ret);
		/* else we wait until the next loop of the scheduler*/
		g_cond_wait(sched->unblock_select_cond,sched->lock);
	}

	return -1;
}
int session_set_and_fdset(SessionSet *ss, fd_set *fd, gint max_fd, SessionSet *tmp)
{
	guint32 *mask1,*mask2,*mask3;
	gint i=0;
	gint j,ret=0;
	mask1=(guint32*)ss;
	mask2=(guint32*)fd;
	mask3=(guint32*)tmp;
	while(i<max_fd){
		*mask3=(*mask1) & (*mask2);	/* computes the AND between the two masks*/
		if ((*mask3)!=0){
			for (j=0;j<32;j++){
				if ( ((*mask3)>>j) & 1){
					ret++;
				}
			}
		}
		i+=32;
		mask1++;
		mask2++;
		mask3++;
	}
	//printf("session_set_and: ret=%i\n",ret);
	return ret;
}
int session_set_select_nosched(SessionSet *recvs, gint hrcv, SessionSet *sends, gint hsnd,  SessionSet *errors, gint herr, struct timeval *timeout)
{
	gint ret=0,error=0,bits;
	fd_set allrecv;
	fd_set allsend;
	fd_set allerr;
	SessionSet temp;
	fd_set *arcv = &allrecv;
	fd_set *asnd = &allsend;
	fd_set *aerr = &allerr;
	gint max_fd=0;

	struct timeval to;
	if(timeout != NULL)
	  {
	  to.tv_sec = timeout->tv_sec;
	  to.tv_usec = timeout->tv_usec;
	  }
	else
	  to.tv_sec = to.tv_usec = 0;

	if(recvs != NULL)
	  session_set_copy_fdset(arcv, recvs);
	else
	  arcv = NULL;

	if(sends != NULL)
	  session_set_copy_fdset(asnd, sends);
	else
	  asnd = NULL;

	if(errors != NULL)
	  session_set_copy_fdset(aerr, errors);
	else
	  aerr = NULL;

	max_fd = (hrcv > hsnd)?hrcv:hsnd;
	if(herr > max_fd)
	  max_fd = herr;

	max_fd++;  
	error = select (max_fd, arcv, asnd, aerr, &to);

	if(error > 0){
		/* computes the SessionSet intersection (in the other words mask intersection) between
		the mask given by the user and scheduler masks */
		if (recvs!=NULL){
			bits=session_set_and_fdset(recvs, arcv, hrcv, &temp);
			if (bits>0){
				ret+=bits;
				/* copy the result set in the given user set */
				session_set_copy(recvs,&temp);
			}
		}
		if (sends!=NULL){
			bits=session_set_and_fdset(sends, asnd, hsnd, &temp);
			if (bits>0){
				ret+=bits;
				/* copy the result set in the given user set */
				session_set_copy(sends,&temp);
			}
		}
		if (errors!=NULL){
			bits=session_set_and_fdset(errors, aerr, herr, &temp);
			if (bits>0){
				ret+=bits;
				/* copy the result set in the given user set */
				session_set_copy(errors,&temp);
			}
		}
		if(!ret)
		  printf("ERROR:ret is 0 and select returned %d\n",error);
		return ret;
	}
	else
	  {
	  printf("ERROR:select returned %d\n",error);
	  return error;
	  }
}

/**
 *session_set_set:
 *@ss:		a set (#SessionSet object)
 *@rtpsession:	a rtp session
 *
 *	This macro adds rtp session @_session to set @_set.
**/

/**
 *session_set_is_set:
 *@ss:		a set (#SessionSet object)
 *@rtpsession:	a rtp session
 *
 *	This macro tests if @_session is part of @_set. 1 is returned if true, 0 else.
**/

/**
 *session_set_clr:
 *@ss:	a set of sessions.
 *@rtpsession: a rtp session.
 *
 *	Removes the @_session from the _set.
 *
 *
**/

#endif


