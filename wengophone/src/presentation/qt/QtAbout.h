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

#ifndef QTABOUT_H
#define QTABOUT_H

#include <util/NonCopyable.h>

class QWidget;
class QDialog;
namespace Ui { class AboutWindow; }

/**
 * About dialog window: shows copyright, license, authors...
 *
 * @author Tanguy Krotoff
 */
class QtAbout : NonCopyable {
public:

	QtAbout(QWidget * parent);

	QDialog * getWidget() const {
		return _aboutDialog;
	}

private:

	Ui::AboutWindow * _ui;

	QDialog * _aboutDialog;
};

#endif	//QTABOUT_H
