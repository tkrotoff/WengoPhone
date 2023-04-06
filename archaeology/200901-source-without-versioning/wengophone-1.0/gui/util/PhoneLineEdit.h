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

#ifndef PHONELINEEDIT_H
#define PHONELINEEDIT_H

#include <qlineedit.h>

class QString;
class QWidget;
class QMouseEvent;
class QKeyEvent;

/**
 * LineEdit with a tooltip that disappears when the user clicks on it.
 *
 * @author Tanguy Krotoff
 */
class PhoneLineEdit : public QLineEdit {
	Q_OBJECT
public:

	PhoneLineEdit(QWidget * parent = 0, const char * name = 0);

	void setHelpText(const QString & helpText);

	QString text() const;

	QString displayText() const;

public slots:

	virtual void setText(const QString & text);

protected:

	virtual void mousePressEvent(QMouseEvent * event);

	virtual void keyPressEvent(QKeyEvent * event);

private:

	bool _helpTextCleared;
};

#endif	//PHONELINEEDIT_H
