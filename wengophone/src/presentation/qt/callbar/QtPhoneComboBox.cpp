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

#include "QtPhoneComboBox.h"

#include <util/SafeDelete.h>
#include <util/Logger.h>

#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

QtPhoneComboBox::QtPhoneComboBox(QWidget * parent)
	: QComboBox(parent) {

	_cleared = false;
	_originalPalette = palette();

	//Grey color
	_greyPalette.setColor(QPalette::Text, Qt::gray);

	setLineEdit(new QtPhoneComboBoxLineEdit(this));

	SAFE_CONNECT(this, SIGNAL(activated(const QString &)), SLOT(comboBoxActivated()));
}

QtPhoneComboBox::~QtPhoneComboBox() {
}

void QtPhoneComboBox::setEditText(const QString & text) {
	static bool already = true;

	if (already) {
		already = false;
		_toolTip = text;
		setToolTipText();
	} else {
		clearLineEdit();
		QComboBox::setEditText(text);
	}
}

QString QtPhoneComboBox::currentText() const {
	if (QComboBox::currentText() == _toolTip) {
		return QString::null;
	} else {
		return QComboBox::currentText();
	}
}

void QtPhoneComboBox::setToolTipText() {
	//Text color is grey
	lineEdit()->setPalette(_greyPalette);

	QComboBox::setEditText(_toolTip);
	_cleared = false;
}

void QtPhoneComboBox::clearLineEdit() {
	//Text color back to original color
	lineEdit()->setPalette(_originalPalette);

	clearEditText();
	_cleared = true;
}

void QtPhoneComboBox::mousePressEvent(QMouseEvent * event) {
	clearLineEdit();
	QComboBox::mousePressEvent(event);
}

void QtPhoneComboBox::keyPressEvent(QKeyEvent * event) {
	if (!_cleared) {
		clearLineEdit();
	}
	QComboBox::keyPressEvent(event);
}

void QtPhoneComboBox::leaveEvent(QEvent * event) {
	if (currentText().isEmpty()) {
		setToolTipText();
	}
	QComboBox::leaveEvent(event);
}

void QtPhoneComboBox::comboBoxActivated() {
	//Text color back to original color
	lineEdit()->setPalette(_originalPalette);
}


QtPhoneComboBoxLineEdit::QtPhoneComboBoxLineEdit(QtPhoneComboBox * qtPhoneComboBox) {
	_qtPhoneComboBox = qtPhoneComboBox;
}

void QtPhoneComboBoxLineEdit::mousePressEvent(QMouseEvent * event) {
	if (!_qtPhoneComboBox->_cleared) {
		_qtPhoneComboBox->clearLineEdit();
	}
	QLineEdit::mousePressEvent(event);
}

void QtPhoneComboBoxLineEdit::keyPressEvent(QKeyEvent * event) {
	if (!_qtPhoneComboBox->_cleared) {
		_qtPhoneComboBox->clearLineEdit();
	}
	QLineEdit::keyPressEvent(event);
}
