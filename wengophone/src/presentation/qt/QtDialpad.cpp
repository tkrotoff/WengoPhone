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

#include "QtDialpad.h"

#include "QtWengoPhone.h"

#include <Object.h>
#include <WidgetFactory.h>

#include <QtGui>

QtDialpad::QtDialpad(QtWengoPhone * qtWengoPhone) : QObject() {
	_qtWengoPhone = qtWengoPhone;

	_dialpadWidget = WidgetFactory::create(":/forms/DialpadWidget.ui", NULL);

	QPushButton * button = NULL;
	button = Object::findChild<QPushButton *>(_dialpadWidget, "oneButton");
	connect(button, SIGNAL(clicked()), SLOT(oneButtonClicked()));

	button = Object::findChild<QPushButton *>(_dialpadWidget, "twoButton");
	connect(button, SIGNAL(clicked()), SLOT(twoButtonClicked()));

	button = Object::findChild<QPushButton *>(_dialpadWidget, "threeButton");
	connect(button, SIGNAL(clicked()), SLOT(threeButtonClicked()));

	button = Object::findChild<QPushButton *>(_dialpadWidget, "fourButton");
	connect(button, SIGNAL(clicked()), SLOT(fourButtonClicked()));

	button = Object::findChild<QPushButton *>(_dialpadWidget, "fiveButton");
	connect(button, SIGNAL(clicked()), SLOT(fiveButtonClicked()));

	button = Object::findChild<QPushButton *>(_dialpadWidget, "sixButton");
	connect(button, SIGNAL(clicked()), SLOT(sixButtonClicked()));

	button = Object::findChild<QPushButton *>(_dialpadWidget, "sevenButton");
	connect(button, SIGNAL(clicked()), SLOT(sevenButtonClicked()));

	button = Object::findChild<QPushButton *>(_dialpadWidget, "eightButton");
	connect(button, SIGNAL(clicked()), SLOT(eightButtonClicked()));

	button = Object::findChild<QPushButton *>(_dialpadWidget, "nineButton");
	connect(button, SIGNAL(clicked()), SLOT(nineButtonClicked()));

	button = Object::findChild<QPushButton *>(_dialpadWidget, "starButton");
	connect(button, SIGNAL(clicked()), SLOT(starButtonClicked()));

	button = Object::findChild<QPushButton *>(_dialpadWidget, "zeroButton");
	connect(button, SIGNAL(clicked()), SLOT(zeroButtonClicked()));

	button = Object::findChild<QPushButton *>(_dialpadWidget, "sharpButton");
	connect(button, SIGNAL(clicked()), SLOT(sharpButtonClicked()));
}

void QtDialpad::buttonClicked(const QString & num) {
	_qtWengoPhone->dialpad(num);
}

void QtDialpad::oneButtonClicked() {
	buttonClicked("1");
}

void QtDialpad::twoButtonClicked() {
	buttonClicked("2");
}

void QtDialpad::threeButtonClicked() {
	buttonClicked("3");
}

void QtDialpad::fourButtonClicked() {
	buttonClicked("4");
}

void QtDialpad::fiveButtonClicked() {
	buttonClicked("5");
}

void QtDialpad::sixButtonClicked() {
	buttonClicked("6");
}

void QtDialpad::sevenButtonClicked() {
	buttonClicked("7");
}

void QtDialpad::eightButtonClicked() {
	buttonClicked("8");
}

void QtDialpad::nineButtonClicked() {
	buttonClicked("9");
}

void QtDialpad::zeroButtonClicked() {
	buttonClicked("0");
}

void QtDialpad::starButtonClicked() {
	buttonClicked("*");
}

void QtDialpad::sharpButtonClicked() {
	buttonClicked("#");
}
