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

#include "XmlConfigReader.h"
using namespace database;

#include "config/Config.h"
#include "config/AdvancedConfig.h"
#include "config/Video.h"
#include "config/Audio.h"
#include <AudioDevice.h>

#include <qobject.h>
#include <qdom.h>
#include <qstring.h>
#include <qmessagebox.h>

#include <memory>
#include <iostream>
using namespace std;

XmlConfigReader::XmlConfigReader(const QString & data)
: _config(Config::getInstance()) {

	QDomDocument doc(Config::DOC_TYPE);
	if (!doc.setContent(data)) {
		return;
	}

	//Print out the element names of all elements that are direct
	//children of the outermost element
	QDomElement docElem = doc.documentElement();

	AdvancedConfig & advConf = AdvancedConfig::getInstance();

	QDomNode node = docElem.firstChild();
	while (!node.isNull()) {
		//Try to convert the node to an element
		QDomElement element = node.toElement();

		if (!element.isNull()) {
			// The node really is an element

			QString tag(element.tagName());
			if (tag == Config::STYLE_TAG) {
				_config.setStyle(element.text());
			}
			if (tag == Config::LANGUAGE_TAG) {
				_config.setLanguage(element.text());
			}
			if (tag == AdvancedConfig::ADVANCEDCONFIG_TAG) {
				XmlAdvConfReader(element.toElement());
			}
#ifdef ENABLE_VIDEO
			if (tag == Video::VIDEO_TAG) {
				XmlVideoConfReader(element.toElement());
			}
#endif
			if (tag == Audio::AUDIO_TAG) {
				XmlAudioReader(element.toElement());
			}
			if (tag == AdvancedConfig::HTTP_PROXY_URL_TAG) {
				advConf.setHttpProxyUrl(element.text());
			}
			if (tag == AdvancedConfig::USE_PROXY_AUTHENTICATION) {
				advConf.setProxyAuthentication(element.text());
			}
			if (tag == AdvancedConfig::HTTP_PROXY_PORT_TAG) {
				advConf.setHttpProxyPort(element.text());
			}
			if (tag == AdvancedConfig::HTTP_PROXY_USERID_TAG) {
				advConf.setHttpProxyUserID(element.text());
			}
			if (tag == AdvancedConfig::HTTP_PROXY_USERPASSWORD_TAG) {
				advConf.setHttpProxyUserPassword(element.text());
			}
		}
		node = node.nextSibling();
	}
}

/**
* @param element XML node
* @brief load audio settings for windows from a XML file
*/
void XmlConfigReader::XmlAudioReader(QDomElement element) {
	Audio & audio = Audio::getInstance();
	QDomNode node;
	QDomElement elem;

	node = element.firstChild();
	while (!node.isNull()) {
		elem = node.toElement();
		if (elem.tagName() == Audio::INPUT_TAG) {
			XmlInputReader(elem.toElement());
		}
		if (elem.tagName() == Audio::OUTPUT_TAG) {
			XmlOutputReader(elem.toElement());
		}
		if (elem.tagName() == Audio::RINGING_TAG) {
			XmlRingingReader(elem.toElement());
		}
		audio.storeSettings();
		node = node.nextSibling();
	}
}

/**
* @param element XML node
* @brief load input audio settings
*/
void XmlConfigReader::XmlInputReader(QDomElement element) {
	Audio & audio = Audio::getInstance();
	QDomNode node;
	QDomElement elem;

	node = element.firstChild();
	while (!node.isNull()) {
		elem = node.toElement();
		if (elem.tagName() == Audio::NAME_TAG) {
			if( elem.text() != "" ) {
			    audio.setInputDevice(elem.text());
			} else {
				StringList l = AudioDevice::getInputMixerDeviceList();
				if( l.size() >= 1 ) {
					audio.setInputDevice(l[0]);
				}
			}
		}
		/*if (elem.tagName() == Audio::IDENTIFIER_TAG)
			audio.setInputDevice(elem.text().toInt());*/
		node = node.nextSibling();
	}
}

/**
* @param element XML node
* @brief load output audio settings
*/
void XmlConfigReader::XmlOutputReader(QDomElement element) {
	Audio & audio = Audio::getInstance();
	QDomNode node;
	QDomElement elem;

	node = element.firstChild();
	while (!node.isNull()) {
		elem = node.toElement();
		if (elem.tagName() == Audio::NAME_TAG) {
			if( elem.text() != "" ) {
				audio.setOutputDevice(elem.text());
			} else {
				StringList l = AudioDevice::getOutputMixerDeviceList();
				if( l.size() >= 1 ) {
					audio.setOutputDevice(l[0]);
				}
			}
		}
		/*if (elem.tagName() == Audio::IDENTIFIER_TAG)
			audio.setOutputDevice(elem.text().toInt());*/
		node = node.nextSibling();
	}
}

/**
* @param element XML node
* @brief load ringing audio settings
*/
void XmlConfigReader::XmlRingingReader(QDomElement element) {
	Audio & audio = Audio::getInstance();
	QDomNode node;
	QDomElement elem;

	node = element.firstChild();
	while (!node.isNull()) {
		elem = node.toElement();
		if (elem.tagName() == Audio::NAME_TAG) {
			if( elem.text() != "" ) {
				audio.setRingingDevice(elem.text());
			} else {
				StringList l = AudioDevice::getOutputMixerDeviceList();
				if( l.size() >= 1 ) {
					audio.setRingingDevice(l[0]);
				}
			}
		}
		/*if (elem.tagName() == Audio::IDENTIFIER_TAG)
			audio.setRingingDevice(elem.text().toInt());*/
		node = node.nextSibling();
	}
}

void XmlConfigReader::XmlAdvConfReader(QDomElement element) {
	QDomNode node;
	QDomElement elem;
	AdvancedConfig & adv_conf = AdvancedConfig::getInstance();

	node = element.firstChild();
	while (!node.isNull()) {
		elem = node.toElement();
		if (elem.tagName() == AdvancedConfig::SIPPORT_TAG) {
			adv_conf.setSipPort(elem.text());
		}
		if (elem.tagName() == AdvancedConfig::NAT_TAG) {
			adv_conf.setNat(elem.text());
		}
		if (elem.tagName() == AdvancedConfig::ECHO_CANCELLER_TAG) {
			adv_conf.setEchoCanceller(elem.text() == "true");
		}
		if (elem.tagName() == AdvancedConfig::USE_HTTP_PROXY_TAG) {
			adv_conf.setUseHttpProxy(elem.text() == "true");
		}
		if (elem.tagName() == AdvancedConfig::USE_PROXY_AUTHENTICATION) {
			adv_conf.setProxyAuthentication(elem.text() == "true");
		}
		if (elem.tagName() == AdvancedConfig::USE_SSL) {
			adv_conf.setSSL(elem.text() == "true");
		}
		
		if (elem.tagName() == AdvancedConfig::MAINWINDOWX_TAG) {
			adv_conf.setWindowX(elem.text().toInt());
		}
		if (elem.tagName() == AdvancedConfig::MAINWINDOWY_TAG) {
			adv_conf.setWindowY(elem.text().toInt());
		}
		if (elem.tagName() == AdvancedConfig::MAINWINDOWWIDTH_TAG) {
			adv_conf.setWindowWidth(elem.text().toInt());
		}
		if (elem.tagName() == AdvancedConfig::MAINWINDOWHEIGHT_TAG) {
			adv_conf.setWindowHeight(elem.text().toInt());
		}
		if (elem.tagName() == AdvancedConfig::AUTOSTART_TAG) {
			adv_conf.setAutoStart(elem.text() == "true");
		}
		
		node = node.nextSibling();
	}
	if (!adv_conf.isValid()) {
		adv_conf.defaultSettings();
	}
}

void XmlConfigReader::XmlVideoConfReader(QDomElement element) {
#ifdef ENABLE_VIDEO
	QDomNode node;
	QDomElement elem;
	Video & video_conf = Video::getInstance();

	node = element.firstChild();
	while (!node.isNull()) {
		elem = node.toElement();
		if (elem.tagName() == Video::FRAMERATE_TAG) {
			video_conf.setFrameRate(elem.text());
		}
		if (elem.tagName() == Video::QUALITY_TAG) {
			video_conf.setQuality(elem.text());
		}
		if (elem.tagName() == Video::INPUTDEVICE_TAG) {
			video_conf.setInputDevice(elem.text());
		}
		if (elem.tagName() == Video::FLIP_TAG) {
			video_conf.setFlip(elem.text() == "true");
		}
		if (elem.tagName() == Video::VIDEOENABLE_TAG) {
			video_conf.setEnable(elem.text() == "true");
		}
		if (elem.tagName() == Video::RESOLUTION_TAG) {
			video_conf.setCaptureResolution(elem.text());
		}
		if (elem.tagName() == Video::CONNECTIONTYPE_TAG) {
			video_conf.setConnectionType(elem.text());
		}
		node = node.nextSibling();
	}
	if (!video_conf.isValid()) {
		video_conf.defaultSettings();
	}
#endif
}

