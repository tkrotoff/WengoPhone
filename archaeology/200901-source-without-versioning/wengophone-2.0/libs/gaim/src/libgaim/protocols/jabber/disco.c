/*
 * gaim - Jabber Protocol Plugin
 *
 * Copyright (C) 2003, Nathan Walp <faceprint@faceprint.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "internal.h"
#include "prefs.h"

#include "buddy.h"
#include "iq.h"
#include "disco.h"
#include "jabber.h"


struct _jabber_disco_info_cb_data {
	gpointer data;
	JabberDiscoInfoCallback *callback;
};

#define SUPPORT_FEATURE(x) \
	feature = xmlnode_new_child(query, "feature"); \
	xmlnode_set_attrib(feature, "var", x);


void jabber_disco_info_parse(JabberStream *js, xmlnode *packet) {
	const char *from = xmlnode_get_attrib(packet, "from");
	const char *type = xmlnode_get_attrib(packet, "type");

	if(!from || !type)
		return;

	if(!strcmp(type, "get")) {
		xmlnode *query, *identity, *feature;
		JabberIq *iq;

		xmlnode *in_query;
		const char *node = NULL;

		if((in_query = xmlnode_get_child(packet, "query"))) {
			node = xmlnode_get_attrib(in_query, "node");
		}


		iq = jabber_iq_new_query(js, JABBER_IQ_RESULT,
				"http://jabber.org/protocol/disco#info");

		jabber_iq_set_id(iq, xmlnode_get_attrib(packet, "id"));

		xmlnode_set_attrib(iq->node, "to", from);
		query = xmlnode_get_child(iq->node, "query");

		if(node)
			xmlnode_set_attrib(query, "node", node);

		if(!node || !strcmp(node, CAPS0115_NODE "#" VERSION)) {

			identity = xmlnode_new_child(query, "identity");
			xmlnode_set_attrib(identity, "category", "client");
			xmlnode_set_attrib(identity, "type", "pc"); /* XXX: bot, console,
														 * handheld, pc, phone,
														 * web */
			xmlnode_set_attrib(identity, "name", PACKAGE);

			SUPPORT_FEATURE("jabber:iq:last")
			SUPPORT_FEATURE("jabber:iq:oob")
			SUPPORT_FEATURE("jabber:iq:time")
			SUPPORT_FEATURE("jabber:iq:version")
			SUPPORT_FEATURE("jabber:x:conference")
			SUPPORT_FEATURE("http://jabber.org/protocol/bytestreams")
			SUPPORT_FEATURE("http://jabber.org/protocol/disco#info")
			SUPPORT_FEATURE("http://jabber.org/protocol/disco#items")
#if 0
				SUPPORT_FEATURE("http://jabber.org/protocol/ibb")
#endif
			SUPPORT_FEATURE("http://jabber.org/protocol/muc")
			SUPPORT_FEATURE("http://jabber.org/protocol/muc#user")
			SUPPORT_FEATURE("http://jabber.org/protocol/si")
			SUPPORT_FEATURE("http://jabber.org/protocol/si/profile/file-transfer")
			SUPPORT_FEATURE("http://jabber.org/protocol/xhtml-im")
		} else {
			xmlnode *error, *inf;

			/* XXX: gross hack, implement jabber_iq_set_type or something */
			xmlnode_set_attrib(iq->node, "type", "error");
			iq->type = JABBER_IQ_ERROR;

			error = xmlnode_new_child(query, "error");
			xmlnode_set_attrib(error, "code", "404");
			xmlnode_set_attrib(error, "type", "cancel");
			inf = xmlnode_new_child(error, "item-not-found");
			xmlnode_set_namespace(inf, "urn:ietf:params:xml:ns:xmpp-stanzas");
		}

		jabber_iq_send(iq);
	} else if(!strcmp(type, "result")) {
		xmlnode *query = xmlnode_get_child(packet, "query");
		xmlnode *child;
		JabberID *jid;
		JabberBuddy *jb;
		JabberBuddyResource *jbr = NULL;
		JabberCapabilities capabilities = JABBER_CAP_NONE;
		struct _jabber_disco_info_cb_data *jdicd;

		if((jid = jabber_id_new(from))) {
			if(jid->resource && (jb = jabber_buddy_find(js, from, TRUE)))
				jbr = jabber_buddy_find_resource(jb, jid->resource);
			jabber_id_free(jid);
		}

		if(jbr)
			capabilities = jbr->capabilities;

		for(child = query->child; child; child = child->next) {
			if(child->type != XMLNODE_TYPE_TAG)
				continue;

			if(!strcmp(child->name, "identity")) {
				const char *category = xmlnode_get_attrib(child, "category");
				const char *type = xmlnode_get_attrib(child, "type");
				if(!category || !type)
					continue;

				if(!strcmp(category, "conference") && !strcmp(type, "text")) {
					/* we found a groupchat or MUC server, add it to the list */
					/* XXX: actually check for protocol/muc or gc-1.0 support */
					js->chat_servers = g_list_append(js->chat_servers, g_strdup(from));
				} else if(!strcmp(category, "directory") && !strcmp(type, "user")) {
					/* we found a JUD */
					js->user_directories = g_list_append(js->user_directories, g_strdup(from));
				}

			} else if(!strcmp(child->name, "feature")) {
				const char *var = xmlnode_get_attrib(child, "var");
				if(!var)
					continue;

				if(!strcmp(var, "http://jabber.org/protocol/si"))
					capabilities |= JABBER_CAP_SI;
				else if(!strcmp(var, "http://jabber.org/protocol/si/profile/file-transfer"))
					capabilities |= JABBER_CAP_SI_FILE_XFER;
				else if(!strcmp(var, "http://jabber.org/protocol/bytestreams"))
					capabilities |= JABBER_CAP_BYTESTREAMS;
				else if(!strcmp(var, "jabber:iq:search"))
					capabilities |= JABBER_CAP_IQ_SEARCH;
				else if(!strcmp(var, "jabber:iq:register"))
					capabilities |= JABBER_CAP_IQ_REGISTER;
			}
		}

		capabilities |= JABBER_CAP_RETRIEVED;

		if(jbr)
			jbr->capabilities = capabilities;

		if((jdicd = g_hash_table_lookup(js->disco_callbacks, from))) {
			jdicd->callback(js, from, capabilities, jdicd->data);
			g_hash_table_remove(js->disco_callbacks, from);
		}
	} else if(!strcmp(type, "error")) {
		JabberID *jid;
		JabberBuddy *jb;
		JabberBuddyResource *jbr = NULL;
		JabberCapabilities capabilities = JABBER_CAP_NONE;
		struct _jabber_disco_info_cb_data *jdicd;

		if(!(jdicd = g_hash_table_lookup(js->disco_callbacks, from)))
			return;

		if((jid = jabber_id_new(from))) {
			if(jid->resource && (jb = jabber_buddy_find(js, from, TRUE)))
				jbr = jabber_buddy_find_resource(jb, jid->resource);
			jabber_id_free(jid);
		}

		if(jbr)
			capabilities = jbr->capabilities;

		jdicd->callback(js, from, capabilities, jdicd->data);
		g_hash_table_remove(js->disco_callbacks, from);
	}
}

void jabber_disco_items_parse(JabberStream *js, xmlnode *packet) {
	const char *from = xmlnode_get_attrib(packet, "from");
	const char *type = xmlnode_get_attrib(packet, "type");

	if(type && !strcmp(type, "get")) {
		JabberIq *iq = jabber_iq_new_query(js, JABBER_IQ_RESULT,
				"http://jabber.org/protocol/disco#items");

		jabber_iq_set_id(iq, xmlnode_get_attrib(packet, "id"));

		xmlnode_set_attrib(iq->node, "to", from);
		jabber_iq_send(iq);
	}
}

static void
jabber_disco_server_result_cb(JabberStream *js, xmlnode *packet, gpointer data)
{
	xmlnode *query, *child;
	const char *from = xmlnode_get_attrib(packet, "from");
	const char *type = xmlnode_get_attrib(packet, "type");

	if(!from || !type)
		return;

	if(strcmp(from, js->user->domain))
		return;

	if(strcmp(type, "result"))
		return;

	while(js->chat_servers) {
		g_free(js->chat_servers->data);
		js->chat_servers = g_list_delete_link(js->chat_servers, js->chat_servers);
	}

	query = xmlnode_get_child(packet, "query");

	for(child = xmlnode_get_child(query, "item"); child;
			child = xmlnode_get_next_twin(child)) {
		JabberIq *iq;
		const char *jid;

		if(!(jid = xmlnode_get_attrib(child, "jid")))
			continue;

		iq = jabber_iq_new_query(js, JABBER_IQ_GET, "http://jabber.org/protocol/disco#info");
		xmlnode_set_attrib(iq->node, "to", jid);
		jabber_iq_send(iq);
	}
}

void jabber_disco_items_server(JabberStream *js)
{
	JabberIq *iq = jabber_iq_new_query(js, JABBER_IQ_GET,
			"http://jabber.org/protocol/disco#items");

	xmlnode_set_attrib(iq->node, "to", js->user->domain);

	jabber_iq_set_callback(iq, jabber_disco_server_result_cb, NULL);
	jabber_iq_send(iq);
}

void jabber_disco_info_do(JabberStream *js, const char *who, JabberDiscoInfoCallback *callback, gpointer data)
{
	JabberID *jid;
	JabberBuddy *jb;
	JabberBuddyResource *jbr = NULL;
	struct _jabber_disco_info_cb_data *jdicd;
	JabberIq *iq;

	if((jid = jabber_id_new(who))) {
		if(jid->resource && (jb = jabber_buddy_find(js, who, TRUE)))
			jbr = jabber_buddy_find_resource(jb, jid->resource);
		jabber_id_free(jid);
	}

	if(jbr && jbr->capabilities & JABBER_CAP_RETRIEVED) {
		callback(js, who, jbr->capabilities, data);
		return;
	}

	jdicd = g_new0(struct _jabber_disco_info_cb_data, 1);
	jdicd->data = data;
	jdicd->callback = callback;

	g_hash_table_insert(js->disco_callbacks, g_strdup(who), jdicd);

	iq = jabber_iq_new_query(js, JABBER_IQ_GET, "http://jabber.org/protocol/disco#info");
	xmlnode_set_attrib(iq->node, "to", who);

	jabber_iq_send(iq);
}


