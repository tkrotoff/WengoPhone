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

#include <QtGui/QtGui>

ToolTipLineEdit::ToolTipLineEdit(QWidget * parent)
	: QLineEdit(parent) {

	_cleared = false;
	_originalPalette = palette();

	//Grey color
	_greyPalette.setColor(QPalette::Text, Qt::gray);

	_toolTipTextDone = false;
}

void ToolTipLineEdit::setText(const QString & text) {
	if (!_toolTipTextDone) {
		_toolTipTextDone = true;
		_toolTip = text;
		setToolTipText();
	} else {
		clearLineEdit();
		QLineEdit::setText(text);
	}
}

QString ToolTipLineEdit::text() const {
	if (QLineEdit::text() == _toolTip) {
		return QString::null;
	} else {
		return QLineEdit::text();
	}
}

void ToolTipLineEdit::setToolTipText() {
	//Text color is grey
	setPalette(_greyPalette);

	QLineEdit::setText(_toolTip);
	_cleared = false;
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

void ToolTipLineEdit::leaveEvent(QEvent * event) {
	if (text().isEmpty()) {
		setToolTipText();
	}
	QLineEdit::leaveEvent(event);
}
