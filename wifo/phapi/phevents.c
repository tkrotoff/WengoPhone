#include <assert.h>
#include <phapi-util/linkedlist.h>
#include "phevents.h"
#include "phcall.h"
#include "owpl_plugin.h"
#include <eXosip/eXosip.h>


typedef struct OWPL_EventSubscriber_t
{
	OWPL_EVENT_CALLBACK_PROC cbFunc;
	void *cbData;
} OWPL_EventSubscriber;


linkedlist_t * gEventSubscriber = 0;

extern OWPL_LOG_REPORT_PROC owplLogReportProc; // defined in phapi.c

//=================================
//  Add a new nomal listener 
//=================================
MY_DLLEXPORT OWPL_RESULT
owplEventListenerAdd(OWPL_EVENT_CALLBACK_PROC pCallbackProc,
                                             void *pUserData)
{
	OWPL_EventSubscriber * newListener;
	if ( ! gEventSubscriber)
	{
		gEventSubscriber = create_linkedlist();
	}

	newListener = malloc(sizeof(OWPL_EventSubscriber));
	newListener->cbData = pUserData;
	newListener->cbFunc = pCallbackProc;
	
	linkedlist_add(gEventSubscriber, newListener, 0);

	return OWPL_RESULT_SUCCESS;
}

//=================================
//  Remove a listener
//=================================

MY_DLLEXPORT OWPL_RESULT
owplEventListenerRemove(OWPL_EVENT_CALLBACK_PROC pCallbackProc)
{
	OWPL_EventSubscriber * subscriber;
	linkedlist_move_first(gEventSubscriber);
	while (1)
	{
		subscriber = linkedlist_get(gEventSubscriber);
		if (subscriber && subscriber->cbFunc == pCallbackProc)
		{
			linkedlist_remove_element(gEventSubscriber, subscriber, 0);
			return OWPL_RESULT_SUCCESS;
		}
		if ( ! linkedlist_move_next(gEventSubscriber)) {
			break;
		}
	}
	return OWPL_RESULT_FAILURE;
}

/***************************  INTERNAL USAGE FUNCTIONS ****************************/

/**
 * This function fires an event to all subscribed listeners
 *
 */

OWPL_RESULT
owplFireEvent(OWPL_EVENT_CATEGORY Category, 
                                            void* pInfo)
{
	OWPL_EventSubscriber * subscriber;

	if (!gEventSubscriber) {
		return OWPL_RESULT_FAILURE;	
	}
	linkedlist_move_first(gEventSubscriber);
	while (1)
	{
		subscriber = linkedlist_get(gEventSubscriber);
		if (subscriber && subscriber->cbFunc)
		{
			subscriber->cbFunc(Category, pInfo, subscriber->cbData);
		}
		if (! linkedlist_move_next(gEventSubscriber)) {
			break;
		}
	}
	return OWPL_RESULT_SUCCESS;
}

/**
 * This function creates a call event and send it to all subscribers
 *
 */
OWPL_RESULT
owplFireCallEvent(OWPL_CALL hCall, 
					OWPL_CALLSTATE_EVENT event, 
					OWPL_CALLSTATE_CAUSE cause,
					const char * szRemoteIdentity,
					OWPL_CALL hAssociatedCall)
{
	phcall_t *call;

	OWPL_CALLSTATE_INFO csInfo;
	memset(&csInfo, 0, sizeof(OWPL_CALLSTATE_INFO));
	csInfo.nSize = sizeof(OWPL_CALLSTATE_INFO);
	csInfo.hCall = hCall;
	call = ph_locate_call_by_cid(hCall);
	if (call) {
		csInfo.hLine = call->vlid;
	}
	csInfo.event = event;
	csInfo.cause = cause;
	csInfo.szRemoteIdentity = szRemoteIdentity;
	csInfo.hAssociatedCall = hAssociatedCall;
	return owplFireEvent(EVENT_CATEGORY_CALLSTATE, &csInfo);
}


/**
 * This function also creates a call event (with more advanced params) 
 * and send it to all subscribers
 *
 */
OWPL_RESULT
owplFireCallEvent2(OWPL_CALL hCall, 
					OWPL_CALLSTATE_EVENT event, 
					OWPL_CALLSTATE_CAUSE cause,
					const char * szRemoteIdentity,
					int nData,
					const void *pData,
					OWPL_CALL hAssociatedCall)
{
	phcall_t *call;

	OWPL_CALLSTATE_INFO csInfo;
	memset(&csInfo, 0, sizeof(OWPL_CALLSTATE_INFO));
	csInfo.nSize = sizeof(OWPL_CALLSTATE_INFO);
	csInfo.hCall = hCall;
	call = ph_locate_call_by_cid(hCall);
	if (call) {
		csInfo.hLine = call->vlid;
	}
	csInfo.event = event;
	csInfo.cause = cause;
	csInfo.szRemoteIdentity = szRemoteIdentity;
	csInfo.hAssociatedCall = hAssociatedCall;
	csInfo.nData = nData;
	csInfo.pData = pData;
	return owplFireEvent(EVENT_CATEGORY_CALLSTATE, &csInfo);
}
						
/**
 * This function creates a line event and send it to all subscribers
 *
 */
OWPL_RESULT
owplFireLineEvent(OWPL_LINE hLine,
						OWPL_LINESTATE_EVENT event,
						OWPL_LINESTATE_CAUSE cause,
						const char *szBodyBytes)        /**< msg body to extract reg body */
{
	OWPL_LINESTATE_INFO lsInfo;
	memset(&lsInfo, 0, sizeof(OWPL_LINESTATE_INFO));
	lsInfo.nSize = sizeof(OWPL_LINESTATE_INFO);
	lsInfo.event = event;
	lsInfo.cause = cause;
	lsInfo.hLine = hLine;
	lsInfo.szBodyBytes = szBodyBytes;

	return owplFireEvent(EVENT_CATEGORY_LINESTATE, &lsInfo);;
}


/**
 * This function creates an error event and send it to all subscribers
 *
 */
OWPL_RESULT
owplFireErrorEvent(OWPL_ERROR_EVENT event,
						void * Data)
{
	OWPL_ERROR_INFO eInfo;
	memset(&eInfo, 0, sizeof(eInfo));
	eInfo.event = event;
	eInfo.Data = Data;
	return owplFireEvent(EVENT_CATEGORY_ERROR, &eInfo);
}

OWPL_RESULT
owplFireSubscriptionEvent(OWPL_SUB hSub,
						  OWPL_SUBSCRIPTION_STATE state,
						  OWPL_SUBSCRIPTION_CAUSE cause,
						  const char* szRemoteIdentity)
{
	OWPL_SUBSTATUS_INFO sInfo;
	memset(&sInfo, 0, sizeof(OWPL_SUBSTATUS_INFO));
	sInfo.nSize = sizeof(OWPL_SUBSTATUS_INFO);
	sInfo.hSub = hSub;
	sInfo.state = state;
	sInfo.cause = cause;
	sInfo.szRemoteIdentity = szRemoteIdentity;
	return owplFireEvent(EVENT_CATEGORY_SUB_STATUS, &sInfo);
}

OWPL_RESULT
owplFireNotificationEvent(OWPL_NOTIFICATION_EVENT event,
						  const char* szXmlContent,
						  const char* szRemoteIdentity)
{
	OWPL_NOTIFICATION_INFO nInfo;
	memset(&nInfo, 0, sizeof(OWPL_NOTIFICATION_INFO));
	nInfo.nSize = sizeof(OWPL_NOTIFICATION_INFO);
	nInfo.event = event;
	nInfo.szXmlContent = szXmlContent;
	nInfo.szRemoteIdentity = szRemoteIdentity;
	return owplFireEvent(EVENT_CATEGORY_NOTIFY, &nInfo);
}

OWPL_RESULT
owplFireMessageEvent(OWPL_MESSAGE_EVENT event,
					 OWPL_MESSAGE_CAUSE cause,
					 const int messageId,
					 const char * szContent,
					 const char * szLocalIdentity,
					 const char * szRemoteIdentity,
					 const char * szContentType,
					 const char * szSubContentType)
{
	OWPL_MESSAGE_INFO mInfo;
	memset(&mInfo, 0, sizeof(OWPL_MESSAGE_INFO));
	mInfo.nSize = sizeof(OWPL_MESSAGE_INFO);
	mInfo.event = event;
	mInfo.cause = cause;
	mInfo.messageId = messageId;
	mInfo.szContent = szContent;
	mInfo.szLocalIdentity = szLocalIdentity;
	mInfo.szRemoteIdentity = szRemoteIdentity;
	mInfo.szContentType = szContentType;
	mInfo.szSubContentType = szSubContentType;
	return owplFireEvent(EVENT_CATEGORY_MESSAGE, &mInfo);
}

/************************************************/
/*     PLUGIN handling routines                 */
/************************************************/


/**
 * This function fires an event to all subscribed plugins
 *
 */

OWPL_RESULT
owplFireEvent2Plugin(OWPL_PLUGIN *plugin, OWPL_EVENT_CATEGORY Category, 
                                            void* pInfo)
{
	if (plugin && 
			plugin->exports && 
			plugin->exports->ct_info &&
			plugin->exports->ct_info->EventHandlerProc)		
	{
			plugin->exports->ct_info->EventHandlerProc(Category, pInfo, 0);
	}	
	return OWPL_RESULT_SUCCESS;
}


/**
 * This function creates a call event 
 * and send it to the specified plugin
 *
 */
OWPL_RESULT
owplFireCallEvent2Plugin(OWPL_PLUGIN *plugin,
					OWPL_CALL hCall, 
					OWPL_CALLSTATE_EVENT event, 
					OWPL_CALLSTATE_CAUSE cause,
					const char * szRemoteIdentity,
					int nData,
					const void *pData,
					OWPL_CALL hAssociatedCall)
{
	phcall_t *call;

	OWPL_CALLSTATE_INFO csInfo;
	memset(&csInfo, 0, sizeof(OWPL_CALLSTATE_INFO));
	csInfo.nSize = sizeof(OWPL_CALLSTATE_INFO);
	csInfo.hCall = hCall;
	call = ph_locate_call_by_cid(hCall);
	if (call) {
		csInfo.hLine = call->vlid;
	}
	csInfo.event = event;
	csInfo.cause = cause;
	csInfo.szRemoteIdentity = szRemoteIdentity;
	csInfo.hAssociatedCall = hAssociatedCall;
	csInfo.nData = nData;
	csInfo.pData = pData;
	return owplFireEvent2Plugin(plugin, EVENT_CATEGORY_CALLSTATE, &csInfo);
}

/**
 * Report a log message to the high level application
 *
 * @param szLogMsg The log message to report
 */
OWPL_RESULT
owplReportLogMessage(const char* szLogMsg)
{
	if (owplLogReportProc) {
		owplLogReportProc(szLogMsg);
	}
}


/*  eXosip dependent routines */

void owplFireExosipCallEvent(eXosip_event_t *je)
{
	OWPL_PLUGIN *owplPlugin;
	char buf[101];

	// Find the call or create it if je->type is EXOSIP_CALL_NEW
	phcall_t * call = ph_locate_call(je, je->type == EXOSIP_CALL_NEW ? 1 : 0);

	if ( call && (call->owplPlugin == 0))
	{
		if (je->i_ctt && je->i_ctt->type)
		{
			strncpy(buf, je->i_ctt->type, sizeof(buf)-1);
			if (je->i_ctt->subtype) {
				strncat(buf, "/", sizeof(buf) - strlen(buf) -1);
				strncat(buf,je->i_ctt->subtype, sizeof(buf) - strlen(buf) - 1);
			}
			owplPlugin = owplGetPlugin4ContentType(buf);
			if (owplPlugin) {
				call->owplPlugin = owplPlugin;
			}
		}
		else {
			return;
		}
	}

	switch(je->type){
			case EXOSIP_CALL_NEW:
				owplFireCallEvent2Plugin(call->owplPlugin,call->cid, 
													CALLSTATE_OFFERING, 
													CALLSTATE_OFFERING_ACTIVE, 
													je->remote_contact, 
													0, 
													je->msg_body, 
													0);
				break;
			
			case EXOSIP_CALL_ANSWERED:
				owplFireCallEvent2Plugin(call->owplPlugin,call->cid, 
													CALLSTATE_CONNECTED, 
													CALLSTATE_CONNECTED_ACTIVE, 
													je->remote_contact, 
													0, 
													je->msg_body,
													0);
				break;

			case EXOSIP_CALL_PROCEEDING:
				// Trying SIP message is received from other peer. What to do with it???
				break;

			case EXOSIP_CALL_RINGING:
				owplFireCallEvent2Plugin(call->owplPlugin,call->cid, 
													CALLSTATE_REMOTE_ALERTING, 
													CALLSTATE_REMOTE_ALERTING_NORMAL,
													je->remote_contact, 
													0, 
													je->msg_body,
													0);
				break;

			case EXOSIP_CALL_REDIRECTED:
				owplFireCallEvent2Plugin(call->owplPlugin,call->cid, 
													CALLSTATE_REDIRECTED, 
													CALLSTATE_REDIRECTED_NORMAL,
													je->remote_contact, 
													0, 
													je->msg_body,
													0);
				break;

			case EXOSIP_CALL_REPLACES:
				// What is it???
				break;

			case EXOSIP_CALL_REQUESTFAILURE:
				owplFireCallEvent2Plugin(call->owplPlugin,call->cid, 
													CALLSTATE_DISCONNECTED, 
													CALLSTATE_DISCONNECTED_NETWORK,
													je->remote_contact, 
													0, 
													je->msg_body,
													0);
				break;

			case EXOSIP_CALL_SERVERFAILURE:
				owplFireCallEvent2Plugin(call->owplPlugin,call->cid, 
													CALLSTATE_DISCONNECTED, 
													CALLSTATE_DISCONNECTED_NETWORK,
													je->remote_contact, 
													0, 
													je->msg_body,
													0);
				break;

			case EXOSIP_CALL_GLOBALFAILURE:
				owplFireCallEvent2Plugin(call->owplPlugin,call->cid, 
													CALLSTATE_DISCONNECTED, 
													CALLSTATE_DISCONNECTED_NETWORK,
													je->remote_contact, 
													0, 
													je->msg_body,
													0);
				break;

			case EXOSIP_CALL_NOANSWER:
				owplFireCallEvent2Plugin(call->owplPlugin,call->cid, 
													CALLSTATE_DISCONNECTED, 
													CALLSTATE_DISCONNECTED_NO_RESPONSE,
													je->remote_contact, 
													0, 
													je->msg_body,
													0);
				break;

			case EXOSIP_CALL_CLOSED:
				owplFireCallEvent2Plugin(call->owplPlugin,call->cid, 
													CALLSTATE_DISCONNECTED, 
													CALLSTATE_DISCONNECTED_NORMAL,
													je->remote_contact, 
													0, 
													je->msg_body,
													0);
				break;

			case EXOSIP_CALL_HOLD:
				owplFireCallEvent2Plugin(call->owplPlugin,call->cid, 
													CALLSTATE_HOLD, 
													CALLSTATE_HOLD_STARTED,
													je->remote_contact, 
													0, 
													je->msg_body,
													0);
				break;

			case EXOSIP_CALL_OFFHOLD:
				owplFireCallEvent2Plugin(call->owplPlugin,call->cid, 
													CALLSTATE_HOLD, 
													CALLSTATE_HOLD_RESUMED,
													je->remote_contact, 
													0, 
													je->msg_body,
													0);
				break;			

			case EXOSIP_CALL_REFERED:
				// ???
				break;

			case EXOSIP_CALL_REFER_STATUS:
				///???
				break;

			case EXOSIP_CALL_REFER_FAILURE:
				///???
				break;			

			default:
				assert(0);
				break;
		}

}
