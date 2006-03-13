#ifndef _PHAPI_X_H_
#define _PHAPI_X_H_

#include "phapi.h"


MY_DLLEXPORT int phx_GetVersion(phConfig_t *cfg);
MY_DLLEXPORT int phx_GetAllVersions(struct phversions *version);
MY_DLLEXPORT int phx_PlaceCall2(phConfig_t *cfg, const char *from, const char *uri, void *userdata, int rcid);
MY_DLLEXPORT int phx_PlaceCall3(phConfig_t *cfg, const char *from, const char *uri, void *userdata, int rcid, int wanted_streams);
MY_DLLEXPORT int phx_PlaceCall(phConfig_t *cfg, const char *from, const char *uri, void *userdata);
MY_DLLEXPORT int phx_SendMessage(phConfig_t *cfg, const char *from, const char *uri, const char *buff);
MY_DLLEXPORT int phx_Subscribe(phConfig_t *cfg, const char *from, const char *to, const int winfo);
MY_DLLEXPORT int phx_Publish(phConfig_t *cfg, const char *from, const char *to, const int winfo, const char * content_type, const char * content);
MY_DLLEXPORT int phx_Refresh(phConfig_t *cfg);
MY_DLLEXPORT int phx_SendOptions(phConfig_t *cfg, const char *from, const char *uri);
MY_DLLEXPORT int phx_AcceptCall2(phConfig_t *cfg, int cid, void *userData);
MY_DLLEXPORT int phx_AcceptCall3(phConfig_t *cfg, int cid, void *userData, int wanted_streams);
MY_DLLEXPORT int phx_RejectCall(phConfig_t *cfg, int cid, int reason);
MY_DLLEXPORT int phx_RingingCall(phConfig_t *cfg, int cid);
MY_DLLEXPORT int phx_CloseCall(phConfig_t *cfg, int cid);
MY_DLLEXPORT int phx_BlindTransferCall(phConfig_t *cfg, int cid, const char *uri);
MY_DLLEXPORT int phx_TransferCall(phConfig_t *cfg, int cid, int tcid);
MY_DLLEXPORT int phx_ResumeCall(phConfig_t *cfg, int cid);
MY_DLLEXPORT int phx_HoldCall(phConfig_t *cfg, int cid);
MY_DLLEXPORT int phx_SetFollowMe(phConfig_t *cfg, const char *uri);
MY_DLLEXPORT int phx_SetBusy(phConfig_t *cfg, int busyFlag);
MY_DLLEXPORT int phx_AddAuthInfo(phConfig_t *cfg, const char *username, const char *userid, const char *passwd, const char *ha1, const char *realm);
MY_DLLEXPORT int phx_Register2(phConfig_t *cfg, const char *username, const char *server, int timeout);
MY_DLLEXPORT int phx_SendDtmf(phConfig_t *cfg, int cid, int dtmfEvent, int mode);
MY_DLLEXPORT int phx_PlaySoundFile(phConfig_t *cfg, const char *fileName , int loop);
MY_DLLEXPORT int phx_StopSoundFile(phConfig_t * cfg);
MY_DLLEXPORT int phx_Init(phConfig_t *cfg, phCallbacks_t *cbk, char * server, int asyncmode);
MY_DLLEXPORT int phx_Poll();
MY_DLLEXPORT void phx_Terminate();

#endif
