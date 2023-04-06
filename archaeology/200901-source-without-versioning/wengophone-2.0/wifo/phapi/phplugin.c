/*
 * phapi   phone api
 *
 * Copyright (C) 2006 Wengo SAS
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
#include <phapi-util/util.h>
#include <phapi-util/mystring.h>

// ----- PRIVATE FUNCTIONS DECLARATION -----
static unsigned int phplugin_content_type_equals(void * element_a, void * element_b);
// -----

// ----- GLOBALS -----

/**
* Indexes the plugins by call_ids
*/
mappinglist_t * callids_to_plugins = NULL;

/**
* Indexes the plugins by content types
*/
mappinglist_t * content_types_to_plugins = NULL;
// -----


/**
* Registers a plugin into PhApi
*
* @param	[in]	plugin : the plugin to register
* @return	TRUE if the plugin could be registered; FALSE else
*/
MY_DLLEXPORT unsigned int phplugin_register(phplugin_t * plugin){
	if(content_types_to_plugins == NULL)
	{
		content_types_to_plugins = create_mappinglist();
	}
	return mappinglist_put(content_types_to_plugins, &(plugin->content_type), plugin, &phplugin_content_type_equals);
}

/**
* Un-registers a plugin from PhApi
*
* @param	[in]	plugin : the plugin to un-register
* @return	the un-registered plugin or NULL if the plugin couldn't be un-registered
*/
MY_DLLEXPORT phplugin_t * phplugin_unregister(phplugin_t * plugin){
	return (phplugin_t *)mappinglist_remove(content_types_to_plugins, &(plugin->content_type), &phplugin_content_type_equals);
}

/**
* Checks wether a plugin is registered or not
*
* @param	[in]	plugin : the plugin to find
* @return	TRUE if the plugin is registered; FALSE else
*/
unsigned int phplugin_is_registered(phplugin_t * plugin){
	return mappinglist_contains_element(content_types_to_plugins, plugin, NULL);
}

/**
* Gets the plugin associated to the given content type
*
* @param	[in]	content_type : a content type
* @return	the plugin associated with content_type, or NULL if no associated plugin was found
*/
phplugin_t * phplugin_get_plugin_associated_to_content_type(phplugin_content_type_t content_type){
	return (phplugin_t *)mappinglist_get(content_types_to_plugins, &content_type, &phplugin_content_type_equals);
}

/**
* Associates a plugin to a call id
*
* @param	[in]	call_id : a call id
* @param	[in]	plugin : a plugin
* @return	TRUE if the plugin could be associated to the call_id; FALSE else
*/
unsigned int phplugin_associate_callid_to_plugin(int call_id, phplugin_t * plugin){
	if(callids_to_plugins == NULL)
	{
		callids_to_plugins = create_mappinglist();
	}
	return mappinglist_put_with_int_key(callids_to_plugins, call_id, plugin);
}

/**
* Dis-associates a call id from any plugin
*
* @param	[in]	call_id : a call id
* @return	TRUE if the plugin could be dis-associated to the call_id; FALSE else
*/
unsigned int phplugin_disassociate_callid_from_any_plugin(int call_id){
	if(mappinglist_remove_with_int_key(callids_to_plugins, call_id) != NULL)
	{		
		return TRUE;
	}
	return FALSE;
}

/**
* Associates a plugin to a call id via a given body type
*
* @param	[in]	call_id : a call id
* @param	[in]	body_type : a body type
* @return	TRUE if the plugin could be associated to the call_id; FALSE else
*/
unsigned int phplugin_associate_callid_to_plugin2(int call_id, const char * body_type){
	phplugin_content_type_t content_type;
	phplugin_t * plugin;

	// prepare the key (content type)			
	sscanf2(body_type, "%s/%s", content_type.type, sizeof(content_type.type), content_type.subtype, sizeof(content_type.subtype));
	// get the plugin associated with the content type
	plugin = phplugin_get_plugin_associated_to_content_type(content_type);
	// associates the plugin to the call id
	if(plugin != NULL)
	{
		phplugin_associate_callid_to_plugin(call_id, plugin);
		return TRUE;
	}
	return FALSE;
}

/**
* Gets the plugin associated to a call id
*
* @param	[in]	call_id : a call id
* @return	the plugin associated with the call id, or NULL if no associated plugin was found
*/
phplugin_t * phplugin_get_plugin_associated_to_call_id(int call_id){
	return (phplugin_t *)mappinglist_get_with_int_key(callids_to_plugins, call_id);
}

// ----- PRIVATE FUNCTIONS -----

/**
* Content type equality checker
*
* @param	[in]	element_a : a content type
* @param	[in]	element_b : another content type
* @return	TRUE if both content types are equal; FALSE else
*/
static unsigned int phplugin_content_type_equals(void * element_a, void * element_b){
	phplugin_content_type_t * _a;
	phplugin_content_type_t * _b;

	if(element_a != NULL && element_b != NULL)
	{
		_a = (phplugin_content_type_t *)element_a;
		_b = (phplugin_content_type_t *)element_b;

		if(strcasecmp(_a->type, _b->type)==0 && strcasecmp(_a->subtype, _b->subtype)==0)
		{
			return TRUE;
		}
	}
	return FALSE;
}
