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

#include "QtEmoticonsManager.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

QtEmoticonsManager * QtEmoticonsManager::_instance = NULL;

QtEmoticonsManager::QtEmoticonsManager() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	loadFromFile(QString::fromStdString(config.getResourcesDir() + "emoticons/icondef2.xml"));
}

QtEmoticonsManager * QtEmoticonsManager::getInstance() {
	if (!_instance) {
		_instance = new QtEmoticonsManager();
	}
	return _instance;
}

int QtEmoticonsManager::getProtocolCount() {
	return _protocolsList.size();
}

int QtEmoticonsManager::getEmoticonsCount(QString protocol) {

	if (_protocolsList.find(protocol) == _protocolsList.end()) {
		return 0;
	}
	return _protocolsList[protocol].size();
}

QtEmoticon QtEmoticonsManager::getEmoticon(const QString & text,const QString & protocol) {
	QtEmoticon e;
	QtEmoticonsList emoticonList;
	QtEmoticonsList::iterator it;
	QStringList::iterator si;
	QStringList tlist;

	if (_protocolsList.find(protocol) == _protocolsList.end()) {
		return e;
	}
	emoticonList = _protocolsList[protocol];

	for (it=emoticonList.begin();it!=emoticonList.end();it++) {
		tlist = (*it).getText();
		for (si=tlist.begin();si!=tlist.end();si++) {
			if ((*si).toUpper() == text.toUpper()) {
				return (*it);
			}
		}
	}
	return e;
}

QtEmoticonsManager::QtEmoticonsList QtEmoticonsManager::getQtEmoticonsList(const QString & protocol) {
	QtEmoticonsList emoticonList;
	if (_protocolsList.find(protocol) == _protocolsList.end()) {
		return emoticonList;
	}
	emoticonList = _protocolsList[protocol];
	return emoticonList;
}

void QtEmoticonsManager::loadFromFile(QString filename) {
	QFile file(filename);
	QString ErrorMsg;
	int ErrorLine = 0;
	int ErrorCol = 0;
	QDomDocument doc("wengoIcons");

	if (!file.open(QIODevice::ReadOnly)) {
		return;
	}

	if (!doc.setContent(&file,&ErrorMsg,&ErrorLine,&ErrorCol)) {
		file.close();
		return;
	}
	file.close();

	QDomElement tmpElement;
	QDomElement docElem = doc.documentElement();
	QDomNode n = docElem.firstChild();
	QString tagName;

	while(!n.isNull()) {
		tmpElement = n.toElement();
		if(!tmpElement.isNull()) {
			tagName = tmpElement.tagName();
			if (tagName == "protocol") {
				readProtocol(n);
			}
		}
		n = n.nextSibling();
	}
}

void QtEmoticonsManager::readProtocol(QDomNode node) {
	QDomNode n = node.firstChild();
	QDomElement element;
	QString attributeName;
	while (!n.isNull()) {
		if (!n.toElement().isNull()) {
			if (n.toElement().tagName() == "icon") {
				element = node.toElement();
				if (!element.isNull()) {
					attributeName = element.attribute("name");
					readIcon(n,attributeName);
				}
			}
		}
		n = n.nextSibling();
	}
}

void QtEmoticonsManager::readIcon(QDomNode node, QString protocol) {

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	QDomNode n1 = node.firstChild();
	QStringList textList;
	QtEmoticon emoticon;
	QPixmap emoticonPix;
	while (!n1.isNull()) {
		QDomElement e1 = n1.toElement();
		if ( !e1.isNull()){
			if (e1.tagName() == "text") {
				textList << e1.text();
			}
			if (e1.tagName() == "object") {
				QString emoticonPath = QString::fromStdString(config.getResourcesDir() + "emoticons/") + e1.text();
				emoticon.setPath(emoticonPath);
				emoticonPix = QPixmap(emoticonPath);
				emoticon.setPixmap(emoticonPix);
				emoticon.setButtonPixmap(emoticonPix);
			}
		}
		n1 = n1.nextSibling();
	}
	emoticon.setText(textList);
	_protocolsList[protocol] << emoticon;
}

QString QtEmoticonsManager::text2Emoticon(const QString & text, const QString protocol) {

	int pos;
	QtEmoticonsList emoticonList;
	QtEmoticonsList::iterator it;
	QStringList stringList;
	QStringList::iterator si;
	QString ret = text;

	if (_protocolsList.find(protocol) == _protocolsList.end()) {
		return text;
	}
	emoticonList = _protocolsList[protocol];
	for (it = emoticonList.begin(); it != emoticonList.end(); it++) {
		stringList = (*it).getText();
		for (si = stringList.begin(); si != stringList.end(); si++) {
			ret.replace((*si),(*it).getHtml(),Qt::CaseInsensitive);
		}
	}
	return ret;
}

QString QtEmoticonsManager::emoticons2Text(const QString & text, const QString protocol) {
	int pos;
	QtEmoticonsList emoticonList;
	QtEmoticonsList::iterator it;
	QStringList stringList;
	QStringList::iterator si;
	QString ret = text;

	if (_protocolsList.find(protocol) == _protocolsList.end()) {
		return text;
	}
	emoticonList = _protocolsList[protocol];

	for (it = emoticonList.begin(); it != emoticonList.end(); it++) {
		stringList = (*it).getText();
		for (si = stringList.begin(); si != stringList.end(); si++) {
			ret.replace((*it).getHtml(),encode((*si)),Qt::CaseInsensitive);
		}
	}
	return ret;
}
QString QtEmoticonsManager::encode(const QString & text) {

	int size;
	int i;
	size = text.size();
	QByteArray ba;

	for (i=0;i<size;i++) {
		switch(text[i].toAscii()) {
			case '<':
				ba += "&lt;";
				break;
			case '>':
				ba += "&gt;";
				break;
			case '&':
				ba += "&amp;";
				break;
			case '\'':
				ba += "&apos;";
				break;
			case '"':
				ba += "&apos;";
				break;
			default:
				ba += text[i];
		}
	}
	return QString(ba);
}

