/**
 * The QQ2003C protocol plugin
 *
 * for gaim
 *
 * Copyright (C) 2004 Puzzlebird
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

#include <string.h>

#include "packet_parse.h"

/* read one byte from buf, 
 * return the number of bytes read if succeeds, otherwise return -1 */
gint read_packet_b(guint8 *buf, guint8 **cursor, gint buflen, guint8 *b)
{
	if (*cursor <= buf + buflen - sizeof(*b)) {
		*b = **(guint8 **) cursor;
		*cursor += sizeof(*b);
		return sizeof(*b);
	} else {
		return -1;
	}
}

/* read two bytes as "guint16" from buf, 
 * return the number of bytes read if succeeds, otherwise return -1 */
gint read_packet_w(guint8 *buf, guint8 **cursor, gint buflen, guint16 *w)
{
	if (*cursor <= buf + buflen - sizeof(*w)) {
		*w = g_ntohs(**(guint16 **) cursor);
		*cursor += sizeof(*w);
		return sizeof(*w);
	} else {
		return -1;
	}
}

/* read four bytes as "guint32" from buf, 
 * return the number of bytes read if succeeds, otherwise return -1 */
gint read_packet_dw(guint8 *buf, guint8 **cursor, gint buflen, guint32 *dw)
{
	if (*cursor <= buf + buflen - sizeof(*dw)) {
		*dw = g_ntohl(**(guint32 **) cursor);
		*cursor += sizeof(*dw);
		return sizeof(*dw);
	} else {
		return -1;
	}
}

/* read datalen bytes from buf, 
 * return the number of bytes read if succeeds, otherwise return -1 */
gint read_packet_data(guint8 *buf, guint8 **cursor, gint buflen, guint8 *data, gint datalen) {
	if (*cursor <= buf + buflen - datalen) {
		g_memmove(data, *cursor, datalen);
		*cursor += datalen;
		return datalen;
	} else {
		return -1;
	}
}

/* pack one byte into buf
 * return the number of bytes packed, otherwise return -1 */
gint create_packet_b(guint8 *buf, guint8 **cursor, guint8 b)
{
	if (*cursor <= buf + MAX_PACKET_SIZE - sizeof(guint8)) {
		**(guint8 **) cursor = b;
		*cursor += sizeof(guint8);
		return sizeof(guint8);
	} else {
		return -1;
	}
}

/* pack two bytes as "guint16" into buf
 * return the number of bytes packed, otherwise return -1 */
gint create_packet_w(guint8 *buf, guint8 **cursor, guint16 w)
{
	if (*cursor <= buf + MAX_PACKET_SIZE - sizeof(guint16)) {
		**(guint16 **) cursor = g_htons(w);
		*cursor += sizeof(guint16);
		return sizeof(guint16);
	} else {
		return -1;
	}
}

/* pack four bytes as "guint32" into buf
 * return the number of bytes packed, otherwise return -1 */
gint create_packet_dw(guint8 *buf, guint8 **cursor, guint32 dw)
{
	if (*cursor <= buf + MAX_PACKET_SIZE - sizeof(guint32)) {
		**(guint32 **) cursor = g_htonl(dw);
		*cursor += sizeof(guint32);
		return sizeof(guint32);
	} else {
		return -1;
	}
}

/* pack datalen bytes into buf
 * return the number of bytes packed, otherwise return -1 */
gint create_packet_data(guint8 *buf, guint8 **cursor, guint8 *data, gint datalen)
{
	if (*cursor <= buf + MAX_PACKET_SIZE - datalen) {
		g_memmove(*cursor, data, datalen);
		*cursor += datalen;
		return datalen;
	} else {
		return -1;
	}
}
