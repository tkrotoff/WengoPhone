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

#ifndef VISIOPOPUPMENU_H
#define VISIOPOPUPMENU_H

#include "VisioOutput.h"

#include <qpopupmenu.h>

class QListViewItem;
class QPoint;
class VisioOutputWidget;

/**
 * @author David Ferlier
 * @author Mathieu Stute
 */
class VisioPopupMenu : public QPopupMenu {
	Q_OBJECT
public:

	/**
	 * Constructs a VisioPopupMenu.
	 *
	 * @param parent parent widget
	 */
	VisioPopupMenu(QWidget * parent, VisioOutputWidget *visio);
	
	~VisioPopupMenu() {
	}
	
public slots:
	
	void showPopup(const QPoint &);
	void slotScale1x();
	void slotScale2x();
	void slotScale3x();
	
private:
	
	int menu_id_fullscreen;
	int menu_id_scale_1x;
	int menu_id_scale_2x;
	int menu_id_scale_3x;

	VisioPopupMenu(const VisioPopupMenu &);
	VisioPopupMenu & operator=(const VisioPopupMenu &);

	/**
	 * Parent widget.
	 */
	QWidget * _parent;
	VisioOutputWidget *_visio;
};

#endif	//VISIOPOPUPMENU_H
