/*
 * phapi   phone api
 *
 * Copyright (C) 2004        Vadim Lebedev <vadim@mbdsys.com>
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

#include "phplugin.h"
#include "phapi.h"
#include <eXosip/eXosip.h>

// ----- PRIVATE FUNCTIONS DECLARATION -----
static phplugin_t * sdp_get_plugin();
static void sdp_on_EXOSIP_CALL_NEW(eXosip_event_t * event);
static void sdp_on_EXOSIP_CALL_ANSWERED(eXosip_event_t * event);
static void sdp_on_EXOSIP_CALL_PROCEEDING(eXosip_event_t * event);
static void sdp_on_EXOSIP_CALL_RINGING(eXosip_event_t * event);
static void sdp_on_EXOSIP_CALL_REDIRECTED(eXosip_event_t * event);
static void sdp_on_EXOSIP_CALL_REPLACES(eXosip_event_t * event);
static void sdp_on_EXOSIP_CALL_REQUESTFAILURE(eXosip_event_t * event);
static void sdp_on_EXOSIP_CALL_SERVERFAILURE(eXosip_event_t * event);
static void sdp_on_EXOSIP_CALL_GLOBALFAILURE(eXosip_event_t * event);
static void sdp_on_EXOSIP_CALL_NOANSWER(eXosip_event_t * event);
static void sdp_on_EXOSIP_CALL_CLOSED(eXosip_event_t * event);
static void sdp_on_EXOSIP_CALL_HOLD(eXosip_event_t * event);
static void sdp_on_EXOSIP_CALL_OFFHOLD(eXosip_event_t * event);
static void sdp_on_EXOSIP_CALL_REFERED(eXosip_event_t * event);
static void sdp_on_EXOSIP_CALL_REFER_STATUS(eXosip_event_t * event);
static void sdp_on_EXOSIP_CALL_REFER_FAILURE(eXosip_event_t * event);
// -----

// ----- PLUGIN SERVICES -----
MY_DLLEXPORT unsigned int sdp_register_plugin(){
	return phplugin_register(sdp_get_plugin());
}
// -----

static phplugin_t * sdp_get_plugin(){
	phplugin_t * plugin = (phplugin_t *)malloc(sizeof(phplugin_t));

	strncpy(plugin->content_type.type, "application", sizeof(plugin->content_type.type));
	strncpy(plugin->content_type.subtype, "sdp", sizeof(plugin->content_type.subtype));

	plugin->plg_callbacks.on_EXOSIP_CALL_ANSWERED				=	&sdp_on_EXOSIP_CALL_ANSWERED;
	plugin->plg_callbacks.on_EXOSIP_CALL_CLOSED					=	&sdp_on_EXOSIP_CALL_CLOSED;
	plugin->plg_callbacks.on_EXOSIP_CALL_GLOBALFAILURE			=	&sdp_on_EXOSIP_CALL_GLOBALFAILURE;
	plugin->plg_callbacks.on_EXOSIP_CALL_HOLD					=	&sdp_on_EXOSIP_CALL_HOLD;
	plugin->plg_callbacks.on_EXOSIP_CALL_NEW					=	&sdp_on_EXOSIP_CALL_NEW;
	plugin->plg_callbacks.on_EXOSIP_CALL_NOANSWER				=	&sdp_on_EXOSIP_CALL_NOANSWER;
	plugin->plg_callbacks.on_EXOSIP_CALL_OFFHOLD				=	&sdp_on_EXOSIP_CALL_OFFHOLD;
	plugin->plg_callbacks.on_EXOSIP_CALL_PROCEEDING				=	&sdp_on_EXOSIP_CALL_PROCEEDING;
	plugin->plg_callbacks.on_EXOSIP_CALL_REDIRECTED				=	&sdp_on_EXOSIP_CALL_REDIRECTED;
	plugin->plg_callbacks.on_EXOSIP_CALL_REFER_FAILURE			=	&sdp_on_EXOSIP_CALL_REFER_FAILURE;
	plugin->plg_callbacks.on_EXOSIP_CALL_REFER_STATUS			=	&sdp_on_EXOSIP_CALL_REFER_STATUS;
	plugin->plg_callbacks.on_EXOSIP_CALL_REFERED				=	&sdp_on_EXOSIP_CALL_REFERED;
	plugin->plg_callbacks.on_EXOSIP_CALL_REPLACES				=	&sdp_on_EXOSIP_CALL_REPLACES;
	plugin->plg_callbacks.on_EXOSIP_CALL_REQUESTFAILURE			=	&sdp_on_EXOSIP_CALL_REQUESTFAILURE;
	plugin->plg_callbacks.on_EXOSIP_CALL_RINGING				=	&sdp_on_EXOSIP_CALL_RINGING;
	plugin->plg_callbacks.on_EXOSIP_CALL_SERVERFAILURE			=	&sdp_on_EXOSIP_CALL_SERVERFAILURE;

	return plugin;
}


static void sdp_on_EXOSIP_CALL_NEW(eXosip_event_t * event){
	ph_call_new(event);
}	

static void sdp_on_EXOSIP_CALL_ANSWERED(eXosip_event_t * event){
	ph_callStopRinging(event);
	ph_call_answered(event);
}

static void sdp_on_EXOSIP_CALL_PROCEEDING(eXosip_event_t * event){
	ph_call_proceeding(event);
}

static void sdp_on_EXOSIP_CALL_RINGING(eXosip_event_t * event){
	ph_call_ringing(event);
}

static void sdp_on_EXOSIP_CALL_REDIRECTED(eXosip_event_t * event){
	ph_call_redirected(event);
}

static void sdp_on_EXOSIP_CALL_REPLACES(eXosip_event_t * event){
	ph_call_replaces(event);
}

static void sdp_on_EXOSIP_CALL_REQUESTFAILURE(eXosip_event_t * event){
	ph_call_requestfailure(event);
	phplugin_disassociate_callid_from_any_plugin(event->cid);
}

static void sdp_on_EXOSIP_CALL_SERVERFAILURE(eXosip_event_t * event){
	ph_call_serverfailure(event);
	phplugin_disassociate_callid_from_any_plugin(event->cid);
}

static void sdp_on_EXOSIP_CALL_GLOBALFAILURE(eXosip_event_t * event){
	ph_call_globalfailure(event);
	phplugin_disassociate_callid_from_any_plugin(event->cid);
}

static void sdp_on_EXOSIP_CALL_NOANSWER(eXosip_event_t * event){
	ph_call_noanswer(event);
	phplugin_disassociate_callid_from_any_plugin(event->cid);
}

static void sdp_on_EXOSIP_CALL_CLOSED(eXosip_event_t * event){
	ph_call_closed(event);
	phplugin_disassociate_callid_from_any_plugin(event->cid);
}

static void sdp_on_EXOSIP_CALL_HOLD(eXosip_event_t * event){
	ph_call_onhold(event);
}

static void sdp_on_EXOSIP_CALL_OFFHOLD(eXosip_event_t * event){
	ph_call_offhold(event);
}

static void sdp_on_EXOSIP_CALL_REFERED(eXosip_event_t * event){
	ph_call_refered(event);
}

static void sdp_on_EXOSIP_CALL_REFER_STATUS(eXosip_event_t * event){
}

static void sdp_on_EXOSIP_CALL_REFER_FAILURE(eXosip_event_t * event){
	ph_call_refer_status(event);
}