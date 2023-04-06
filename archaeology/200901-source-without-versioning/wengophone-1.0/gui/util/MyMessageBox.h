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

#ifndef MYMESSAGEBOX_H
#define MYMESSAGEBOX_H

#include <NonCopyable.h>

#include <qmessagebox.h>

class QString;

/**
 * Wrapper around QMessageBox.
 *
 * Cannot be called MessageBox because of a clash name with
 * a function from the win32 API.
 *
 * @author Tanguy Krotoff
 */
class MyMessageBox : NonCopyable {
public:

	/**
	 * Opens a question message box.
	 *
	 * Example:
	 * <code>
	 * int buttonClicked = MessageBox::question(parent, tr("Wengo"),
	 *                       tr("Do you want to..."), "tr(Yes"), tr("No"));
	 * switch(buttonClicked) {
	 * default:
	 * case QMessageBox::Yes:
	 *    ...
	 *    break;
	 * case QMessageBox::No:
	 *    ...
	 *    break;
	 * }
	 * </code>
	 */
	static int question(QWidget * parent, const QString & caption, const QString & text,
			const QString & yesButtonText, const QString & noButtonText);
};

#endif	//MYMESSAGEBOX_H
