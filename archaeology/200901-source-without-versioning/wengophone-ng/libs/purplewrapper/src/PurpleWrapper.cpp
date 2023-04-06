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

#include <purplewrapper/PurpleWrapper.h>

#include <purplewrapper/PurpleEnumIMProtocol.h>

#include <coipmanager_base/EnumAccountType.h>
#include <coipmanager_base/account/Account.h>
#include <coipmanager_base/account/JabberAccount.h>

#include <cutil/global.h>

#ifdef OS_WINDOWS
	#include <winsock2.h>
	#include <windows.h>
#endif

#ifndef CC_MSVC8
extern "C" {
#endif
#include <glib.h>
#include <libpurple/internal.h>
#include <libpurple/core.h>
#include <libpurple/eventloop.h>
#include <libpurple/blist.h>
#include <libpurple/debug.h>
#include <libpurple/util.h>
#include <libpurple/blist.h>
#include <libpurple/conversation.h>
#include <libpurple/account.h>
#include <libpurple/privacy.h>
#ifndef CC_MSVC8
}
#endif

#include <cutil/global.h>
#include <util/File.h>
#include <util/Logger.h>
#include <util/Path.h>
#include <util/SafeDelete.h>
#include <util/String.h>

PurpleWrapper * PurpleWrapper::_instance = NULL;

#define PURPLE_WG_READ_COND  (G_IO_IN | G_IO_HUP | G_IO_ERR)
#define PURPLE_WG_WRITE_COND (G_IO_OUT | G_IO_HUP | G_IO_ERR | G_IO_NVAL)

typedef struct _PurpleWgIOClosure {
	PurpleInputFunction function;
	guint result;
	gpointer data;
} PurpleWgIOClosure;

static void purple_wg_io_destroy(gpointer data) {
	g_free(data);
}

static gboolean purple_wg_io_invoke(GIOChannel * source, GIOCondition condition, gpointer data) {

	PurpleInputCondition purple_cond = (PurpleInputCondition) 0;
	if (condition & PURPLE_WG_READ_COND) {
		purple_cond = (PurpleInputCondition)(purple_cond | PURPLE_INPUT_READ);
	}
	if (condition & PURPLE_WG_WRITE_COND) {
		purple_cond = (PurpleInputCondition)(purple_cond | PURPLE_INPUT_WRITE);
	}

#ifdef OS_WINDOWS
	if (!purple_cond) {
		return TRUE;
	}
#endif /* OS_WINDOWS */

	PurpleWgIOClosure * closure = (PurpleWgIOClosure *) data;
#ifdef OS_WINDOWS
	closure->function(closure->data, g_io_channel_win32_get_fd(source), purple_cond);
#else
	closure->function(closure->data, g_io_channel_unix_get_fd(source), purple_cond);
#endif

	return TRUE;
}

static guint purple_wg_input_add(gint fd, PurpleInputCondition condition,
	PurpleInputFunction function, gpointer data) {

	PurpleWgIOClosure * closure = g_new0(PurpleWgIOClosure, 1);

	closure->function = function;
	closure->data = data;

	GIOCondition cond = (GIOCondition) 0;
	if (condition & PURPLE_INPUT_READ) {
		cond = (GIOCondition)(cond | PURPLE_WG_READ_COND);
	}
	if (condition & PURPLE_INPUT_WRITE) {
		cond = (GIOCondition)(cond | PURPLE_WG_WRITE_COND);
	}

	GIOChannel * channel;
#ifdef OS_WINDOWS
	channel = wpurple_g_io_channel_win32_new_socket(fd);
#else
	channel = g_io_channel_unix_new(fd);
#endif
	closure->result = g_io_add_watch_full(channel, G_PRIORITY_DEFAULT, cond,
		purple_wg_io_invoke, closure, purple_wg_io_destroy);

	g_io_channel_unref(channel);
	return closure->result;
}

static void sigpipe_catcher(int sig) {
	LOG_DEBUG("SIGPIPE caught: " + String::fromNumber(sig));
}

static gpointer purpleMainEventLoop(gpointer /*data*/) {
	GMainLoop * loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(loop);

	g_thread_exit(NULL);
	return NULL;
}

static PurpleCoreUiOps coreUiOps = {
	NULL, // ui_prefs_init
	NULL, // debug_ui_init
	NULL, // ui_init
	NULL, // quit
	NULL, // get_ui_info
	NULL, // _purple_reserved1
	NULL, // _purple_reserved2
	NULL, // _purple_reserved3
};

static PurpleEventLoopUiOps eventloopUiOps = {
	g_timeout_add,
	(gboolean (*)(guint))g_source_remove,
	purple_wg_input_add,
	(gboolean (*)(guint))g_source_remove,
	NULL, // input_get_error
	NULL, // timeout_add_seconds
	NULL, // _purple_reserved2
	NULL, // _purple_reserved3
	NULL, // _purple_reserved4
};

PurpleWrapper & PurpleWrapper::getInstance() {
	if (!_instance) {
		_instance = new PurpleWrapper();
	}

	return *_instance;
}

void * PurpleWrapper::getHandle() {
	static int handle;
	return &handle;
}

void PurpleWrapper::setConnectionCallbacks(PurpleConnectionUiOps * connUiOps) {
	_connUiOps = connUiOps;
}

void PurpleWrapper::setConversationCallbacks(PurpleConversationUiOps * convUiOps) {
	_convUiOps = convUiOps;
}

void PurpleWrapper::setBuddyListCallbacks(PurpleBlistUiOps * blistUiOps) {
	_blistUiOps = blistUiOps;
}

void PurpleWrapper::setAccountCallbacks(PurpleAccountUiOps * accountUiOps) {
	_accountUiOps = accountUiOps;
}

PurpleWrapper::PurpleWrapper() {
	_connUiOps = NULL;
	_convUiOps = NULL;
	_blistUiOps = NULL;
	_isInitialized = FALSE;
}

PurpleWrapper::~PurpleWrapper() {
}

void PurpleWrapper::setConfigPath(const std::string & configPath) {
	_configPath = configPath;
}

void PurpleWrapper::initialize() {
	if (_isInitialized) {
		return;
	}

	if (!g_thread_supported()) {
		g_thread_init(NULL);
	}

	char * user_dir = g_build_filename(_configPath.c_str(), "purple", NULL);

	// Delete the home_dir if exists
	File * userDir = new File(std::string(user_dir));
	userDir->remove();
	OWSAFE_DELETE(userDir);
	////

	File::createPath(user_dir + File::getPathSeparator());
	purple_util_set_user_dir(user_dir);

	setCallbacks();

	char * search_path = g_build_filename(Path::getApplicationDirPath().c_str(), "plugins", NULL);
	purple_plugins_add_search_path(search_path);
	purple_plugins_add_search_path("plugins");
	g_free(search_path);

#ifdef OS_MACOSX
	signal(SIGPIPE, sigpipe_catcher);
#endif

	if (!purple_core_init("Wengo PURPLE")) {
		fprintf(stderr, "Initialization of the LibPurple core failed\n");
	}

	g_thread_create(purpleMainEventLoop, NULL, FALSE, NULL);

	_isInitialized = TRUE;
}

void PurpleWrapper::uninitialize() {
	if (_isInitialized) {
		purple_core_quit();
		_isInitialized = FALSE;
	}
}

void PurpleWrapper::setCallbacks() {
	purple_core_set_ui_ops(&coreUiOps);
	purple_eventloop_set_ui_ops(&eventloopUiOps);
	purple_connections_set_ui_ops(_connUiOps);
	purple_conversations_set_ui_ops(_convUiOps);
	purple_blist_set_ui_ops(_blistUiOps);
	purple_accounts_set_ui_ops(_accountUiOps);
/*	purple_privacy_set_ui_ops(&privacy_wg_ops);
*/
}

std::string PurpleWrapper::cleanLogin(PurpleAccount *pAccount) {
	String result = std::string(purple_account_get_username(pAccount));
	if (pAccount) {
		EnumAccountType::AccountType accountType = PurpleIMPrcl::getEnumIMProtocol(pAccount);
		if ((accountType == EnumAccountType::AccountTypeJabber)
			|| (accountType == EnumAccountType::AccountTypeGTalk)) {
			// LibPurple automatically adds the resource in the username field
			// (e.g: "login/resource" instead of "login")
			result = result.split("/")[0];
		}
	}

	return result;
}

std::string PurpleWrapper::purpleLogin(const Account & account) {
	String result = account.getLogin();
	if ((account.getAccountType() == EnumAccountType::AccountTypeJabber)
		|| (account.getAccountType() == EnumAccountType::AccountTypeGTalk)) {
		// LibPurple automatically needs the resource in the username field
		// (e.g: "login/resource" instead of "login")
		JabberAccount * jabberAccount = static_cast<JabberAccount *>(account.getPrivateAccount());
		result = result + "/" + jabberAccount->getResource();
	}

	return result;
}
