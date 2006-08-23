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
#ifndef OWQTEMOTICONSMANAGER_H
#define OWQTEMOTICONSMANAGER_H

#include "QtEmoticon.h"

#include <QtGui/QtGui>
#include <QtXml/QtXml>

/**
 *
 * @ingroup presentation
 * @author Mr K.
 * @author Mathieu Stute
 */
class QtEmoticonsManager {
public:

	typedef QList<QtEmoticon> QtEmoticonsList;

	typedef QHash<QString, QtEmoticonsList> ProtocolsList;

	static QtEmoticonsManager * getInstance();

	QtEmoticonsManager();

	QtEmoticon getEmoticon(const QString & text, const QString & protocol);

	void loadFromFile(QString filename);

	QString text2Emoticon(const QString & text, const QString protocol);

	QString emoticons2Text(const QString & text, const QString protocol);

	QtEmoticonsList getQtEmoticonsList(const QString & protocol);

	int getProtocolCount();

	int getEmoticonsCount(QString protocol);

private:

	void readProtocol(QDomNode node);

	void readIcon(QDomNode node, QString protocol);

	QString encode(const QString & text);

	ProtocolsList _protocolsList;

	QtEmoticonsList _qtEmoticonsList;

	static QtEmoticonsManager * _instance;
};

#endif //OWQTEMOTICONSMANAGER_H
