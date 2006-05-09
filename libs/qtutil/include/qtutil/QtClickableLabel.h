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

#ifndef QTCLICKABLELABLE_H
#define QTCLICKABLELABLE_H

#include <QtGui>

/**
 *
 * @author Mr K
 * @author Mathieu Stute
 */
class QtClickableLabel : public QLabel {
Q_OBJECT
public:

	QtClickableLabel( QWidget * parent = 0, Qt::WFlags f = 0 );

	void setSelected(bool value);

	void setSelectedBgColor(const QColor & bgcolor);

protected:

	virtual void mousePressEvent ( QMouseEvent * e );

	virtual void mouseReleaseEvent ( QMouseEvent * e );

	QColor _bgcolor;

	bool _mousePressed;

Q_SIGNALS:

	void clicked();

	void clicked(const QString & text);

	void clicked(QtClickableLabel * instance);

	void rightClicked();

	void rightClicked(const QString & text);

	void rightClicked(QtClickableLabel * instance);
};

#endif
