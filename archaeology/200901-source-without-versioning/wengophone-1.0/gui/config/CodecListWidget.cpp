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

#include "CodecListWidget.h"

#include "MyWidgetFactory.h"
#include "Codec.h"
#include "CodecList.h"

#include <qlistview.h>
#include <qpushbutton.h>

#include <iostream>
using namespace std;

CodecListWidget::CodecListWidget(CodecList & codecList, QWidget * parent)
: QObject(parent), _codecList(codecList) {

	_codecListWidget = MyWidgetFactory::create("CodecListWidgetForm.ui", this, parent);

	//List of Codec
	_codecListView = (QListView *) _codecListWidget->child("codecListView", "QListView");
	_codecListView->setSorting(-1);	//No sorting
	connect(_codecListView, SIGNAL(selectionChanged(QListViewItem *)),
		this, SLOT(selectionChanged(QListViewItem *)));

	//Up button
	_upButton = (QPushButton *) _codecListWidget->child("upButton", "QPushButton");
	connect(_upButton, SIGNAL(clicked()),
		this, SLOT(upButtonClicked()));

	//Down button
	_downButton = (QPushButton *) _codecListWidget->child("downButton", "QPushButton");
	connect(_downButton, SIGNAL(clicked()),
		this, SLOT(downButtonClicked()));

	//Default button
	_defaultButton = (QPushButton *) _codecListWidget->child("defaultButton", "QPushButton");
	connect(_defaultButton, SIGNAL(clicked()),
		this, SLOT(defaultButtonClicked()));

	_codecSelected = NULL;

	//Loads the Codec list
	loadCodecList();
}

void CodecListWidget::loadCodecList() {
	_codecListView->clear();

	for (int i = _codecList.size() - 1; i > -1; i--) {
		const Codec & codec = _codecList[i];

		//Inserts the Codec into the QListView
		new QListViewItem(_codecListView, codec.getName());
	}
}

void CodecListWidget::readPriorities() {
	//Ugly trick: reads the priority once all the items has been inserted
	//priority == itemPos()
	//This has to be done because itemPos() is not incremented by 1.
	//The priorities are not like 1, 2, 3... but something like 16, 32, 64...
	//from a graphical point of vue.
	QListViewItemIterator it(_codecListView);
	while (it.current()) {
		QListViewItem * current = it.current();
		Codec * codec = _codecList.getCodec(current->text(0));
		codec->setPriority(current->itemPos());
		++it;
	}
}

void CodecListWidget::enableButtons(QListViewItem * codecSelected) {
	if (codecSelected->itemAbove()) {
		_upButton->setEnabled(true);
	} else {
		_upButton->setEnabled(false);
	}

	if (codecSelected->itemBelow()) {
		_downButton->setEnabled(true);
	} else {
		_downButton->setEnabled(false);
	}
}

void CodecListWidget::selectionChanged(QListViewItem * codecSelected) {
	_codecSelected = _codecList.getCodec(codecSelected->text(0));
	if (_codecSelected == NULL) {
		return;
	}
	_codecSelected->setPriority(codecSelected->itemPos());

	enableButtons(codecSelected);
}

void CodecListWidget::upButtonClicked() {
	if (_codecSelected == NULL) {
		return;
	}

	//Get QListViewItem
	QListViewItem * codecItem = _codecListView->itemAt(QPoint(0, _codecSelected->getPriority()));
	if (codecItem == NULL) {
		return;
	}

	//New position of the item
	QListViewItem * above = codecItem->itemAbove();
	if (above == NULL) {
		return;
	}
	above->moveItem(codecItem);

	readPriorities();

	enableButtons(codecItem);
}

void CodecListWidget::downButtonClicked() {
	if (_codecSelected == NULL) {
		return;
	}

	//Get QListViewItem
	QListViewItem * codecItem = _codecListView->itemAt(QPoint(0, _codecSelected->getPriority()));
	if (codecItem == NULL) {
		return;
	}

	//New position of the item
	QListViewItem * below = codecItem->itemBelow();
	if (below == NULL) {
		return;
	}
	codecItem->moveItem(below);

	readPriorities();

	enableButtons(codecItem);
}

void CodecListWidget::defaultButtonClicked() {
	_codecList.loadDefaultCodecList();
	loadCodecList();
}

void CodecListWidget::save() {
}
