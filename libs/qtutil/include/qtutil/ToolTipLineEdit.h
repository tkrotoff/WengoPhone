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

#ifndef TOOLTIPLINEEDIT_H
#define TOOLTIPLINEEDIT_H

#include <QtGui/QLineEdit>
#include <QtGui/QPalette>

class QWidget;
class QMouseEvent;
class QKeyEvent;

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

public Q_SLOTS:

	void setText(const QString & text);

private:

	void mousePressEvent(QMouseEvent * event);

	void keyPressEvent(QKeyEvent * event);

	void clearLineEdit();

	bool _cleared;

	QPalette _originalPalette;
};

#endif	//TOOLTIPLINEEDIT_H
