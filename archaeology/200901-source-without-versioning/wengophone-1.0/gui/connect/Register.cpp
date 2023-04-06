/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include "Register.h"

#include "sip/PhApiCallbacks.h"
#include "Softphone.h"
#include "StatusBar.h"
#include "MainWindow.h"
#include "HomePageWidget.h"
#include "config/Config.h"
#include "config/Authentication.h"
#include "config/Codec.h"
#include "config/CodecList.h"
#include "config/AdvancedConfig.h"
#include "config/Audio.h"
#include "config/Video.h"
#include "connect/Connect.h"
#include "util/emit_signal.h"

#include <josua/jmainconfig.h>
#include <phapi.h>

#include <qmessagebox.h>

#include <iostream>
#include <cassert>
using namespace std;

bool Register::_unregisterAsked = false;

Register::Register() {
	phDebugLevel = 0;
	phLogFileName = NULL;

	_isPhApiInitialized = false;

	//Configure phApi (phcfg)
	//configurePhApi(false);
}

Register::~Register() {
	//terminate();
}

void Register::doRegister() {
	cerr << "Register: makes a register request" << endl;
	
	static const int registrationTimeout = 50 * 60;
	QString userName;

	_unregisterAsked = false;

	//Sends Authentication informations to the server
	sendAuthInfoToServer();

	const Authentication & auth = Authentication::getInstance();
	
	
	userName = auth.getDisplayName();
	if (userName.isEmpty())
		userName = auth.getUsername();
		
	if (_PCtoPCmode) {
		_phoneLineId = phAddVline2("guest",
					"sip:guest@guest",
					"",	//registryServer
					"",
					registrationTimeout);
	} else if (!userName.isEmpty() &&
			!auth.getUserId().isEmpty() &&
			!auth.getServer().isEmpty() &&
			!auth.getProxy().isEmpty()) {

		_phoneLineId = phAddVline2(userName,
					auth.getUserId(),
					auth.getServer(),	//registryServer
					auth.getProxy()+":"+auth.getPort(),
					registrationTimeout);
	}

	assert(_phoneLineId != -1 && "phAddVline() failed");
}

void Register::unRegister() {
	cerr << "Register: unregister request" << endl;

	//Makes an unregister when deleting a virtual line
	phDelVline(_phoneLineId);
}

void Register::initPhApi() {
	//So we can re-init phApi
	terminate();

	//Checks if another instance of phApi (=> WengoPhone) is already running
	if (PhApiCallbacks::initPhApi()) {
		QMessageBox::critical(Softphone::getInstance().getActiveWindow(),
			tr("Fatal error"),
			tr("Wengo cannot work properly\nThe SIP port number of Wengo is already being used by another software"),
			tr("Ok"));

		//Exits without calling disconnect
		//Softphone::getInstance().exit(false);
	} else {
		_isPhApiInitialized = true;
	}
}

void Register::terminate() {
	if (_isPhApiInitialized) {
		phTerminate();
	}
	//_isPhApiInitialized = false;
}

void Register::configurePhApi(bool PCtoPCmode, bool httpTunnelEnabled) {
	static const char * INPUT_DEVICE_TAG = "IN=";
	static const char * OUTPUT_DEVICE_TAG  = "OUT=";

	phConfig_t *cfg = phGetConfig();

	//So we can re-init phApi
	terminate();

	_PCtoPCmode = PCtoPCmode;

	//Authentication informations
	configureSipUser();

	//SIP port and NAT configuration
	const AdvancedConfig & advancedConfig = AdvancedConfig::getInstance();
	strncpy(cfg->sipport, advancedConfig.getSipPort(), sizeof(cfg->sipport));
	strcpy(cfg->nattype, advancedConfig.getNat());
	if (advancedConfig.hasEchoCanceller()) {
		cfg->noaec = 0;
	} else {
		cfg->noaec = 1;
	}

	cfg->nodefaultline = 1;  // TELL phIinti to NOT register a line
	cfg->force_proxy = 0;

	//Codec configuration
	CodecList & audioCodecList = Config::getInstance().getAudioCodecList();
	strncpy(cfg->audio_codecs, audioCodecList.toString(), sizeof(cfg->audio_codecs));
	CodecList & videoCodecList = Config::getInstance().getVideoCodecList();
	strncpy(cfg->video_codecs, videoCodecList.toString(), sizeof(cfg->video_codecs));

	//Audio device configuration
	const Audio & audio = Audio::getInstance();
	phChangeAudioDevices(QString("pa:") + INPUT_DEVICE_TAG
			+ QString("").setNum(audio.getInputDeviceId())
			+ " "
			+ OUTPUT_DEVICE_TAG
			+ QString("").setNum(audio.getOutputDeviceId()));
#ifdef ENABLE_VIDEO
	const Video & video = Video::getInstance();
	strcpy(cfg->video_config.video_device, video.getInputDeviceName());
#endif

	//HTTP tunnel/proxy
	//Should be done before phInit()
	if (httpTunnelEnabled) {
		//Local proxy URL
		char * localProxyUrl = NULL;
		if (!advancedConfig.getHttpProxyUrl().isEmpty()) {
			localProxyUrl = (char *) advancedConfig.getHttpProxyUrl().ascii();
		}

		//Local proxy port
		int localProxyPort = 0;
		if (!advancedConfig.getHttpProxyPort().isEmpty()) {
			localProxyPort = advancedConfig.getHttpProxyPort().toInt();
		}

		//Remote proxy server URL = HTTP tunnel
		const Authentication & auth = Authentication::getInstance();
		QString remoteProxyUrl = auth.getHttpTunnelHost();

		//Remote proxy port number
		int remoteProxyPort = 443;
		if (localProxyPort == 0) {
			remoteProxyPort = 80;
		}

		phTunnelConfig(localProxyUrl, localProxyPort,
				remoteProxyUrl, remoteProxyPort,
				advancedConfig.getHttpProxyUserID(), advancedConfig.getHttpProxyUserPassword(),
				0);
	}
	else {
		// FIXME:  We're forcing NAT type to force peridoic send
		// of the OPTIONS message 
		cfg->nat_refresh_time = 15;
		cfg->use_tunnel = 0;
		strncpy(cfg->nattype, "fcone", sizeof(cfg->nattype));
	}

	//Initialized phApi
	//Do it after configure() because otherwise it does not work
	initPhApi();
}

void Register::configureSipUser() {
	phConfig_t *cfg = phGetConfig();
	const Authentication & auth = Authentication::getInstance();

	if (_PCtoPCmode) {
		cerr << "Register: PC to PC mode" << endl;

		strncpy(cfg->identity, "sip:guest@guest", sizeof(cfg->identity));
		strncpy(cfg->proxy, "", sizeof(cfg->proxy));
		cfg->force_proxy = 0;
	} else {
		if (!auth.getUserId().isEmpty()) {
			QString identity = auth.getDisplayName() + " " +
					"<sip:" + auth.getUserId() + "@" + auth.getServer() + ">";
			strncpy(cfg->identity, identity, sizeof(cfg->identity));
		}

		if (!auth.getProxy().isEmpty()) {
			QString proxy = auth.getProxy();
			if (!auth.getPort().isEmpty()) {
				proxy += ":" + auth.getPort();
			}
			strncpy(cfg->proxy, proxy, sizeof(cfg->proxy));
		}
		cfg->force_proxy = auth.isForceRegister();
	}
}

bool Register::sendAuthInfoToServer() {
	const Authentication & auth = Authentication::getInstance();

	if (phAddAuthInfo(auth.getUsername(), auth.getUserId(),
			auth.getPassword(), 0, auth.getRealm()) == 0) {
		return true;
	} else {
		return false;
	}
}

void Register::registerProgress(int /*registerId*/, int status) {
	Softphone & softphone = Softphone::getInstance();

	//No answer from the server
	if (status < 0) {
		softphone.getStatusBar().setSipStatus(false);
		QMessageBox::critical(softphone.getActiveWindow(),
				tr("Registering failed"),
				tr("No answer from the server during the SIP registering process"));
	}

	//Error
	if (status > 0) {
		softphone.getStatusBar().setSipStatus(false);
		/*QMessageBox::critical(softphone.getActiveWindow(),
				tr("Registering failed"),
				tr("Error during the SIP registering process; code error: %1").arg(status));*/
	}

	//Register successfully done
	if (status == 0) {
		softphone.getStatusBar().setSipStatus(true);
#ifdef WIN32
		softphone.getMainWindow().getHomePageWidget()->goToSoftphoneWeb(false);
#endif

		if (!_unregisterAsked) {
			//Emits the signal connected
			Connect & connect = Connect::getInstance();
			emit_signal(&connect, SIGNAL(connected()));
		}
	}

	//Emits the signal registerDone
	Connect & connect = Connect::getInstance();
	emit_signal(&connect, SIGNAL(registerDone()));
}
