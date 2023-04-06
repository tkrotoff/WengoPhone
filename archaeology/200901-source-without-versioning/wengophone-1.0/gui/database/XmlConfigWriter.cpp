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

#include "XmlConfigWriter.h"
using namespace database;

#include "config/Config.h"

#include <qdom.h>
#include <qstring.h>

XmlConfigWriter::XmlConfigWriter(const Config & config) {
	QDomDocument doc(Config::DOC_TYPE);
	QDomElement root = doc.createElement(Config::DOC_TYPE);
	doc.appendChild(root);

	//Style
	root.appendChild(createElement(doc, Config::STYLE_TAG, config.getStyle()));

	//Audio
	root.appendChild(XmlAudioWriter(doc, Audio::getInstance()));

	//Language
	root.appendChild(createElement(doc, Config::LANGUAGE_TAG, config.getLanguage()));

	//Advanced configuration settings
	root.appendChild(XmlAdvConfWriter(doc, AdvancedConfig::getInstance()));


	//Video configuration settings
#ifdef ENABLE_VIDEO
	root.appendChild(XmlVideoConfWriter(doc, Video::getInstance()));
#endif

	const AdvancedConfig & advConf = AdvancedConfig::getInstance();

	if (!advConf.getHttpProxyUrl().isEmpty()) {
		root.appendChild(createElement(doc,
				AdvancedConfig::HTTP_PROXY_URL_TAG,
				advConf.getHttpProxyUrl()));
	}

	if (!advConf.getHttpProxyPort().isEmpty()) {
		root.appendChild(createElement(doc,
				AdvancedConfig::HTTP_PROXY_PORT_TAG,
				advConf.getHttpProxyPort()));
	}

	if (!advConf.getHttpProxyUserID().isEmpty()) {
		root.appendChild(createElement(doc,
				AdvancedConfig::HTTP_PROXY_USERID_TAG,
				advConf.getHttpProxyUserID()));
	}

	if (!advConf.getHttpProxyUserPassword().isEmpty()) {
		root.appendChild(createElement(doc,
				AdvancedConfig::HTTP_PROXY_USERPASSWORD_TAG,
				advConf.getHttpProxyUserPassword()));
	}

	_data = doc.toString();
}

/**
 * @param doc xml document
 * @param window audio settings
 * @brief translate windows audio settings into XML data
 * @return XML data for windows audio settings
 */
QDomElement XmlConfigWriter::XmlAudioWriter(QDomDocument & doc, const Audio & audio) {
	QDomElement root = doc.createElement(Audio::AUDIO_TAG);

	//Input path
	QDomElement input = doc.createElement(Audio::INPUT_TAG);
	//input.appendChild(createElement(doc, Audio::IDENTIFIER_TAG, audio.getInputDevice()));
	input.appendChild(createElement(doc, Audio::NAME_TAG, audio.getInputDeviceNameStored()));
	root.appendChild(input);

	//Output path
	QDomElement output = doc.createElement(Audio::OUTPUT_TAG);
	//output.appendChild(createElement(doc, Audio::IDENTIFIER_TAG, audio.getOutputDevice()));
	output.appendChild(createElement(doc, Audio::NAME_TAG, audio.getOutputDeviceNameStored()));
	root.appendChild(output);

	//Ringing path
	QDomElement ringing = doc.createElement(Audio::RINGING_TAG);
	//ringing.appendChild(createElement(doc, Audio::IDENTIFIER_TAG, audio.getRingingDevice()));
	ringing.appendChild(createElement(doc, Audio::NAME_TAG, audio.getRingingDeviceNameStored()));
	root.appendChild(ringing);
	
/*	//sound driver (linux only)
	QDomElement sounddriver = doc.createElement(Audio::SOUNDDRIVER_TAG);
	sounddriver.appendChild(createElement(doc, Audio::SOUNDDRIVER_TAG, audio.getDriver()));
	root.appendChild(sounddriver);
*/
		
	return root;
}

/**
 * @param doc xml document
 * @param advConf advanced configuration settings
 * @brief translate advanced configuration settings into XML data
 * @return XML data for advanced configuration settings
 */
QDomElement XmlConfigWriter::XmlAdvConfWriter(QDomDocument & doc, AdvancedConfig & advConf) {
	QDomElement root;

	root = doc.createElement(AdvancedConfig::ADVANCEDCONFIG_TAG);
	
	//auto start
	if (advConf.getAutoStart()) {
		root.appendChild(createElement(doc,
				AdvancedConfig::AUTOSTART_TAG,
				"true"));
	} else {
		root.appendChild(createElement(doc,
				AdvancedConfig::AUTOSTART_TAG,
				"false"));
	}

	//Port
	root.appendChild(createElement(doc,
				AdvancedConfig::SIPPORT_TAG,
				advConf.getSipPort()));

	//Nat
	root.appendChild(createElement(doc,
				AdvancedConfig::NAT_TAG,
				advConf.getNat()));

	//EchoCanceller
	if (advConf.hasEchoCanceller()) {
		root.appendChild(createElement(doc,
				AdvancedConfig::ECHO_CANCELLER_TAG,
				"true"));
	} else {
		root.appendChild(createElement(doc,
				AdvancedConfig::ECHO_CANCELLER_TAG,
				"false"));
	}

	//HttpProxy
	if (advConf.isHttpProxyUsed()) {
		root.appendChild(createElement(doc,
				AdvancedConfig::USE_HTTP_PROXY_TAG,
				"true"));
	} else {
		root.appendChild(createElement(doc,
				AdvancedConfig::USE_HTTP_PROXY_TAG,
				"false"));
	}

	//Proxy authentication
	if (advConf.isProxyAuthentication()) {
		root.appendChild(createElement(doc,
						 AdvancedConfig::USE_PROXY_AUTHENTICATION,
						 "true"));
	} else {
		root.appendChild(createElement(doc,
						 AdvancedConfig::USE_PROXY_AUTHENTICATION,
						 "false"));
	}
	
	//use SSL
	if (advConf.useSSL()) {
		root.appendChild(createElement(doc,
						 AdvancedConfig::USE_SSL,
						 "true"));
	} else {
		root.appendChild(createElement(doc,
						 AdvancedConfig::USE_SSL,
						 "false"));
	}
	
	//Main Window size & position
	root.appendChild(createElement(doc,
				AdvancedConfig::MAINWINDOWX_TAG,
				QString::number(advConf.getWindowX())));
	root.appendChild(createElement(doc,
				AdvancedConfig::MAINWINDOWY_TAG,
				QString::number(advConf.getWindowY())));
	root.appendChild(createElement(doc,
				AdvancedConfig::MAINWINDOWWIDTH_TAG,
				QString::number(advConf.getWindowWidth())));
	root.appendChild(createElement(doc,
				AdvancedConfig::MAINWINDOWHEIGHT_TAG,
				QString::number(advConf.getWindowHeight())));
	
	return root;
}

/**
 * @param doc xml document
 * @param video video configuration settings
 * @brief translate video configuration settings into XML data
 * @return XML data for video configuration settings
 */
QDomElement XmlConfigWriter::XmlVideoConfWriter(QDomDocument & doc, const Video & videoConf) {
	QDomElement root;
#ifdef ENABLE_VIDEO
	root = doc.createElement(Video::VIDEO_TAG);

	//FrameRate
	root.appendChild(createElement(doc,
				Video::FRAMERATE_TAG,
				videoConf.getFrameRate()));

	//Quality
	root.appendChild(createElement(doc,
				Video::QUALITY_TAG,
				videoConf.getQuality()));

	//Resolution
	root.appendChild(createElement(doc,
				Video::RESOLUTION_TAG,
				videoConf.getCaptureResolution()));

    //Device
	root.appendChild(createElement(doc,
				Video::INPUTDEVICE_TAG,
				videoConf.getInputDeviceName()));

	root.appendChild(createElement(doc,
				Video::CONNECTIONTYPE_TAG,
				videoConf.getConnectionType()));
	//Flip
	if(videoConf.getFlip()) {
		root.appendChild(createElement(doc,
					Video::FLIP_TAG,
					"true"));
	} else {
		root.appendChild(createElement(doc,
					Video::FLIP_TAG,
					"false"));
	}
	
	//Enable
	if(videoConf.getEnable()) {
		root.appendChild(createElement(doc,
					Video::VIDEOENABLE_TAG,
					"true"));
	} else {
		root.appendChild(createElement(doc,
					Video::VIDEOENABLE_TAG,
					"false"));
	}
	
#endif
	return root;
}


/**
 * @param doc xml document
 * @tag name tag
 * @data text of the XML element
 * @brief create a XML element and insert it in the DOM tree
 * @return a XML element
 */
QDomElement XmlConfigWriter::createElement(QDomDocument & doc, const QString & tag, const QString & data) {
	QDomElement element = doc.createElement(tag);
	QDomText text = doc.createTextNode(data);
	element.appendChild(text);
	return element;
}

/**
 * @param doc xml document
 * @tag name tag
 * @data text of the XML element
 * @brief create a XML element and insert it in the DOM tree
 * @return a XML element
 */
QDomElement XmlConfigWriter::createElement(QDomDocument	& doc, const QString & tag, const int data) {
	QDomElement element = doc.createElement(tag);
	QDomText text = doc.createTextNode(QString("%1").arg(data));
	element.appendChild(text);
	return element;
}
