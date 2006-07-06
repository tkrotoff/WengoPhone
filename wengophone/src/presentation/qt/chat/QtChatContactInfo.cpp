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

#include "QtChatContactInfo.h"

#include <QtGui>

QtChatContactInfo::QtChatContactInfo() {
	_isNull = true;
}

QtChatContactInfo::QtChatContactInfo(const QString & bgColor, const QString & textColor,
	const QString & nickName) {
	_bgColor = bgColor;
	_textColor = textColor;
	_nickName = nickName;
	_isNull = false;
}

QtChatContactInfo::QtChatContactInfo(const QtChatContactInfo & other) {
	_bgColor = other._bgColor;
	_textColor = other._textColor;
	_nickName = other._nickName;
	_isNull = false;
}

QtChatContactInfo::~QtChatContactInfo() {
}

QtChatContactInfo & QtChatContactInfo::operator=(QtChatContactInfo & other) {
	_bgColor = other._bgColor;
	_textColor = other._textColor;
	_nickName = other._nickName;
	_isNull = false;
	return *this;
}

QString QtChatContactInfo::getHeader() const {
	QString header = QString("<table border=0 width=100% cellspacing=0 "
	"cellpadding=3><tr><td BGCOLOR=%1> <font color=%2><b> %3 </b></font></td><td BGCOLOR=%4 align=right>"
	"<font color=%5><b> %6 </b></font></td></tr></table>").
	arg(_bgColor).
	arg(_textColor).
	arg(_nickName).
	arg(_bgColor).
	arg(_textColor).
	arg(QTime::currentTime().toString());
	return header;
}
