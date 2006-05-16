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

#include <qtutil/ToolTipLineEdit.h>

#include <QtGui>

ToolTipLineEdit::ToolTipLineEdit(QWidget * parent)
	: QLineEdit(parent) {

	_cleared = false;
	_originalPalette = palette();

	//Text color is grey
	QPalette palette;
	palette.setColor(QPalette::Text, Qt::gray);
	setPalette(palette);
}

void ToolTipLineEdit::setText(const QString & text) {
	clearLineEdit();
	QLineEdit::setText(text);
}

void ToolTipLineEdit::clearLineEdit() {
	//Text color back to original color
	setPalette(_originalPalette);

	clear();
	_cleared = true;
}

void ToolTipLineEdit::mousePressEvent(QMouseEvent * event) {
	if (!_cleared) {
		clearLineEdit();
	}
	QLineEdit::mousePressEvent(event);
}

void ToolTipLineEdit::keyPressEvent(QKeyEvent * event) {
	if (!_cleared) {
		clearLineEdit();
	}
	QLineEdit::keyPressEvent(event);
}
