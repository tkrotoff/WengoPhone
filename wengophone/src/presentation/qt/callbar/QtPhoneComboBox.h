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

#ifndef OWQTPHONECOMBOBOX_H
#define OWQTPHONECOMBOBOX_H

#include <QtGui/QComboBox>
#include <QtGui/QPalette>
#include <QtCore/QString>

class QtPhoneComboBoxLineEdit;

class QWidget;
class QMouseEvent;
class QKeyEvent;
class QEvent;

/**
 * QComboBox based on ToolTipLineEdit.
 *
 * Permits to add a message like:
 * "Please enter a phone number"
 *
 * FIXME factorize code with ToolTipLineEdit
 *
 * @author Tanguy Krotoff
 */
class QtPhoneComboBox : public QComboBox {
	Q_OBJECT
	friend class QtPhoneComboBoxLineEdit;
public:

	QtPhoneComboBox(QWidget * parent);

	~QtPhoneComboBox();

	QString currentText() const;

public Q_SLOTS:

	void setEditText(const QString & text);

private Q_SLOTS:

	void comboBoxActivated();

private:

	void setToolTipText();

	void clearLineEdit();

	void mousePressEvent(QMouseEvent * event);

	void keyPressEvent(QKeyEvent * event);

	void leaveEvent(QEvent * event);

	bool _cleared;

	QString _toolTip;

	QPalette _originalPalette;

	QPalette _greyPalette;
};

#include <QtGui/QLineEdit>

class QtPhoneComboBoxLineEdit : public QLineEdit {
	Q_OBJECT
public:

	QtPhoneComboBoxLineEdit(QtPhoneComboBox * qtPhoneComboBox);

private:

	void mousePressEvent(QMouseEvent * event);

	void keyPressEvent(QKeyEvent * event);

	QtPhoneComboBox * _qtPhoneComboBox;
};

#endif	//OWQTPHONECOMBOBOX_H
