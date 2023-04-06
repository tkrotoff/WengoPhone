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

#ifndef __PHPLUGIN_H__
#define __PHPLUGIN_H__

#include <eXosip/eXosip.h>
#include <phapi-util/mappinglist.h>


#ifdef WIN32
#if defined(BUILD_PHAPI_DLL)
#define MY_DLLEXPORT __declspec(dllexport)
#elif defined(PHAPI_DLL)
#define MY_DLLEXPORT __declspec(dllimport)
#endif
#endif

#ifndef MY_DLLEXPORT
#define MY_DLLEXPORT 
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	//#define PLUGIN_MAX_COUNT	8

	// ----- TYPES -----
	struct phplugin_callbacks
	{
		void (*on_EXOSIP_CALL_NEW)						(eXosip_event_t *);
		void (*on_EXOSIP_CALL_ANSWERED)					(eXosip_event_t *);
		void (*on_EXOSIP_CALL_PROCEEDING)				(eXosip_event_t *);
		void (*on_EXOSIP_CALL_RINGING)					(eXosip_event_t *);
		void (*on_EXOSIP_CALL_REDIRECTED)				(eXosip_event_t *);
		void (*on_EXOSIP_CALL_REPLACES)					(eXosip_event_t *);
		void (*on_EXOSIP_CALL_REQUESTFAILURE)			(eXosip_event_t *);
		void (*on_EXOSIP_CALL_SERVERFAILURE)			(eXosip_event_t *);
		void (*on_EXOSIP_CALL_GLOBALFAILURE)			(eXosip_event_t *);
		void (*on_EXOSIP_CALL_NOANSWER)					(eXosip_event_t *);
		void (*on_EXOSIP_CALL_CLOSED)					(eXosip_event_t *);
		void (*on_EXOSIP_CALL_HOLD)						(eXosip_event_t *);
		void (*on_EXOSIP_CALL_OFFHOLD)					(eXosip_event_t *);
		void (*on_EXOSIP_CALL_REFERED)					(eXosip_event_t *);
		void (*on_EXOSIP_CALL_REFER_STATUS)				(eXosip_event_t *);
		void (*on_EXOSIP_CALL_REFER_FAILURE)			(eXosip_event_t *);
	};

	struct phplugin_content_type{
		char type[32];
		char subtype[32];
	};
	typedef struct phplugin_content_type phplugin_content_type_t;

	struct phplugin{
		struct phplugin_content_type content_type;
		struct phplugin_callbacks plg_callbacks;
	};

	typedef struct phplugin phplugin_t;
	// -----


	// ----- GLOBALS -----

	/**
	* Indexes the plugins by call_ids
	*/
	extern mappinglist_t * callids_to_plugins;

	/**
	* Indexes the plugins by content types
	*/
	extern mappinglist_t * content_types_to_plugins;
	// -----



	//phplugin_t * get_plugin();

	/**
	* Registers a plugin into PhApi
	*
	* @param	[in]	plugin : the plugin to register
	* @return	TRUE if the plugin could be registered; FALSE else
	*/
	MY_DLLEXPORT unsigned int phplugin_register(phplugin_t * plugin);

	/**
	* Un-registers a plugin from PhApi
	*
	* @param	[in]	plugin : the plugin to un-register
	* @return	the un-registered plugin or NULL if the plugin couldn't be un-registered
	*/
	MY_DLLEXPORT phplugin_t * phplugin_unregister(phplugin_t * plugin);

	/**
	* Checks wether a plugin is registered or not
	*
	* @param	[in]	plugin : the plugin to find
	* @return	TRUE if the plugin is registered; FALSE else
	*/
	unsigned int phplugin_is_registered(phplugin_t * plugin);

	/**
	* Gets the plugin associated to the given content type
	*
	* @param	[in]	content_type : a content type
	* @return	the plugin associated with content_type, or NULL if no associated plugin was found
	*/
	phplugin_t * phplugin_get_plugin_associated_to_content_type(phplugin_content_type_t content_type);

	/**
	* Associates a plugin to a call id
	*
	* @param	[in]	call_id : a call id
	* @param	[in]	plugin : a plugin
	* @return	TRUE if the plugin could be associated to the call_id; FALSE else
	*/
	unsigned int phplugin_associate_callid_to_plugin(int callid, phplugin_t * plugin);

	/**
	* Dis-associates a call id from any plugin
	*
	* @param	[in]	call_id : a call id
	* @return	TRUE if the plugin could be dis-associated to the call_id; FALSE else
	*/
	unsigned int phplugin_disassociate_callid_from_any_plugin(int call_id);

	/**
	* Associates a plugin to a call id via a given body type
	*
	* @param	[in]	call_id : a call id
	* @param	[in]	body_type : a body type
	* @return	TRUE if the plugin could be associated to the call_id; FALSE else
	*/
	unsigned int phplugin_associate_callid_to_plugin2(int call_id, const char * body_type);

	/**
	* Gets the plugin associated to a call id
	*
	* @param	[in]	call_id : a call id
	* @return	the plugin associated with the call id, or NULL if no associated plugin was found
	*/
	phplugin_t * phplugin_get_plugin_associated_to_call_id(int callid);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PHPLUGIN_H__ */
