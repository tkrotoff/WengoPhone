/**
 * @file error.c Error functions
 *
 * gaim
 *
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
#include "msn.h"
#include "error.h"

const char *
msn_error_get_text(unsigned int type)
{
	static char msg[MSN_BUF_LEN];

	switch (type) {
		case 0:
			g_snprintf(msg, sizeof(msg),
					   _("Unable to parse message"));
		case 200:
			g_snprintf(msg, sizeof(msg),
					   _("Syntax Error (probably a Gaim bug)"));
			break;
		case 201:
			g_snprintf(msg, sizeof(msg),
					   _("Invalid e-mail address"));
			break;
		case 205:
			g_snprintf(msg, sizeof(msg), _("User does not exist"));
			break;
		case 206:
			g_snprintf(msg, sizeof(msg),
					   _("Fully Qualified Domain Name missing"));
			break;
		case 207:
			g_snprintf(msg, sizeof(msg), _("Already Logged In"));
			break;
		case 208:
			g_snprintf(msg, sizeof(msg), _("Invalid Username"));
			break;
		case 209:
			g_snprintf(msg, sizeof(msg), _("Invalid Friendly Name"));
			break;
		case 210:
			g_snprintf(msg, sizeof(msg), _("List Full"));
			break;
		case 215:
			g_snprintf(msg, sizeof(msg), _("Already there"));
			break;
		case 216:
			g_snprintf(msg, sizeof(msg), _("Not on list"));
			break;
		case 217:
			g_snprintf(msg, sizeof(msg), _("User is offline"));
			break;
		case 218:
			g_snprintf(msg, sizeof(msg), _("Already in the mode"));
			break;
		case 219:
			g_snprintf(msg, sizeof(msg), _("Already in opposite list"));
			break;
		case 223:
			g_snprintf(msg, sizeof(msg), _("Too many groups"));
			break;
		case 224:
			g_snprintf(msg, sizeof(msg), _("Invalid group"));
			break;
		case 225:
			g_snprintf(msg, sizeof(msg), _("User not in group"));
			break;
		case 229:
			g_snprintf(msg, sizeof(msg), _("Group name too long"));
			break;
		case 230:
			g_snprintf(msg, sizeof(msg), _("Cannot remove group zero"));
			break;
		case 231:
			g_snprintf(msg, sizeof(msg),
					   _("Tried to add a user to a group "
						 "that doesn't exist"));
			break;
		case 280:
			g_snprintf(msg, sizeof(msg), _("Switchboard failed"));
			break;
		case 281:
			g_snprintf(msg, sizeof(msg), _("Notify Transfer failed"));
			break;

		case 300:
			g_snprintf(msg, sizeof(msg), _("Required fields missing"));
			break;
		case 301:
			g_snprintf(msg, sizeof(msg), _("Too many hits to a FND"));
			break;
		case 302:
			g_snprintf(msg, sizeof(msg), _("Not logged in"));
			break;

		case 500:
			g_snprintf(msg, sizeof(msg), _("Service Temporarily Unavailable"));
			break;
		case 501:
			g_snprintf(msg, sizeof(msg), _("Database server error"));
			break;
		case 502:
			g_snprintf(msg, sizeof(msg), _("Command disabled"));
			break;
		case 510:
			g_snprintf(msg, sizeof(msg), _("File operation error"));
			break;
		case 520:
			g_snprintf(msg, sizeof(msg), _("Memory allocation error"));
			break;
		case 540:
			g_snprintf(msg, sizeof(msg), _("Wrong CHL value sent to server"));
			break;

		case 600:
			g_snprintf(msg, sizeof(msg), _("Server busy"));
			break;
		case 601:
			g_snprintf(msg, sizeof(msg), _("Server unavailable"));
			break;
		case 602:
			g_snprintf(msg, sizeof(msg), _("Peer Notification server down"));
			break;
		case 603:
			g_snprintf(msg, sizeof(msg), _("Database connect error"));
			break;
		case 604:
			g_snprintf(msg, sizeof(msg),
					   _("Server is going down (abandon ship)"));
			break;
		case 605:
			g_snprintf(msg, sizeof(msg), _("Server unavailable"));
			break;

		case 707:
			g_snprintf(msg, sizeof(msg), _("Error creating connection"));
			break;
		case 710:
			g_snprintf(msg, sizeof(msg),
					   _("CVR parameters are either unknown or not allowed"));
			break;
		case 711:
			g_snprintf(msg, sizeof(msg), _("Unable to write"));
			break;
		case 712:
			g_snprintf(msg, sizeof(msg), _("Session overload"));
			break;
		case 713:
			g_snprintf(msg, sizeof(msg), _("User is too active"));
			break;
		case 714:
			g_snprintf(msg, sizeof(msg), _("Too many sessions"));
			break;
		case 715:
			g_snprintf(msg, sizeof(msg), _("Passport not verified"));
			break;
		case 717:
			g_snprintf(msg, sizeof(msg), _("Bad friend file"));
			break;
		case 731:
			g_snprintf(msg, sizeof(msg), _("Not expected"));
			break;

		case 800:
			g_snprintf(msg, sizeof(msg),
					   _("Friendly name changes too rapidly"));
			break;

		case 910:
		case 912:
		case 918:
		case 919:
		case 921:
		case 922:
			g_snprintf(msg, sizeof(msg), _("Server too busy"));
			break;
		case 911:
		case 917:
			g_snprintf(msg, sizeof(msg), _("Authentication failed"));
			break;
		case 913:
			g_snprintf(msg, sizeof(msg), _("Not allowed when offline"));
			break;
		case 914:
		case 915:
		case 916:
			g_snprintf(msg, sizeof(msg), _("Server unavailable"));
			break;
		case 920:
			g_snprintf(msg, sizeof(msg), _("Not accepting new users"));
			break;
		case 923:
			g_snprintf(msg, sizeof(msg),
					   _("Kids Passport without parental consent"));
			break;
		case 924:
			g_snprintf(msg, sizeof(msg),
					   _("Passport account not yet verified"));
			break;
		case 928:
			g_snprintf(msg, sizeof(msg), _("Bad ticket"));
			break;

		default:
			g_snprintf(msg, sizeof(msg), _("Unknown Error Code %d"), type);
			break;
	}

	return msg;
}

void
msn_error_handle(MsnSession *session, unsigned int type)
{
	char buf[MSN_BUF_LEN];

	g_snprintf(buf, sizeof(buf), _("MSN Error: %s\n"),
			   msn_error_get_text(type));

	gaim_notify_error(session->account->gc, NULL, buf, NULL);
}
