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

#ifndef OWTOOLTIPLINEEDIT_H
#define OWTOOLTIPLINEEDIT_H

#include <QtGui/QLineEdit>
#include <QtGui/QPalette>
#include <QtCore/QString>

class QWidget;
class QMouseEvent;
class QKeyEvent;
class QEvent;

/**
 * QLineEdit with a tooltip that disappears when the user clicks on it.
 *
 * @see QLineEdit
 * @author Tanguy Krotoff
 */
class ToolTipLineEdit : public QLineEdit {
	Q_OBJECT
public:

	ToolTipLineEdit(QWidget * parent);

	/**
	 * @see QLineEdit::text()
	 */
	QString text() const;

public Q_SLOTS:

	/**
	 * @see QLineEdit::setText()
	 */
	void setText(const QString & text);

	void languageChanged();

private:

	/**
	 * Initializes the ToolTopLineEdit.
	 */
	void init();

	void setToolTipText();

	void clearLineEdit();

	void mousePressEvent(QMouseEvent * event);

	void keyPressEvent(QKeyEvent * event);

	void leaveEvent(QEvent * event);

	bool _cleared;

	/** Tool tip text already set? */
	bool _toolTipTextDone;

	QString _toolTip;

	QPalette _originalPalette;

	QPalette _greyPalette;
};

#endif	//OWTOOLTIPLINEEDIT_H
