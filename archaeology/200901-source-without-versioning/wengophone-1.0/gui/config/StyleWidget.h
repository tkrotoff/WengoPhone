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

#ifndef STYLEWIDGET_H
#define STYLEWIDGET_H

#include <qobject.h>

class QWidget;
class QListBoxItem;
class QString;

/**
 * Style component of the ConfigWindow.
 *
 * Selection of the style/theme for the application.
 *
 * @author Tanguy Krotoff
 */
class StyleWidget : public QObject {
	Q_OBJECT
public:

	StyleWidget(QWidget * parent);

	~StyleWidget();

	/**
	 * Gets the low-level widget.
	 *
	 * @return the low-level widget for this gui component
	 */
	QWidget * getWidget() const {
		return _styleWidget;
	}

private slots:

	/**
	 * A new QListBoxItem has been selected.
	 *
	 * @param itemSelected QListBoxItem that has been selected
	 * @see
	 */
	void selectionChanged(QListBoxItem * itemSelected);

private:

	StyleWidget(const StyleWidget &);
	StyleWidget & operator=(const StyleWidget &);

	static void setDefaultWengoStyle();

	/**
	 * The low-level widget of this gui component.
	 */
	QWidget * _styleWidget;

	QString _style;
};

#endif	//STYLEWIDGET_H
