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

#include "QtCallBar.h"

#include "QtPhoneComboBox.h"

#include <qtutil/WengoStyleLabel.h>
#include <qtutil/MouseEventFilter.h>

#include <QtGui/QtGui>

QtCallBar::QtCallBar(QWidget * parent)
	: QFrame(parent) {

	setFrameShape(QFrame::NoFrame);

	//callButton
	_callButton = new WengoStyleLabel(this);
	_callButton->setPixmaps(
			QPixmap(), //Start
			QPixmap(":/pics/callbar/call_bar_button.png"), //End
			QPixmap(), //Fill

			QPixmap(), //Start
			QPixmap(":/pics/callbar/call_bar_button_on.png"), //End
			QPixmap() //Fill
			);
	_callButton->setMaximumSize(QSize(45, 65));
	connect(_callButton, SIGNAL(clicked()), SLOT(callButtonClickedSlot()));

	//hangUpButton
	_hangUpButton = new WengoStyleLabel(this);
	_hangUpButton->setPixmaps(
			QPixmap(":/pics/callbar/call_bar_button_hangup.png"), //Start
			QPixmap(), //End
			QPixmap(), //Fill

			QPixmap(":/pics/callbar/call_bar_button_hangup_on.png"), //Start
			QPixmap(), //End
			QPixmap() //Fill
			);
	_hangUpButton->setMaximumSize(QSize(25, 65));
	_hangUpButton->setMinimumSize(QSize(25, 65));
	connect(_hangUpButton, SIGNAL(clicked()), SLOT(hangUpButtonClickedSlot()));

	//phoneComboBoxContainerLabel
	WengoStyleLabel * phoneComboBoxContainerLabel = new WengoStyleLabel(this);
	phoneComboBoxContainerLabel->setPixmaps(
			QPixmap(":/pics/callbar/call_bar_start.png"), //Start
			QPixmap(), //End
			QPixmap(":/pics/callbar/call_bar_fill.png"), //Fill

			QPixmap(":/pics/callbar/call_bar_start.png"), //Start
			QPixmap(), //End
			QPixmap(":/pics/callbar/call_bar_fill.png") //Fill
			);
	QGridLayout * comboContainterLayout = new QGridLayout(phoneComboBoxContainerLabel);

	//phoneComboBox
	_phoneComboBox = new QtPhoneComboBox(phoneComboBoxContainerLabel);
	_phoneComboBox->setEditable(true);
	_phoneComboBox->setEditText(tr("+(country code) number or nickname"));
	_phoneComboBox->setMaximumSize(QSize(10000, 22));
	_phoneComboBox->setAutoCompletion(false);
	connect(_phoneComboBox->lineEdit(), SIGNAL(returnPressed()), SLOT(phoneComboBoxReturnPressedSlot()));
	connect(_phoneComboBox, SIGNAL(editTextChanged(const QString &)), SLOT(phoneComboBoxEditTextChangedSlot(const QString &)));
	MousePressEventFilter * leftMouseFilter = new MousePressEventFilter(this, SLOT(phoneComboBoxClickedSlot()), Qt::LeftButton);
	_phoneComboBox->installEventFilter(leftMouseFilter);

	comboContainterLayout->addWidget(_phoneComboBox);

	QGridLayout * layout = new QGridLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	layout->addWidget(phoneComboBoxContainerLabel, 0, 0);
	layout->addWidget(_hangUpButton, 0, 1);
	layout->addWidget(_callButton, 0, 2);
}

QtCallBar::~QtCallBar() {
}

void QtCallBar::setEnabledCallButton(bool enable) {
	_callButton->setEnabled(enable);
}

void QtCallBar::setEnabledHangUpButton(bool enable) {
	_hangUpButton->setEnabled(enable);
}

std::string QtCallBar::getPhoneComboBoxCurrentText() const {
	return _phoneComboBox->currentText().toStdString();
}

void QtCallBar::clearPhoneComboBox() {
	_phoneComboBox->clear();
}

void QtCallBar::clearPhoneComboBoxEditText() {
	_phoneComboBox->clearEditText();
}

void QtCallBar::setPhoneComboBoxEditText(const std::string & text) {
	_phoneComboBox->setEditText(QString::fromStdString(text));
}

void QtCallBar::addPhoneComboBoxItem(const std::string & text) {
	_phoneComboBox->insertItem(0, QString::fromStdString(text));
}

void QtCallBar::callButtonClickedSlot() {
	callButtonClicked();
}

void QtCallBar::hangUpButtonClickedSlot() {
	hangUpButtonClicked();
}

void QtCallBar::phoneComboBoxReturnPressedSlot() {
	phoneComboBoxReturnPressed();
}

void QtCallBar::phoneComboBoxEditTextChangedSlot(const QString & text) {
	phoneComboBoxEditTextChanged(text);
}

void QtCallBar::phoneComboBoxClickedSlot() {
	phoneComboBoxClicked();
}
