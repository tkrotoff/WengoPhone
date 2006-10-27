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

#ifndef OWWENGOSTYLELABEL_H
#define OWWENGOSTYLELABEL_H

#include <qtutil/owqtutildll.h>

#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QColor>

/**
 * TODO Refactoring
 * ClickableSkinnedLabel
 *
 * @author Tanguy Krotoff
 */
class WengoStyleLabel : public QLabel {
	Q_OBJECT
public:

	enum Mode {
		Normal,
		Toggled,
	};

	OWQTUTIL_API WengoStyleLabel(QWidget * parent, Mode = Normal, Qt::AlignmentFlag hAlign = Qt::AlignHCenter);

	OWQTUTIL_API ~WengoStyleLabel();

	OWQTUTIL_API void setPixmaps(const QPixmap & normalLeftPixmap,
			const QPixmap & normalRightPixmap,
			const QPixmap & normalFillPixmap,
			const QPixmap & pressedLeftPixmap,
			const QPixmap & pressedRightPixmap,
			const QPixmap & pressedFillPixmap);

	/*void setLeftPixmaps(const QPixmap & normalRightPixmap, const QPixmap & pressedRightPixmap);

	void setRightPixmaps(const QPixmap & normalRightPixmap, const QPixmap & pressedRightPixmap);*/

	OWQTUTIL_API void setTextColor(const QColor & color);

	//void setBackgroundColor(const QColor & color);

	OWQTUTIL_API void setTextAlignment(int alignment) {
		_alignment = alignment;
	}

	OWQTUTIL_API void setSelected(bool value) {
		_selected = value;
	}

public Q_SLOTS:

	OWQTUTIL_API void setText(const QString & text);

Q_SIGNALS:

	OWQTUTIL_API void clicked();

private:

	void paintEvent(QPaintEvent * event);

	void resizeEvent(QResizeEvent * event);

	void drawText(QPainter * painter);

	void mouseMoveEvent(QMouseEvent * event);

	void mousePressEvent(QMouseEvent * event);

	void mouseReleaseEvent(QMouseEvent * event);

	QPixmap _normalFillPixmap;

	QPixmap _normalLeftPixmap;

	QPixmap _normalRightPixmap;

	QPixmap _pressedFillPixmap;

	QPixmap _pressedLeftPixmap;

	QPixmap _pressedRightPixmap;

	QColor _textColor;

	QColor _backgroundColor;

	bool _pressed;

	bool _selected;

	QWidget * _parent;

	int _alignment;

	bool _toggled;

	Mode _mode;
};

#endif	//OWWENGOSTYLELABEL_H
