/**
 * Gaim is the legal property of its developers, whose names are too numerous
 * to list here.  Please refer to the COPYRIGHT file distributed with this
 * source distribution.
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
 */

#ifndef _GAIM_GOOGLE_H_
#define _GAIM_GOOGLE_H_

/* This is a place for Google Talk-specific XMPP extensions to live
 * such that they don't intermingle with code for the XMPP RFCs and XEPs :) */

#include "jabber.h"

void jabber_gmail_init(JabberStream *js);
void jabber_gmail_poke(JabberStream *js, xmlnode *node);

void jabber_google_roster_init(JabberStream *js);
void jabber_google_roster_outgoing(JabberStream *js, xmlnode *query, xmlnode *item);
void jabber_google_roster_incoming(JabberStream *js, xmlnode *item);
void jabber_google_roster_add_deny(GaimConnection *gc, const char *who);
void jabber_google_roster_rem_deny(GaimConnection *gc, const char *who);



#endif   /* _GAIM_GOOGLE_H_ */
