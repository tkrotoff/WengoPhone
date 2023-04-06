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

#include "StyleWidget.h"

#include "Config.h"
#include "MyWidgetFactory.h"

#include <qlabel.h>
#include <qlistbox.h>
#include <qstylefactory.h>
#include <qstringlist.h>

#include <iostream>
using namespace std;

StyleWidget::StyleWidget(QWidget * parent) : QObject(parent) {
	_styleWidget = MyWidgetFactory::create("StyleWidgetForm.ui", this, parent);

	QStringList list = QStyleFactory::keys();
	list.sort();
	QListBox * styleListBox = (QListBox *) _styleWidget->child("styleListBox", "QListBox");
	styleListBox->insertStringList(list);

	connect(styleListBox,
		SIGNAL(selectionChanged(QListBoxItem *)), this,
		SLOT(selectionChanged(QListBoxItem *)));
}

StyleWidget::~StyleWidget() {
	delete _styleWidget;
}

void StyleWidget::selectionChanged(QListBoxItem * itemSelected) {
	Config::getInstance().setStyle(itemSelected->text());
}
