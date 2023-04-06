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

#include "VisioPopupMenu.h"

#include "AudioCallManager.h"
#include "AudioCall.h"

#include <qlistview.h>
#include <qpoint.h>
#include <qwidgetfactory.h>
#include <qdialog.h>
#include <qstring.h>

#include <iostream>
using namespace std;

VisioPopupMenu::VisioPopupMenu(QWidget * parent, VisioOutputWidget *visio)
: QPopupMenu(parent) {
	_parent = parent;
	_visio = visio;
	/*
	menu_id_scale_1x = insertItem(tr("Scale 1x"), this,
			SLOT(scale1x()));
	menu_id_scale_2x = insertItem(tr("Scale 2x"), this,
			SLOT(scale2x()));
	menu_id_scale_3x = insertItem(tr("Scale 3x"), this,
			SLOT(scale3x()));
	*/
	insertItem(tr("Respect ratio"));
	connect(this, SIGNAL( activated ( int ) ), _visio, SLOT( repack(int ) ) );
}

void VisioPopupMenu::slotScale1x() {
	/*cout << "SLOT SCALE" << endl;
	_visio->resize(_visio->base_width, _visio->base_height);*/
}

void VisioPopupMenu::slotScale2x() {
	//_visio->resize(_visio->base_width * 2, _visio->base_height * 2);
}

void VisioPopupMenu::slotScale3x() {
	//_visio->resize(_visio->base_width * 3, _visio->base_height * 3);
}

void VisioPopupMenu::showPopup(const QPoint & p) {
	this->popup(p);
}
