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

#ifndef EMOTICONSWIDGET_H
#define EMOTICONSWIDGET_H

#include <qobject.h>

#include "iconset.h"

class QGridLayout;

/**
 * Emoticons widget component of the Chat session.
 *
 * @author Mathieu Stute
 */
class EmoticonsWidget : public QObject {
	Q_OBJECT
	public:

	/**
	* Constructs the EmoticonsWidget.
	*
	* @param parent parent widget
	*/
	EmoticonsWidget(QObject * parent, bool useTextLabel = true, int nbcol = 5);

   /**
		 * Gets the low-level gui widget of this class.
		 *
		 * @return the low-level widget
	*/
	QWidget * getWidget() const {
		return _emoticonsWidget;
	}

	void setIconset(Iconset iconset);

	Iconset getIconset() {
		return _iconset;
	}

	void update();

	QMimeSourceFactory *getMimeSourceFactory();

	static QString getFirstShortcut(QRegExp);

	static QString regExp2HumanReadable(QRegExp);
	
public slots:

	void setIconset(const QString&);

private slots:

	void clicked();

private:

	void erase();

	/**
		* Low-level widget of this class.
	*/
	QWidget * _emoticonsWidget;

	QGridLayout * _layout;

	Iconset _iconset;

	int _nbcol;

	bool _useTextLabel;

};

#endif  //EMOTICONSWIDGET_H
