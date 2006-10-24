 /*
	Zenbow Secure VoIP.
	Copyright (C) 2006 Everbee Networks 

	* This program is free software; you can redistribute it and/or
	* modify it under the terms of the GNU General Public License
	* as published by the Free Software Foundation; either version 2
	* of the License, or (at your option) any later version.

	* This program is distributed in the hope that it will be useful,
	* but WITHOUT ANY WARRANTY; without even the implied warranty of
	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	* GNU General Public License for more details.

	* You should have received a copy of the GNU General Public License
	* along with this program; if not, write to the Free Software
	* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

	* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
	* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
	* COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
	* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
	* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
	* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
	* OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <assert.h>
#include <osip2/osip.h>
#include <sdp.h>
#include <session.h>
#include <crypto.h>
#include <sessionsManager.h>
#include <svoip.h>
#include <svoip_phapi.h>
#include <rtpsession.h>

#include <phmedia.h> //for accessing cid from _rtp_session

EVRB_CIPHER_MODE	g_cipherMode = EVRB_NOCRYPTO;

void sVoIP_phapi_setCipherMode(int cipherMode)
{
	g_cipherMode = cipherMode;
	smSetAllCipherMode(cipherMode);
}

int sVoIP_phapi_isCrypted(int cid)
{
	int		i;
	int		cipherMode = 0;
	EVRB_SESSION	*ses = NULL;

	i = smSession(cid, &ses, &cipherMode);
	if (i != SOK)
	{ 
		return 0;
	}
	return (cipherMode != EVRB_NOCRYPTO);
}

int sVoIP_phapi_handle_invite_in(int cid, osip_message_t *sip)
{
	int			i = 0;
	int			cipherMode = 0;
	EVRB_SESSION*		ses = NULL;
	osip_body_t		*body;

	assert(cid >= 0 && sip != NULL);
#ifdef _DEBUG_
	fprintf(stdout,"sVoIP_phapi_handle_invite_in\n");
#endif
	i= sVoIP_init();
	if (SOK != i)
	{
		SVOIP_PERROR(i,stdout,"sVoIP_phapi_handle_invite_in");
		return INIT_ERROR;
	}

	// Check that we have sdp
	assert(sip->content_type != NULL && 
	       sip->content_type->type != NULL &&
	       sip->content_type->subtype != NULL);
	if (strcmp(sip->content_type->type, "application") ||
	    strcmp(sip->content_type->subtype, "sdp"))
	  {
	    SVOIP_PERROR(BAD_SIP_MESSAGE, stdout,"sVoIP_phapi_handle_invite_in");
	    return -1;	    
	  }
	/* Session is created here */
	if (g_cipherMode == EVRB_NOCRYPTO)
		return 0;
	i = sVoIP_preCreateSession(cid, g_cipherMode);
	if (i != 0)
	  {
#ifdef _DEBUG_
	    fprintf(stdout,"sVoIP_preCreateSession failed %i\n",i);
#endif
	    return 0;
	  }
#ifdef _DEBUG_
	fprintf(stdout,"incoming INVITE message %i %i %p %i\n" ,cid,i,ses,cipherMode); fflush(stdout);
#endif
	i = osip_message_get_body(sip, 0, &body);
	if (i != SOK)
	{
#ifdef _DEBUG_
		fprintf(stdout,"sVoIP_handle_invite_in: osip_message_get_body error %i\n",i);
#endif
		return -1;
	}
	i = sVoIP_SIPHandleINVITE2(cid,body->body,body->length);
	if (i != SOK)
	{
		SVOIP_PERROR(i,stdout,"sVoIP_phapi_handle_invite_in");
		return i;
	}
	return SOK;
}

int sVoIP_phapi_handle_ok_in(int cid, osip_message_t *sip)
{
	int			i = 0;
	int			cipherMode = 0;
	EVRB_SESSION*		ses = NULL;
	osip_body_t		*body;

	assert(cid >= 0 && sip != NULL);
#ifdef _DEBUG_
	fprintf(stdout,"sVoIP_phapi_handle_ok_in\n");
#endif
#ifdef _DEBUG_
	fprintf(stdout,"incoming OK message %i %i %p %i\n" ,cid,i,ses,cipherMode); fflush(stdout);
#endif
	// Check sessionn
	i = smSession(cid, &ses, &cipherMode);
	if (i != SOK)
	{ 
		SVOIP_PERROR(i,stdout,"sVoIP_phapi_handle_ok_out");
		return BAD_SESSION;
	}
	i = osip_message_get_body(sip, 0, &body);
	if (i != SOK)
	{
#ifdef _DEBUG_
		fprintf(stdout,"sVoIP_handle_ok_in: osip_message_get_body error %i\n",i);
#endif
		return -1;
	}
	i = sVoIP_SIPHandleOK2(cid, body->body, body->length);
	if (i != SOK)
	{
		SVOIP_PERROR(i,stdout,"sVoIP_phapi_handle_ok_in");
		return i;
	}
	return SOK;
}

int sVoIP_phapi_handle_bye_in(int cid, osip_message_t *sip)
{
	int			i = 0;
	int			cipherMode = 0;
	EVRB_SESSION*		ses = NULL;

	assert(cid >= 0 && sip != NULL);
#ifdef _DEBUG_
	fprintf(stdout,"sVoIP_phapi_handle_bye_in,\n");
#endif
	// Check sessionn
	i = smSession(cid, &ses, &cipherMode);
	if (i != SOK)
	{ 
		SVOIP_PERROR(i,stdout,"sVoIP_phapi_handle_bye_in");
		return BAD_SESSION;
	}
#ifdef _DEBUG_
	fprintf(stdout,"incoming BYE message %i %i %p %i\n" ,cid,i,ses,cipherMode); fflush(stdout);
#endif
	smClose(cid);
	return SOK;
}

int sVoIP_phapi_handle_cancel_in(int cid, osip_message_t *sip)
{
	int			i = 0;
	int			cipherMode = 0;
	EVRB_SESSION*		ses = NULL;

	assert(cid >= 0 && sip != NULL);
#ifdef _DEBUG_
	fprintf(stdout,"sVoIP_phapi_handle_cancel_in\n");
#endif
	// Check sessionn
	i = smSession(cid, &ses, &cipherMode);
	if (i != SOK)
	{ 
		SVOIP_PERROR(i,stdout,"sVoIP_phapi_handle_bye_out");
		return BAD_SESSION;
	}
#ifdef _DEBUG_
	fprintf(stdout,"incoming CANCEL message %i %i %p %i\n" ,cid,i,ses,cipherMode); fflush(stdout);
#endif
	smClose(cid);
	return SOK;
}

int sVoIP_phapi_handle_invite_out(int cid, osip_message_t *sip)
{
	int			i = 0;
	int			cipherMode = 0;
	EVRB_SESSION*		ses = NULL;
	char			*sdp;
	size_t			size;
	osip_body_t		*body;

	assert(cid >= 0 && sip != NULL);
#ifdef _DEBUG_
	fprintf(stdout,"sVoIP_phapi_handle_invite_out\n");
#endif
	i= sVoIP_init();
	if (SOK != i)
	{
		SVOIP_PERROR(i,stdout,"sVoIP_phapi_handle_invite_out");
		return INIT_ERROR;
	}

	// Check that we have sdp
	assert(sip->content_type != NULL && 
	       sip->content_type->type != NULL &&
	       sip->content_type->subtype != NULL);
	if (strcmp(sip->content_type->type, "application") ||
	    strcmp(sip->content_type->subtype, "sdp"))
	  {
	    SVOIP_PERROR(BAD_SIP_MESSAGE, stdout,"sVoIP_phapi_handle_invite_out");
	    return -1;	    
	  }
	// Check session
	i = smSession(cid, &ses, &cipherMode);
	if (i == SESSION_NOT_INITIALIZE)
	{ 
		/* Session is created here */
		if (g_cipherMode == EVRB_NOCRYPTO)
			return 0;
		i = sVoIP_preCreateSession(cid, g_cipherMode);
		if (i != 0)
		{
#ifdef _DEBUG_
			fprintf(stdout,"sVoIP_preCreateSession failed %i\n",i);
#endif
			return 0;
		}
	}
#ifdef _DEBUG_
	fprintf(stdout,"outgoing INVITE message %i %i %p %i\n" ,cid,i,ses,cipherMode); fflush(stdout);
#endif
	i = sVoIP_SIPAugmentINVITE2(cid,&sdp,&size);
	if (i != SOK)
	{
		SVOIP_PERROR(i,stdout,"sVoIP_phapi_handle_invite_out");
		return i;
	}
	// Add SDP field in SIP message
	i = osip_message_set_body(sip, sdp, size);
	if (i != SOK)
	{
#ifdef _DEBUG_
		fprintf(stdout,"sVoIP_handle_invite_out: osip_message_set_body error %i\n",i);
#endif
		free(sdp);
		return -1;
	}
	free(sdp);
	return SOK;
}

int sVoIP_phapi_handle_ok_out(int cid, osip_message_t *sip)
{
	int			i = 0;
	int			cipherMode = 0;
	EVRB_SESSION*		ses = NULL;
	char			*sdp;
	int			size;

	assert(cid >= 0 && sip != NULL);
#ifdef _DEBUG_
	fprintf(stdout,"sVoIP_phapi_handle_ok_out\n");
#endif
	// Check sessionn
	i = smSession(cid, &ses, &cipherMode);
	if (i != SOK)
	{ 
		SVOIP_PERROR(i,stdout,"sVoIP_phapi_handle_ok_out");
		return BAD_SESSION;
	}
#ifdef _DEBUG_
	fprintf(stdout,"outgoing OK message %i %i %p %i\n" ,cid,i,ses,cipherMode); fflush(stdout);
#endif
	i = sVoIP_SIPAugmentOK2(cid,&sdp,&size);
	if (i != SOK)
	{
		SVOIP_PERROR(i,stdout,"sVoIP_phapi_handle_ok_out");
		return i;
	}
	// Add SDP field in SIP message
	i = osip_message_set_body(sip, sdp, size);
	if (i != SOK)
	{
#ifdef _DEBUG_
		fprintf(stdout,"sVoIP_handle_ok_out: osip_message_set_body error %i\n",i);
#endif
		free(sdp);
		return -1;
	}
	free(sdp);
	return SOK;
}

int sVoIP_phapi_handle_bye_out(int cid, osip_message_t *sip)
{
	int			i = 0;
	int			cipherMode = 0;
	EVRB_SESSION*		ses = NULL;

	assert(cid >= 0 && sip != NULL);
#ifdef _DEBUG_
	fprintf(stdout,"sVoIP_phapi_handle_bye_out\n");
#endif
	// Check sessionn
	i = smSession(cid, &ses, &cipherMode);
	if (i != SOK)
	{ 
		SVOIP_PERROR(i,stdout,"sVoIP_phapi_handle_bye_out");
		return BAD_SESSION;
	}
#ifdef _DEBUG_
	fprintf(stdout,"outgoing BYE message %i %i %p %i\n" ,cid,i,ses,cipherMode); fflush(stdout);
#endif
	smClose(cid);
	return SOK;
}

#ifdef _DEBUG_
int initialize = 0;
#endif

void sVoIP_phapi_recvRtp(RtpSession *session, gpointer error, mblk_t *mp){
	size_t			len;
	int			i;
	int			cid;

#ifdef _DEBUG_
	fprintf(stdout,"sVoIP_phapi_recvRtp\n");
#endif
	cid = session->externalID;
	if (cid < 0)
	  return;
	len = mp->b_wptr - mp->b_rptr;
	i = sVoIP_RTPrecv(cid,mp->b_rptr,&len);
	if (SOK != i)
	  {
	    SVOIP_PERROR(CIPHER_ERROR,stdout,"sVoIP_phapi_recvRtp");
	    //	    *error = CIPHER_ERROR;
	    return;
	  }
	mp->b_wptr = mp->b_rptr+len;
}

void sVoIP_phapi_sendRtp(RtpSession *session, gpointer error, mblk_t *mp){
	size_t			len;
	int			i;
	int			cid;

#ifdef _DEBUG_
	fprintf(stdout,"sVoIP_phapi_sendRtp\n");
#endif
	cid = session->externalID;
	if (cid < 0)
	  return;
	len = mp->b_wptr - mp->b_rptr;
	i = sVoIP_RTPsend(cid,mp->b_rptr,&len);
	if (SOK != i)
	  {
	    SVOIP_PERROR(CIPHER_ERROR,stdout,"sVoIP_phapi_sendRtp");
	    // *error = CIPHER_ERROR;
	    return;
	  }
	mp->b_wptr=mp->b_rptr+len;
}
