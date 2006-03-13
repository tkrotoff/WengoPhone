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
#ifndef QTEMOTICON_H
#define QTEMOTICON_H

#include <QtGui>

class QtEmoticon
{
public:

	QtEmoticon();
	
	QtEmoticon(const QtEmoticon & source);
	
	~QtEmoticon();
	
	void setText(const QStringList & text) { _text = text;};
	
	void setPath(const QString & path) { _path = path;};
	
	void setPixmap(const QPixmap & pixmap) { _pixmap = pixmap;};
	
	void setButtonPixmap(const QPixmap & pixmap) { _buttonPixmap = pixmap;};
	
	void setRegExp(const QString & regExp);
	
	QString & getRegExp() {return _regExp;};
	
	QStringList & getText() { return _text;};
	
	QString & getDefaultText() { return _text[0];};
	
	QPixmap & getPixmap() { return _pixmap;};
	
	QPixmap & getButtonPixmap() { return _buttonPixmap;};
	
	QString & getPath() { return _path;};
	
	QString getHtmlRegExp();
	
	QString getHtml();
	
	bool isNull();
	
	//bool operator <(QtEmoticon & source) const;
	
protected:
	QString _path;
	QStringList _text;
	QString _regExp;
	QPixmap _pixmap;
	QPixmap _buttonPixmap;
};

#endif
