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

#include "DialPadWidget.h"

#include "MyWidgetFactory.h"
#include "AudioCallManager.h"
#include "Softphone.h"
#include "config/Audio.h"

#include <Sound.h>

#include <qwidget.h>
#include <qdir.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qstringlist.h>

#include <cassert>

DialPadWidget::DialPadWidget(QWidget * parent, QComboBox & phoneComboBox)
: QObject(parent), _phoneComboBox(phoneComboBox) {

	_dtmfSoundsPath = Softphone::getInstance().getApplicationDirPath() + "sounds/dtmf/";

	_dialPadWidget = MyWidgetFactory::create("DialPadWidgetForm.ui", this, parent);

	_oneButton = (QPushButton *) _dialPadWidget->child("oneButton", "QPushButton");
	_twoButton = (QPushButton *) _dialPadWidget->child("twoButton", "QPushButton");
	_threeButton = (QPushButton *) _dialPadWidget->child("threeButton", "QPushButton");
	_fourButton = (QPushButton *) _dialPadWidget->child("fourButton", "QPushButton");
	_fiveButton = (QPushButton *) _dialPadWidget->child("fiveButton", "QPushButton");
	_sixButton = (QPushButton *) _dialPadWidget->child("sixButton", "QPushButton");
	_sevenButton = (QPushButton *) _dialPadWidget->child("sevenButton", "QPushButton");
	_eightButton = (QPushButton *) _dialPadWidget->child("eightButton", "QPushButton");
	_nineButton = (QPushButton *) _dialPadWidget->child("nineButton", "QPushButton");
	_zeroButton = (QPushButton *) _dialPadWidget->child("zeroButton", "QPushButton");
	_starButton = (QPushButton *) _dialPadWidget->child("starButton", "QPushButton");
	_sharpButton = (QPushButton *) _dialPadWidget->child("sharpButton", "QPushButton");

	_dtmfThemeComboBox = (QComboBox *) _dialPadWidget->child("dtmfThemeComboBox", "QComboBox");
	_dtmfThemeComboBox->insertStringList(getListDtmfTheme());

	//Connections
	connect(_oneButton, SIGNAL(clicked()),
		this, SLOT(oneButtonClicked()));
	connect(_twoButton, SIGNAL(clicked()),
		this, SLOT(twoButtonClicked()));
	connect(_threeButton, SIGNAL(clicked()),
		this, SLOT(threeButtonClicked()));
	connect(_fourButton, SIGNAL(clicked()),
		this, SLOT(fourButtonClicked()));
	connect(_fiveButton, SIGNAL(clicked()),
		this, SLOT(fiveButtonClicked()));
	connect(_sixButton, SIGNAL(clicked()),
		this, SLOT(sixButtonClicked()));
	connect(_sevenButton, SIGNAL(clicked()),
		this, SLOT(sevenButtonClicked()));
	connect(_eightButton, SIGNAL(clicked()),
		this, SLOT(eightButtonClicked()));
	connect(_nineButton, SIGNAL(clicked()),
		this, SLOT(nineButtonClicked()));
	connect(_zeroButton, SIGNAL(clicked()),
		this, SLOT(zeroButtonClicked()));
	connect(_starButton, SIGNAL(clicked()),
		this, SLOT(starButtonClicked()));
	connect(_sharpButton, SIGNAL(clicked()),
		this, SLOT(sharpButtonClicked()));
}

void DialPadWidget::playDTMF(const QString & soundFile) {
	if (_dtmfThemeComboBox->currentItem() == 0 || _dtmfThemeComboBox->currentItem() == 1) {
		Sound::play(_dtmfSoundsPath + soundFile + ".wav", "0");
		if (soundFile[0] == 'x') {
			_phoneComboBox.lineEdit()->insert("*");
			AudioCallManager::getInstance().playDTMF('*');
		} else if (soundFile.contains("sharp")) {
			_phoneComboBox.lineEdit()->insert("#");
			AudioCallManager::getInstance().playDTMF('#');
		} else {
			_phoneComboBox.lineEdit()->insert(soundFile[0]);
			AudioCallManager::getInstance().playDTMF(soundFile[0]);
		}
	} else {
		QString soundFilePath = _dtmfSoundsPath + _dtmfThemeComboBox->currentText() + QDir::separator() + soundFile + ".raw";
		AudioCallManager::getInstance().playDTMFSoundFile(soundFilePath);
		Sound::play(soundFilePath, Audio::getInstance().getRingingDeviceName());
	}
}

void DialPadWidget::oneButtonClicked() {
	playDTMF("1");
}

void DialPadWidget::twoButtonClicked() {
	playDTMF("2");
}

void DialPadWidget::threeButtonClicked() {
	playDTMF("3");
}

void DialPadWidget::fourButtonClicked() {
	playDTMF("4");
}

void DialPadWidget::fiveButtonClicked() {
	playDTMF("5");
}

void DialPadWidget::sixButtonClicked() {
	playDTMF("6");
}

void DialPadWidget::sevenButtonClicked() {
	playDTMF("7");
}

void DialPadWidget::eightButtonClicked() {
	playDTMF("8");
}

void DialPadWidget::nineButtonClicked() {
	playDTMF("9");
}

void DialPadWidget::zeroButtonClicked() {
	playDTMF("0");
}

void DialPadWidget::starButtonClicked() {
	playDTMF("x");
}

void DialPadWidget::sharpButtonClicked() {
	playDTMF("sharp");
}

std::vector<QPushButton *> DialPadWidget::getNumberButtons() const {
	std::vector<QPushButton *> res;

	res.push_back(_oneButton);
	res.push_back(_twoButton);
	res.push_back(_threeButton);
	res.push_back(_fourButton);
	res.push_back(_fiveButton);
	res.push_back(_sixButton);
	res.push_back(_sevenButton);
	res.push_back(_eightButton);
	res.push_back(_nineButton);
	res.push_back(_zeroButton);
	return res;
}

QPushButton * DialPadWidget::getStarButton() const {
	return _starButton;
}

QPushButton * DialPadWidget::getSharpButton() const {
	return _sharpButton;
}

QStringList DialPadWidget::getListDtmfTheme() const {
	QDir dir(Softphone::getInstance().getApplicationDirPath() + "sounds/dtmf");
	QStringList listTheme = dir.entryList(QDir::Dirs);
	QStringList::Iterator it;

	it = listTheme.find(".");
	if (it != listTheme.end()) {
		listTheme.remove(it);
	}
	it = listTheme.find("..");
	if (it != listTheme.end()) {
		listTheme.remove(it);
	}

	return listTheme;
}
