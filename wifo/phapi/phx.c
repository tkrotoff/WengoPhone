/*
 * phapi   phone api
 *
 * Copyright (C) 2004        Vadim Lebedev <vadim@mbdsys.com>
 * Parts Copyright (C) 2002,2003   Aymeric Moizard <jack@atosc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software F undation; either version 2,
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
 * @file phapi.c
 * @brief softphone  API
 *
 * phapi is a library providing simplified api to create VOIP sessions
 * using eXosip library oSIP stack and oRTP stcak 
 * <P>
 */

#include "phapi.h"
#include "phx.h"
#include "phcall.h"

MY_DLLEXPORT int phx_GetVersion(phConfig_t *cfg)
{
  return phGetVersion();
}

#ifdef EMBED
MY_DLLEXPORT int phx_GetAllVersions(struct phversions *version)
{
  phGetAllVersions(version);
}
#endif

#if 0
MY_DLLEXPORT int phx_PlaceCall3(phConfig_t *cfg, const char *from, const char *uri, void *userdata, int rcid, int wanted_streams)
{
  return phPlaceCall3(from, uri, userdata, rcid, wanted_streams);
}


MY_DLLEXPORT int phx_PlaceCall(phConfig_t *cfg, const char *from, const char *uri, void *userdata)
{
  return phPlaceCall3(from, uri, userdata, 0, PH_STREAM_AUDIO);

}

MY_DLLEXPORT int phx_PlaceCall2(phConfig_t *cfg, const char *from, const char *uri, void *userdata, int rcid)
{
  return phPlaceCall3(from, uri, userdata, rcid, PH_STREAM_AUDIO);

}
#endif


MY_DLLEXPORT int phx_SendMessage(phConfig_t *cfg, const char *from, const char *uri, const char *buff)
{
  return phSendMessage(from, uri, buff);
}

MY_DLLEXPORT int phx_Subscribe(phConfig_t *cfg, const char *from, const char *to, const int winfo)
{
  return phSubscribe(from, to, winfo);
}


MY_DLLEXPORT int phx_Publish(phConfig_t *cfg, const char *from, const char *to, const int winfo, const char * content_type, const char * content)
{
  return phPublish(from, to, winfo, content_type, content);
}


MY_DLLEXPORT int phx_Refresh(phConfig_t *cfg)
{
  phRefresh();
  return 1;
}


MY_DLLEXPORT int phx_SendOptions(phConfig_t *cfg, const char *from, const char *uri)
{
  return phSendOptions(from, uri);
}


MY_DLLEXPORT int phx_AcceptCall3(phConfig_t *cfg, int cid, void *userData, int wanted_streams)
{
  return phAcceptCall3(cid, userData, wanted_streams);
}

MY_DLLEXPORT int phx_AcceptCall2(phConfig_t *cfg, int cid, void *userData)
{
  return phAcceptCall3(cid, userData, PH_STREAM_AUDIO);
}


MY_DLLEXPORT int phx_RejectCall(phConfig_t *cfg, int cid, int reason)
{
  return phRejectCall(cid, reason);
}


MY_DLLEXPORT int phx_RingingCall(phConfig_t *cfg, int cid)
{
  return phRingingCall(cid);
}


MY_DLLEXPORT int phx_CloseCall(phConfig_t *cfg, int cid)
{
  return phCloseCall(cid);
}

MY_DLLEXPORT int phx_BlindTransferCall(phConfig_t *cfg, int cid, const char *uri)
{
  return phBlindTransferCall(cid, uri);
}

MY_DLLEXPORT int phx_TransferCall(phConfig_t *cfg, int cid, int tcid)
{
  return phTransferCall(cid, tcid);
}


MY_DLLEXPORT int phx_ResumeCall(phConfig_t *cfg, int cid)
{
  return phResumeCall(cid);

}

MY_DLLEXPORT int phx_HoldCall(phConfig_t *cfg, int cid)
{
  return phHoldCall(cid);
}

MY_DLLEXPORT int phx_SetFollowMe(phConfig_t *cfg, const char *uri)
{
  return phSetFollowMe(uri);
}

MY_DLLEXPORT int phx_SetBusy(phConfig_t *cfg, int busyFlag)
{
  return phSetBusy(busyFlag);
}

MY_DLLEXPORT int phx_AddAuthInfo(phConfig_t *cfg, const char *username, const char *userid, const char *passwd, const char *ha1, const char *realm)
{
  return phAddAuthInfo(username, userid, passwd, ha1, realm);
}


#if 0
MY_DLLEXPORT int phx_Register2(phConfig_t *cfg, const char *username, const char *server, int timeout)
{
  return phRegister2(username, server, timeout);
}
#endif


MY_DLLEXPORT int phx_SendDtmf(phConfig_t *cfg, int cid, int dtmfEvent, int mode)
{
  return phSendDtmf(cid, dtmfEvent, mode);
}

MY_DLLEXPORT int phx_PlaySoundFile(phConfig_t *cfg, const char *fileName , int loop)
{
  return phx_PlaySoundFile(cfg, fileName , loop);
}

MY_DLLEXPORT int phx_StopSoundFile(phConfig_t * cfg)
{
  return phStopSoundFile();
}

MY_DLLEXPORT int phx_Init(phConfig_t *cfg, phCallbacks_t *cbk, char * server, int asyncmode)
{
  return phInit(cbk, server, asyncmode);
}

MY_DLLEXPORT int phx_Poll()
{
  return phPoll();
}

MY_DLLEXPORT void phx_Terminate()
{
  phTerminate();
}

