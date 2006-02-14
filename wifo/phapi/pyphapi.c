/*
 * C/Python wrapper for phapi
 *
 * Copyright (C) 2005 Wengo SAS
 * Copyright (C) 2005 David Ferlier <david.ferlier@wengo.fr>
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
 *
 */

/*
 * @æuthor David Ferlier <david.ferlier@wengo.fr>
 * @author Mathieu Stute <mathieu.stute@wengo.fr>
 */

/*
 *  TODO:
 *
 *  - play/stop soundfile
 *  - rec/speaker set volume
 *  - getNatInfo
 *  - conference functions
 *
 */
 
#include "phglobal.h"
 
#ifdef OS_WINDOWS
#undef DEBUG
#undef _DEBUG

#include <windows.h>
#include <shlwapi.h>

static void (*userNotify)(char *buf, int size);
LONG unhandledExceptionFilter(struct _EXCEPTION_POINTERS * pExceptionInfo) {
    userNotify(NULL, NULL);
    return 0;
}

#endif

#include <Python.h>
#include <phapi.h>

static void pyphapi_callback_callProgress(int cid, 
                const phCallStateInfo_t *info);
static void pyphapi_callback_registerProgress(int cid, 
                int regStatus);
static void pyphapi_callback_transferProgress(int cid,
                const phTransferStateInfo_t *info);
static void pyphapi_callback_confProgress(int cfid,
                const phConfStateInfo_t *info);
static void pyphapi_callback_msgProgress(int mid,
                const phMsgStateInfo_t *info);
static void pyphapi_callback_onNotify(const char* event, 
                const char* from, const char* content);
static void pyphapi_callback_subscriptionProgress(int sid,
                const phSubscriptionStateInfo_t *info);
static void pyphapi_callback_errorNotify(enum phErrors error);
static void pyphapi_callback_frameDisplay(int cid, 
                phVideoFrameReceivedEvent_t *ev);

static phCallbacks_t pyphapi_callbacks = {
    pyphapi_callback_callProgress,
    pyphapi_callback_transferProgress,
    pyphapi_callback_confProgress,
    pyphapi_callback_registerProgress,
    pyphapi_callback_msgProgress,
    pyphapi_callback_onNotify,
    pyphapi_callback_subscriptionProgress,
    pyphapi_callback_frameDisplay,
    pyphapi_callback_errorNotify,
};

static PyObject *pyphapi_callProgress = NULL;
static PyObject *pyphapi_registerProgress =  NULL;
static PyObject *pyphapi_subscriptionProgress =  NULL;
static PyObject *pyphapi_confProgress =  NULL;
static PyObject *pyphapi_onNotify = NULL;
static PyObject *pyphapi_transferProgress = NULL;
static PyObject *pyphapi_msgProgress = NULL;
static PyObject *pyphapi_errorNotify = NULL;


/*
 * @brief Initializes the module
 *
 */
static PyObject * pyphapi(PyObject* self) {
    PyEval_InitThreads();
        
    return Py_BuildValue("s","");
}

/*
 * @brief Wraps phInit()
 *
 */
static PyObject * PyPhInit(PyObject * self, PyObject * params) {
    
    char * server;
    int asyncmode;
    int ret;
    phConfig_t *cfg;

    cfg = phGetConfig();

    strcpy(cfg->nattype, "fcone");
    strcpy(cfg->sipport, "5060");

    cfg->nodefaultline = 1;
    cfg->force_proxy = 1;
    cfg->nat_refresh_time = 30;
    cfg->use_tunnel = 0;

    if (PyArg_ParseTuple(params, "si", &server, &asyncmode)) {
        ret = phInit(&pyphapi_callbacks, server, asyncmode);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phAddAuthInfo()
 *
 */
static PyObject * PyPhAddAuthInfo(PyObject *self, PyObject *params) {
    const char *username;
    const char *userid;
    const char *passwd;
    const char *ha1;
    const char *realm;
    int pycode, ret;

    pycode = PyArg_ParseTuple(params, "sssss", &username,
            &userid, &passwd, &ha1, &realm);

    if (!pycode) {
        return Py_None;
    }

    ret = phAddAuthInfo(username, userid, passwd, ha1, realm);
    return Py_BuildValue("i", ret);
}

/*
 * @brief phAddVline
 *
 */
static PyObject * PyPhAddVline(PyObject *self, PyObject *params) {
    const char *username;
    const char *host;
    const char *proxy;
    int regTimeout;
    int pycode, ret;

    pycode = PyArg_ParseTuple(params, "sssi", &username,
            &host, &proxy, &regTimeout);

    if (!pycode) {
        return Py_None;
    }

    ret = phAddVline(username, host, proxy, regTimeout);
    return Py_BuildValue("i", ret);
}

/*
 * @brief phTunnelConfig
 *
 */
static PyObject * PyPhTunnelConfig(PyObject *self, PyObject *params) {
    const char *http_proxy;
    const char *httpt_server;
    const char *proxy_user;
    const char *proxy_passwd;
    const int http_proxy_port;
    const int httpt_server_port;
    int autoconf;
    int ret;

    if (PyArg_ParseTuple(params, "sisissi",
                &http_proxy, &http_proxy_port,
                &httpt_server, &httpt_server_port,
                &proxy_user, &proxy_passwd, &autoconf)) {

        ret = phTunnelConfig(http_proxy,
                             http_proxy_port,
                             httpt_server,
                             httpt_server_port,
                             proxy_user,
                             proxy_passwd,
                             autoconf);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief phAddVline2
 *
 */
static PyObject * PyPhAddVline2(PyObject *self, PyObject *params) {
    const char *displayname;
    const char *username;
    const char *host;
    const char *proxy;
    int regTimeout;
    int pycode, ret;
    phConfig_t *cfg;

    cfg = phGetConfig();

    pycode = PyArg_ParseTuple(params, "ssssi", &displayname, &username,
            &host, &proxy, &regTimeout);

    if (!pycode) {
        return Py_None;
    }

    strcpy(cfg->proxy, proxy);

    ret = phAddVline2(displayname, username, host, proxy, regTimeout);
    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phPoll()
 *
 */
static PyObject * PyPhPoll(PyObject *self, PyObject *params) {
    int ret;

    ret = phPoll();
    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phRefresh()
 *
 */
static PyObject * PyPhRefresh(PyObject *self, PyObject *params) {
    phRefresh();
    return Py_None;
}

/*
 * @brief Wraps phTerminate()
 *
 */
static PyObject * PyPhTerminate(PyObject *self, PyObject *params) {
    phTerminate();
    return Py_None;
}

/*
 * @brief Wraps phLinePlaceCall2()
 *
 */
static PyObject * PyPhLinePlaceCall2(PyObject *self, PyObject *params) {
    const char *uri;
    int rcid;
    int streams;
    void *userdata;
    int vlid;
    int ret = -12;

    if (PyArg_ParseTuple(params, "issii", &vlid, &uri, &userdata, &rcid, &streams)) {
        ret = phLinePlaceCall2(vlid, uri, userdata, rcid, streams);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phLineSendOptions
 *
 */
static PyObject * PyPhLineSendOptions(PyObject *self, PyObject *params) {
    int vlid;
    int ret;
    const char *to;

    if (PyArg_ParseTuple(params, "is", &vlid, &to)) {
        ret = phLineSendOptions(vlid, to);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phLineSendMessage()
 *
 */
static PyObject * PyPhLineSendMessage(PyObject *self, PyObject *params) {
    int vlid;
    int ret;
    const char *to;
    const char *buff;

    if (PyArg_ParseTuple(params, "iss", &vlid, &to, &buff)) {
        ret = phLineSendMessage(vlid, to, buff);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phLinePublish()
 *
 */
static PyObject * PyPhLinePublish(PyObject *self, PyObject *params) {
    int vlid;
    int ret;
    const int winfo;
    const char *to;
    const char *content_type;
    const char *content;

    if (PyArg_ParseTuple(params, "isiss", &vlid, &to, &winfo, &content_type, &content)) {
        ret = phLinePublish(vlid, to, winfo, content_type, content);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phLineSubscribe
 *
 */
static PyObject * PyPhLineSubscribe(PyObject *self, PyObject *params) {
    int vlid;
    int ret;
    const int winfo;
    const char *to;

    if (PyArg_ParseTuple(params, "isi", &vlid, &to, &winfo)) {
        ret = phLineSubscribe(vlid, to, winfo);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phLineSetFollowMe
 *
 */
static PyObject * PyPhLineSetFollowMe(PyObject *self, PyObject *params) {
    int vlid;
    int ret;
    const char *uri;

    if (PyArg_ParseTuple(params, "is", &vlid, &uri)) {
        ret = phLineSetFollowMe(vlid, uri);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phLineSetBusy
 *
 */
static PyObject * PyPhLineSetBusy(PyObject *self, PyObject *params) {
    int vlid;
    int ret;
    int busy_flag;

    if (PyArg_ParseTuple(params, "is", &vlid, &busy_flag)) {
        ret = phLineSetBusy(vlid, busy_flag);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phAcceptCall2()
 *
 */
static PyObject * PyPhAcceptCall2(PyObject *self, PyObject *params) {
    int cid;
    int ret;
    void *userdata;

    if (PyArg_ParseTuple(params, "iO&", &cid, &userdata)) {
        ret = phAcceptCall2(cid, userdata);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phAcceptCall3()
 *
 */
static PyObject * PyPhAcceptCall3(PyObject *self, PyObject *params) {
    int cid;
    int ret;
    void *userdata;
    int stream;

    if (PyArg_ParseTuple(params, "iO&i", &cid, &userdata, &stream)) {
        ret = phAcceptCall3(cid, userdata, stream);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phRejectCall()
 *
 */
static PyObject * PyPhRejectCall(PyObject *self, PyObject *params) {
    int cid;
    int reason;
    int ret;

    if (PyArg_ParseTuple(params, "ii", &cid, &reason)) {
        ret = phRejectCall(cid, reason);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phRingingCall()
 *
 */
static PyObject * PyPhRingingCall(PyObject *self, PyObject *params) {
    int cid;
    int ret;

    if (PyArg_ParseTuple(params, "i", &cid)) {
        ret = phRingingCall(cid);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phCloseCall()
 *
 */
static PyObject * PyPhCloseCall(PyObject *self, PyObject *params) {
    int cid;
    int ret;

    if (PyArg_ParseTuple(params, "i", &cid)) {
        ret = phCloseCall(cid);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phSendMessage()
 *
 */
static PyObject * PyPhSendMessage(PyObject *self, PyObject *params) {
    const char *from;
    const char *uri;
    const char *buff;
    int ret;

    if (PyArg_ParseTuple(params, "sss", &from, &uri, &buff)) {
        ret = phSendMessage(from, uri, buff);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phSubscribe()
 *
 */
static PyObject * PyPhSubscribe(PyObject *self, PyObject *params) {
    const char *from;
    const char *to;
    const int winfo;
    int ret;

    if (PyArg_ParseTuple(params, "ssi", &from, &to, &winfo)) {
        ret = phSubscribe(from, to, winfo);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phPublish()
 *
 */
static PyObject * PyPhPublish(PyObject *self, PyObject *params) {
    const char *from;
    const char *to;
    const char *content_type;
    const char *content;
    const int winfo;
    int ret;

    if (PyArg_ParseTuple(params, "ssiss", &from, &to, &winfo,
                    &content_type, &content)) {
        ret = phPublish(from, to, winfo, content_type, content);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phDelVline
 *
 */
static PyObject * PyPhDelVline(PyObject *self, PyObject *params) {
    int vlid;
    int ret;

    if (PyArg_ParseTuple(params, "i", &vlid)) {
        ret = phDelVline(vlid);
    }

    return Py_BuildValue("i", ret);
}


/*
 * @brief Wraps phTransferCall
 *
 */
static PyObject * PyPhTransferCall(PyObject *self, PyObject *params) {
    int cid;
    int targetCid;
    int ret;

    if (PyArg_ParseTuple(params, "ii", &cid, &targetCid)) {
        ret = phTransferCall(cid, targetCid);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phBlindTransferCall
 *
 */
static PyObject * PyPhBlindTransferCall(PyObject *self, PyObject *params) {
    int cid;
    char * uri;
    int ret;

    if (PyArg_ParseTuple(params, "is", &cid, &uri)) {
        ret = phBlindTransferCall(cid, uri);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phResumeCall
 *
 */
static PyObject * PyPhResumeCall(PyObject *self, PyObject *params) {
    int cid;
    int ret;

    if (PyArg_ParseTuple(params, "i", &cid)) {
        ret = phResumeCall(cid);
    }

    return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phHoldCall
 *
 */
static PyObject * PyPhHoldCall(PyObject *self, PyObject *params) {
	int cid;
	int ret;
	
	if (PyArg_ParseTuple(params, "i", &cid)) {
		ret = phHoldCall(cid);
	}
	
	return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phSendDtmf
 *
 */
static PyObject * PyPhSendDtmf(PyObject *self, PyObject *params) {
	int cid;
	int dtmfChar;
	int mode;
	int ret;
	
	if (PyArg_ParseTuple(params, "iii", &cid, &dtmfChar, &mode)) {
		ret = phSendDtmf(cid, dtmfChar, mode);
	}
	
	return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phChangeAudioDevices
 *
 */
static PyObject * PyPhChangeAudioDevices(PyObject *self, PyObject *params) {
	char * devstr;
	int ret;
	
	if (PyArg_ParseTuple(params, "s", &devstr)) {
		ret = phChangeAudioDevices(devstr);
	}
	
	return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phCrash
 *
 */
static PyObject * PyPhCrash(PyObject *self, PyObject *params) {
	int ret;
	ret = phCrash();
	return Py_BuildValue("i", ret);
}

/*
 * @brief Wraps phSetDebugLevel
 *
 */
static PyObject * PyPhSetDebugLevel(PyObject *self, PyObject *params) {
    int level;

    if (PyArg_ParseTuple(params, "i", &level)) {
        phSetDebugLevel(level);
    }
    
    return Py_None;
}

/*
 * @brief Wraps" phGetNatInfo
 *
 */
static PyObject * PyPhGetNatInfo(PyObject *self, PyObject *params) {
    char * arg;
    char nattype[8];
    char firewallip[16];
    int ret;
    
    ret = phGetNatInfo(nattype, 8, firewallip, 16);
    if( !ret ) {
    
        if (PyArg_ParseTuple(params, "s", &arg)) {
            if(strcmp(arg, "type" ) == 0) {
                return Py_BuildValue("s", nattype);
            }
            if(strcmp(arg, "firewall" ) == 0) {
                return Py_BuildValue("s", firewallip);
            }
        }
    }
    return Py_None;
}

/*
 * This function initialize the python callbacks functions that will
 * be called when a corresponding C callback from the phCallbacks
 * structure is called from the phapi stack
 *
 * @params  args The callbacks as a PyObject tuple, with the following order :
 *
 *      callProgress
 *      regProgress
 *
 */
static PyObject * PyPhSetCallbacks(PyObject *self, PyObject *args) {
    PyObject *result = NULL;
    PyObject *callback_call_progress;
    PyObject *callback_reg_progress;
    PyObject *callback_error_notify;
    PyObject *callback_transfer_progress;
    PyObject *callback_conf_progress;
    PyObject *callback_msg_progress;
    PyObject *callback_on_notify;
    PyObject *callback_subscription_progress;
    
    if (PyArg_ParseTuple(args, "OOOOOOOO",
                &callback_call_progress,
                &callback_reg_progress,
                &callback_error_notify,
                &callback_transfer_progress,
                &callback_conf_progress,
                &callback_msg_progress,
                &callback_on_notify,
                &callback_subscription_progress)) {

        if (!PyCallable_Check(callback_reg_progress)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }

        Py_XINCREF(callback_reg_progress);
        Py_XDECREF(pyphapi_registerProgress);

        Py_XINCREF(callback_call_progress);
        Py_XDECREF(pyphapi_callProgress);

        Py_XINCREF(callback_error_notify);
        Py_XDECREF(pyphapi_errorNotify);

        Py_XINCREF(callback_transfer_progress);
        Py_XDECREF(pyphapi_transferProgress);
        
        Py_XINCREF(callback_conf_progress);
        Py_XDECREF(pyphapi_confProgress);

        Py_XINCREF(callback_msg_progress);
        Py_XDECREF(pyphapi_msgProgress);

        Py_XINCREF(callback_on_notify);
        Py_XDECREF(pyphapi_onNotify);

        Py_XINCREF(callback_subscription_progress);
        Py_XDECREF(pyphapi_subscriptionProgress);
        
        Py_INCREF(Py_None);
        pyphapi_registerProgress = callback_reg_progress;
        pyphapi_callProgress = callback_call_progress;
        pyphapi_errorNotify = callback_error_notify;
        pyphapi_transferProgress = callback_transfer_progress;
        pyphapi_confProgress = callback_conf_progress;
        pyphapi_msgProgress = callback_msg_progress;
        pyphapi_onNotify = callback_on_notify;
        pyphapi_subscriptionProgress = callback_subscription_progress;
        result = Py_None;
    }

    return result;
}

/*
 * @brief Lock the Python GIL (Global Interpreter Lock) and call a function
 *
 * This function first locks the GIL, and then calls a python callback
 * with some arguments. Once called it decrements ref count of the
 * arguments variable, and then releases the GIL.
 *
 * @param   callback    The function to call
 * @param   args        The arguments to pass to the function
 *
 */
void pyphapi_lock_and_call(PyObject *callback, PyObject *args) {
    PyGILState_STATE gstate;

    if (callback) {
        gstate = PyGILState_Ensure();
        PyEval_CallObject(callback, args);
        Py_DECREF(args);
        PyGILState_Release(gstate);
    }
}

/*
 * callProgress() phapi's C callback
 *
 */
void pyphapi_callback_callProgress(int cid, const phCallStateInfo_t *info) {
    PyObject *call_info;
    
    call_info = Py_BuildValue("(ii)", cid, info->event);
    pyphapi_lock_and_call(pyphapi_callProgress, call_info);
}

/*
 * transferProgress() phapi's C callback
 *
 */
void pyphapi_callback_transferProgress(int cid,
                const phTransferStateInfo_t *info) {
    PyObject *transfer_info;

    transfer_info = Py_BuildValue("(ii)", cid, 0);
    pyphapi_lock_and_call(pyphapi_transferProgress, transfer_info);
}

/*
 * confProgress() phapi's C callback
 *
 */
void pyphapi_callback_confProgress(int cfid,
                const phConfStateInfo_t *info) {
    PyObject *conf_info;

    conf_info = Py_BuildValue("(iiii)", cfid, info->confEvent, info->memberCid, info->errorCode);
    pyphapi_lock_and_call(pyphapi_confProgress, conf_info);
}

/*
 * msgProgress() phapi's C callback
 *
 */
void pyphapi_callback_msgProgress(int mid,const phMsgStateInfo_t *info) {
    PyObject *msg_info;
            
    msg_info = Py_BuildValue("(iiisssss)",
                    mid,
                    info->event,
                    info->status,
                    info->from,
                    info->to,
                    info->ctype,
                    info->subtype,
                    info->content);

    pyphapi_lock_and_call(pyphapi_msgProgress, msg_info);
}

/*
 * onNotify() phapi's C callback
 *
 */
void pyphapi_callback_onNotify(const char* event, const char* from,
                const char* content) {
    PyObject *notify_info;

    notify_info = Py_BuildValue("(sss)", event, from, content);
    pyphapi_lock_and_call(pyphapi_onNotify, notify_info);
}

/*
 * subscriptionProgress() phapi's C callback
 *
 */
void pyphapi_callback_subscriptionProgress(int sid,
                const phSubscriptionStateInfo_t *info) {
    PyObject *sub_info;

    sub_info = Py_BuildValue("(iiiss)", sid, info->event, info->status, info->from, info->to);
    pyphapi_lock_and_call(pyphapi_subscriptionProgress, sub_info);
}

/*
 * errorNotify() phapi's C callback
 *
 */
void pyphapi_callback_errorNotify(enum phErrors error) {
    PyObject *err_info;

    err_info = Py_BuildValue("(i)", (int) error);
    pyphapi_lock_and_call(pyphapi_errorNotify, err_info);
}

/*
 * registerProgress() phapi's C callback
 *
 */
static void pyphapi_callback_registerProgress(int cid, int regStatus) {
    PyObject *reg_info;

    reg_info = Py_BuildValue("(ii)", cid, regStatus);
    pyphapi_lock_and_call(pyphapi_registerProgress, reg_info);
}

/*
 * frameDisplay() phapi's C callback
 *
 */
static void pyphapi_callback_frameDisplay(int cid, phVideoFrameReceivedEvent_t *ev) {
}

/*
 * @brief wrap access to phcfg
 *
 */
static PyObject * PyPhCfgSetS(PyObject *self, PyObject *params) {
    char * field;
    int value_int;
    char * value_string;
    phConfig_t *cfg;
    
    cfg = phGetConfig();
    
    if (PyArg_ParseTuple(params, "ss", &field, &value_string)) {
        if( strcmp(field, "local_rtp_port" )==0) {strncpy(cfg->local_rtp_port, value_string,16);}
        if( strcmp(field, "local_audio_rtcp_port" )==0) {strncpy(cfg->local_audio_rtcp_port, value_string,16);}
        if( strcmp(field, "local_video_rtp_port" )==0) {strncpy(cfg->local_video_rtp_port, value_string,16);}
        if( strcmp(field, "local_video_rtcp_port" )==0) {strncpy(cfg->local_video_rtcp_port, value_string,16);}
        if( strcmp(field, "sipport" )==0) {strncpy(cfg->sipport, value_string,16);}
        if( strcmp(field, "nattype" )==0) {strncpy(cfg->nattype, value_string,16);}
        if( strcmp(field, "audio_codecs" )==0) {strncpy(cfg->audio_codecs, value_string,128);}
        if( strcmp(field, "video_codecs" )==0) {strncpy(cfg->video_codecs, value_string,128);}
        if( strcmp(field, "audio_dev" )==0) {strncpy(cfg->audio_dev, value_string,64);}
        if( strcmp(field, "stunserver" )==0) {strncpy(cfg->stunserver, value_string,128);}
        if( strcmp(field, "httpt_server" )==0) {strncpy(cfg->httpt_server, value_string,128);}
        if( strcmp(field, "http_proxy" )==0) {strncpy(cfg->http_proxy, value_string,128);}
        if( strcmp(field, "http_proxy_user" )==0) {strncpy(cfg->http_proxy_user, value_string,128);}
        if( strcmp(field, "http_proxy_passwd" )==0) {strncpy(cfg->http_proxy_passwd, value_string,128);}
        if( strcmp(field, "plugin_path" )==0) {strncpy(cfg->plugin_path, value_string,256);}
    }
    return Py_None;
}


/*
 * @brief wrap access to phcfg
 *
 */
static PyObject * PyPhCfgSetI(PyObject *self, PyObject *params) {
    char * field;
    int value_int;
    char * value_string;
    phConfig_t *cfg;
    
    cfg = phGetConfig();
    
    if (PyArg_ParseTuple(params, "si", &field, &value_int)) {
        if( strcmp(field, "asyncmode" ) == 0) {cfg->asyncmode=value_int;}
        if( strcmp(field, "nomedia" ) == 0) {cfg->nomedia=value_int;}
        if( strcmp(field, "noaec" ) == 0) {cfg->noaec=value_int;}
        if( strcmp(field, "vad" ) == 0) {cfg->vad=value_int;}
        if( strcmp(field, "cng" ) == 0) {cfg->cng=value_int;}
        if( strcmp(field, "hdxmode" ) == 0) {cfg->hdxmode=value_int;}
        if( strcmp(field, "nat_refresh_time" ) == 0) {cfg->nat_refresh_time=value_int;}
        if( strcmp(field, "jitterdepth" ) == 0) {cfg->jitterdepth=value_int;}
        if( strcmp(field, "autoredir" ) == 0) {cfg->autoredir=value_int;}
        if( strcmp(field, "use_tunnel" ) == 0) {cfg->use_tunnel=value_int;}
        if( strcmp(field, "httpt_server_port" ) == 0) {cfg->httpt_server_port=value_int;}
        if( strcmp(field, "http_proxy_port" ) == 0) {cfg->http_proxy_port=value_int;}
    }
    return Py_None;
}


/*
 * Convenient define to declare a function with variable arguments in
 * the module's API table
 *
 */
#define PY_PHAPI_FUNCTION_DECL(n,f) \
    { n, (PyCFunction) f, METH_VARARGS, "" }

#define PY_PHAPI_FUNCTION_DECL_NULL \
    { NULL, NULL, 0, NULL }

/*
 * Declaration of the module API table
 *
 */
static PyMethodDef pyphapi_funcs[] = {
    { "pyphapi",(PyCFunction) pyphapi, METH_NOARGS,  "Python Module of phApi"},
    PY_PHAPI_FUNCTION_DECL("phInit",                PyPhInit),
    PY_PHAPI_FUNCTION_DECL("phAddAuthInfo",         PyPhAddAuthInfo),
    PY_PHAPI_FUNCTION_DECL("phAddVline",            PyPhAddVline),
    PY_PHAPI_FUNCTION_DECL("phAddVline2",           PyPhAddVline2),
    PY_PHAPI_FUNCTION_DECL("phSetCallbacks",        PyPhSetCallbacks),
    PY_PHAPI_FUNCTION_DECL("phPoll",                PyPhPoll),
    PY_PHAPI_FUNCTION_DECL("phRefresh",             PyPhRefresh),
    PY_PHAPI_FUNCTION_DECL("phTerminate",           PyPhTerminate),
    PY_PHAPI_FUNCTION_DECL("phTunnelConfig",        PyPhTunnelConfig),
    PY_PHAPI_FUNCTION_DECL("phLinePlaceCall2",      PyPhLinePlaceCall2),
    PY_PHAPI_FUNCTION_DECL("phLineSendOptions",     PyPhLineSendOptions),
    PY_PHAPI_FUNCTION_DECL("phLineSendMessage",     PyPhLineSendMessage),
    PY_PHAPI_FUNCTION_DECL("phLineSubscribe",       PyPhLineSubscribe),
    PY_PHAPI_FUNCTION_DECL("phLineSetFollowMe",     PyPhLineSetFollowMe),
    PY_PHAPI_FUNCTION_DECL("phLineSetBusy",         PyPhLineSetBusy),
    PY_PHAPI_FUNCTION_DECL("phLinePublish",         PyPhLinePublish),
    PY_PHAPI_FUNCTION_DECL("phAcceptCall2",         PyPhAcceptCall2),
    PY_PHAPI_FUNCTION_DECL("phAcceptCall3",         PyPhAcceptCall3),
    PY_PHAPI_FUNCTION_DECL("phRejectCall",          PyPhRejectCall),
    PY_PHAPI_FUNCTION_DECL("phCloseCall",           PyPhCloseCall),
    PY_PHAPI_FUNCTION_DECL("phRingingCall",         PyPhRingingCall),
    PY_PHAPI_FUNCTION_DECL("phSendMessage",         PyPhSendMessage),
    PY_PHAPI_FUNCTION_DECL("phSubscribe",           PyPhSubscribe),
    PY_PHAPI_FUNCTION_DECL("phPublish",             PyPhPublish),
    PY_PHAPI_FUNCTION_DECL("phDelVline",            PyPhDelVline),
    PY_PHAPI_FUNCTION_DECL("phTransferCall",        PyPhTransferCall),
    PY_PHAPI_FUNCTION_DECL("phBlindTransferCall",   PyPhBlindTransferCall),
    PY_PHAPI_FUNCTION_DECL("phResumeCall",          PyPhResumeCall),
    PY_PHAPI_FUNCTION_DECL("phHoldCall",            PyPhHoldCall),
    PY_PHAPI_FUNCTION_DECL("phCfgSetS",             PyPhCfgSetS),
    PY_PHAPI_FUNCTION_DECL("phCfgSetI",             PyPhCfgSetI),
    PY_PHAPI_FUNCTION_DECL("phSendDtmf",            PyPhSendDtmf),
    PY_PHAPI_FUNCTION_DECL("phChangeAudioDevices",  PyPhChangeAudioDevices),
    PY_PHAPI_FUNCTION_DECL("phCrash",               PyPhCrash),
    PY_PHAPI_FUNCTION_DECL("phSetDebugLevel",       PyPhSetDebugLevel),
    PY_PHAPI_FUNCTION_DECL("phGetNatInfo",          PyPhGetNatInfo),
    
    PY_PHAPI_FUNCTION_DECL_NULL,
};

/*
 * Function called on import
 *
 */
PyMODINIT_FUNC initpyphapi(void) {
#ifdef WIN32
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)unhandledExceptionFilter);
#endif
    Py_InitModule3("pyphapi", pyphapi_funcs, "");
}
