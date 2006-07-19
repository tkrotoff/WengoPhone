/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
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

#include <cutil/global.h>

#ifdef OS_WINDOWS
	#include <winsock2.h>
	#include <windows.h>
#endif

extern "C" {
#include "glib.h"
#include "gaim/internal.h"
#include "gaim/core.h"
#include "gaim/eventloop.h"
#include "gaim/blist.h"
#include "gaim/util.h"
#include "gaim/connection.h"
#include "gaim/blist.h"
#include "gaim/conversation.h"
#include "gaim/account.h"
#include "gaim/privacy.h"
}

#include "GaimIMFactory.h"

#include "GaimAccountMngr.h"
#include "GaimChatMngr.h"
#include "GaimConnectMngr.h"
#include "GaimContactListMngr.h"
#include "GaimPresenceMngr.h"

#include <util/File.h>
#include <util/Logger.h>
#include <util/Path.h>

extern GaimConversationUiOps chat_wg_ops;
extern GaimBlistUiOps blist_wg_ops;
extern GaimConnectionUiOps conn_wg_ops;
extern GaimAccountUiOps acc_wg_ops;
extern GaimPrivacyUiOps privacy_wg_ops;


/* ********************* GAIM CALLBACK ********************* */
#define GAIM_WG_READ_COND  (G_IO_IN | G_IO_HUP | G_IO_ERR)
#define GAIM_WG_WRITE_COND (G_IO_OUT | G_IO_HUP | G_IO_ERR | G_IO_NVAL)

typedef struct _GaimWgIOClosure {
	GaimInputFunction function;
	guint result;
	gpointer data;

} GaimWgIOClosure;

static void gaim_wg_io_destroy(gpointer data)
{
	g_free(data);
}

static gboolean gaim_wg_io_invoke(GIOChannel *source, GIOCondition condition, gpointer data)
{
	GaimWgIOClosure *closure = (GaimWgIOClosure *) data;
	GaimInputCondition gaim_cond = (GaimInputCondition) 0;

	if (condition & GAIM_WG_READ_COND)
		gaim_cond = (GaimInputCondition)(gaim_cond|GAIM_INPUT_READ);
	if (condition & GAIM_WG_WRITE_COND)
		gaim_cond = (GaimInputCondition)(gaim_cond|GAIM_INPUT_WRITE);

#ifdef OS_WINDOWS
	if(! gaim_cond) {
#if DEBUG
		gaim_debug(GAIM_DEBUG_MISC, "wg_eventloop",
			"CLOSURE received GIOCondition of 0x%x, which does not"
			" match 0x%x (READ) or 0x%x (WRITE)\n",
			condition, GAIM_WG_READ_COND, GAIM_WG_WRITE_COND);
#endif /* DEBUG */

		return TRUE;
	}
#endif /* OS_WINDOWS */

#ifdef OS_WINDOWS
	closure->function(closure->data, g_io_channel_win32_get_fd(source), gaim_cond);
#else
	closure->function(closure->data, g_io_channel_unix_get_fd(source), gaim_cond);
#endif

	return TRUE;
}

static guint gaim_wg_input_add(gint fd, GaimInputCondition condition, 
	GaimInputFunction function, gpointer data)
{
	GaimWgIOClosure *closure = g_new0(GaimWgIOClosure, 1);
	GIOChannel *channel;
	GIOCondition cond = (GIOCondition) 0;

	closure->function = function;
	closure->data = data;

	if (condition & GAIM_INPUT_READ)
		cond = (GIOCondition)(cond|GAIM_WG_READ_COND);
	if (condition & GAIM_INPUT_WRITE)
		cond = (GIOCondition)(cond|GAIM_WG_WRITE_COND);

#ifdef OS_WINDOWS
	channel = g_io_channel_win32_new_socket(fd);
#else
	channel = g_io_channel_unix_new(fd);
#endif
	closure->result = g_io_add_watch_full(channel, G_PRIORITY_DEFAULT, cond,
		gaim_wg_io_invoke, closure, gaim_wg_io_destroy);

	g_io_channel_unref(channel);
	return closure->result;
}

static void sigpipe_catcher(int sig) {
	LOG_DEBUG("SIGPIPE caught: " + String::fromNumber(sig));
}

gpointer GaimMainEventLoop(gpointer data)
{
	GMainLoop *loop;

	loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(loop);

	g_thread_exit(NULL);
	return NULL;
}


/* ******************************************************* */

static GaimCoreUiOps core_wg_ops =
{
	NULL,
	NULL,
	NULL,
	NULL
};

static GaimEventLoopUiOps eventloop_wg_ops =
{
	g_timeout_add,
	(guint (*)(guint))g_source_remove,
	gaim_wg_input_add,
	(guint (*)(guint))g_source_remove
};

GaimIMFactory::GaimIMFactory()
{
	AccountMngr = GaimAccountMngr::getInstance();
	ConnectMngr = GaimConnectMngr::getInstance();
	PresenceMngr = GaimPresenceMngr::getInstance();
	ChatMngr = GaimChatMngr::getInstance();
	ContactListMngr = GaimContactListMngr::getInstance();

	GaimIMInit();
	GaimWrapperInit();
}

void GaimIMFactory::GaimSetCallbacks()
{
	gaim_core_set_ui_ops(&core_wg_ops);
	gaim_eventloop_set_ui_ops(&eventloop_wg_ops);
	gaim_connections_set_ui_ops(&conn_wg_ops);
	gaim_conversations_set_ui_ops(&chat_wg_ops);
	gaim_blist_set_ui_ops(&blist_wg_ops);
	gaim_accounts_set_ui_ops(&acc_wg_ops);
	gaim_privacy_set_ui_ops(&privacy_wg_ops);
}

void GaimIMFactory::GaimWrapperInit()
{
	AccountMngr->Init();
	ConnectMngr->Init();
	PresenceMngr->Init();
	ChatMngr->Init();
	ContactListMngr->Init();
}

void GaimIMFactory::GaimIMInit()
{
	char *search_path;
	char *home_dir;

	if (!g_thread_supported())
		g_thread_init(NULL);

#ifdef OS_WIN32
	wgaim_init(GetModuleHandle(0));
#endif

	std::string configPath;

#if defined(OS_MACOSX) || defined(OS_WINDOWS)
	configPath = Path::getConfigurationDirPath() + File::convertPathSeparators("WengoPhone/");
#else
	configPath = Path::getConfigurationDirPath() + File::convertPathSeparators(".wengophone/");
#endif

	home_dir = g_build_filename(configPath.c_str(), "gaim", NULL);
	File::createPath(home_dir + File::getPathSeparator());
	gaim_util_set_user_dir(home_dir);

	GaimSetCallbacks();

	search_path = g_build_filename(Path::getApplicationDirPath().c_str(), "plugins", NULL);
	gaim_plugins_add_search_path(search_path);
	gaim_plugins_add_search_path("plugins");
	g_free(search_path);

	signal(SIGPIPE, sigpipe_catcher);

	if (!gaim_core_init("Wengo GAIM"))
	{
		fprintf(stderr, "Initialization of the Gaim core failed\n");
	}

	g_thread_create(GaimMainEventLoop, NULL, FALSE, NULL);
}

IMConnect *GaimIMFactory::createIMConnect(IMAccount &account)
{
	return ConnectMngr->AddIMConnect(account);
}

IMChat *GaimIMFactory::createIMChat(IMAccount &account)
{
	return ChatMngr->AddIMChat(account);
}

IMPresence *GaimIMFactory::createIMPresence(IMAccount &account)
{
	return PresenceMngr->AddIMPresence(account);
}

IMContactList *GaimIMFactory::createIMContactList(IMAccount &account)
{
	return ContactListMngr->AddIMContactList(account);
}

