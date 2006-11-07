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

#include "DtmfTheme.h"

#include <model/WengoPhone.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/phoneline/IPhoneLine.h>

#include <util/File.h>
#include <sound/Sound.h>
#include <sound/AudioDevice.h>

#include <tinyxml.h>
#include <util/Logger.h>

DtmfTheme::DtmfTheme(WengoPhone & wengoPhone, std::string repertory, std::string xmlDescriptor)
	: _wengoPhone(wengoPhone), _repertory(repertory) {

	_dialpadMode = DialpadMode::plain;
	_name = "";
	_imageFile = "";

	_xmlDescriptor = _repertory + xmlDescriptor;
	TiXmlDocument doc;
	doc.LoadFile(_xmlDescriptor);

	TiXmlHandle docHandle(&doc);

	//extract info from the dialpad element
	TiXmlElement * dialpadElt = docHandle.FirstChild("dialpad").Element();
	if( dialpadElt ) {

		const char * attr = dialpadElt->Attribute("mode");
		if(attr) {
			if( std::string(attr) == "plain") {
				_dialpadMode = DialpadMode::plain;
			} else if( std::string(attr) == "iconified") {
				_dialpadMode = DialpadMode::iconified;
			} else if( std::string(attr) == "svg") {
				_dialpadMode = DialpadMode::svg;
			}
			else {
				_dialpadMode = DialpadMode::unknown;
			}
		}

		attr = dialpadElt->Attribute("name");
		if(attr) {
			_name = std::string(attr);
		}

		attr = dialpadElt->Attribute("pixmap");
		if(attr) {
			_imageFile = std::string(attr);
		}
	}

	TiXmlElement * tonesNode = dialpadElt->FirstChildElement("tones");
	if( tonesNode ) {

		//iterate over <tone>
		TiXmlElement * toneElt = tonesNode->FirstChildElement("tone");
		while(toneElt) {

			std::string key = "";
			std::string soundFile = "";
			std::string text = "";
			std::string imageFile = "";
			Tone::Action localAction = Tone::Play;
			Tone::Action remoteAction = Tone::Play;
			Tone::AudioFormat audioFormat = Tone::Raw;

			//extract the attribut key, sound_file & audioformat
			const char * attr = toneElt->Attribute("key");
			if(attr) {
				key = std::string(attr);
			}

			attr = toneElt->Attribute("format");
			if(attr) {
				if( std::string(attr) == "raw") {
					audioFormat = Tone::Raw;
				} else if( std::string(attr) == "wav") {
					audioFormat = Tone::Wav;
				} else {
					audioFormat = Tone::Unknown;
				}

				soundFile = "sound_" + key + "." + std::string(attr);
			}

			//local, remote, ...
			attr = toneElt->Attribute("local");
			if(attr) {
				if( std::string(attr) == "play") {
					localAction = Tone::Play;
				} else {
					localAction = Tone::None;
				}
			}

			attr = toneElt->Attribute("remote");
			if(attr) {
				if( std::string(attr) == "play") {
					remoteAction = Tone::Play;
				} else {
					remoteAction = Tone::None;
				}
			}

			attr = toneElt->Attribute("image_file");
			if(attr) {
				imageFile = std::string(attr);
			}

			attr = toneElt->Attribute("text");
			if(attr) {
				text = std::string(attr);
			}

			//the minimum to have a valid Tone
			if( ( key != "") || ( audioFormat == Tone::Unknown ) ){
				Tone * tone = new Tone(key, soundFile, text, imageFile, localAction, remoteAction, audioFormat);
				_toneList[key] = tone;
			}

			toneElt = toneElt->NextSiblingElement("tone");
		}
	}
}

DtmfTheme::~DtmfTheme() {
}

std::string DtmfTheme::getName() const {
	return _name;
}

void DtmfTheme::playTone(const std::string & key) const {

	const Tone * tone = getTone(key);

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string soundfile = _repertory + tone->getSoundFile();

	if( tone->getLocalAction() == Tone::Play ) {
		Sound::play(File::convertPathSeparators(soundfile), config.getAudioRingerDeviceId());
	}

	if( tone->getRemoteAction() == Tone::Play ) {
		//TODO: sendsoundfile
		//int cid = _wengoPhone.getCurrentUserProfile().getActivePhoneLine().getActivePhoneCall();
		//_wengoPhone.getCurrentUserProfile().getActivePhoneLine().playSoundFile(tone->getSoundFile());
	}
}

std::list<std::string> DtmfTheme::getToneList() const {

	std::list<std::string> toReturn;

	DtmfTheme::ToneList::const_iterator it;
	for(it = _toneList.begin(); it != _toneList.end(); it++) {
		toReturn.push_back((*it).first);
	}

	return toReturn;
}

const Tone * DtmfTheme::getTone(const std::string & key) const {

	DtmfTheme::ToneList::const_iterator it;
	for(it = _toneList.begin(); it != _toneList.end(); it++) {

		if( (*it).first == key ) {
			return (*it).second;
		}
	}
	return NULL;
}


std::string DtmfTheme::getImageFile() const {
	return _imageFile;
}

DtmfTheme::DialpadMode DtmfTheme::getDialpadMode() const {
	return _dialpadMode;
}

std::string DtmfTheme::getRepertory() const {
	return _repertory;
}