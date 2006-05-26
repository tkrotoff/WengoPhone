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
#ifndef QTWENGOSTYLELABEL_H
#define QTWENGOSTYLELABEL_H

#include <QtGui>

class QtWengoStyleLabel : public QLabel
{

	Q_OBJECT
public:

	QtWengoStyleLabel ( QWidget * parent = 0, Qt::WFlags f = 0 );

	~QtWengoStyleLabel ( );

	void setPixmaps(const QPixmap & normaleLeftPixmap,
	                const QPixmap & normaleRightPixmap,
	                const QPixmap & normaleFillPixmap,
	                const QPixmap & pressedLeftPixmap,
	                const QPixmap & pressedRightPixmap,
	                const QPixmap & pressedFillPixmap);

	void setTextColor(const QColor & color);

	void setBackgroundColor(const QColor & color);

	void setTextAlignment(int alignment) {_alignment = alignment;};

	void setSelected(bool value) { _selected=value; };

public Q_SLOTS:

    void setText(const QString & text);

Q_SIGNALS:

	void clicked();

	void clicked(const QString & text);

	void clicked(QtWengoStyleLabel * instance);


protected:

	virtual void paintEvent(QPaintEvent * event);

	virtual void resizeEvent ( QResizeEvent * event );

	virtual void drawText(QPainter * painter);

	virtual void mouseMoveEvent ( QMouseEvent * event );

	virtual void mousePressEvent ( QMouseEvent * event );

	virtual void mouseReleaseEvent ( QMouseEvent * event );

	QPixmap  _normalFillPixmap;

	QPixmap  _normalLeftPixmap;

	QPixmap  _normalRightPixmap;

	QPixmap  _pressedFillPixmap;

	QPixmap  _pressedLeftPixmap;

	QPixmap  _pressedRightPixmap;

	QColor _textColor;

	QColor _backgroundColor;

	bool _pressed;

	bool _selected;

	QWidget * _parent;

	int _alignment;

};

#endif
