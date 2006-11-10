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

#include "QtDialpad.h"

#include "ui_DialpadWidget.h"

#include "QtSVGDialpad.h"
#include "QtWengoPhone.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/dtmf/DtmfTheme.h>

#include <control/dtmf/CDtmfThemeManager.h>

#include <sound/Sound.h>
#include <sound/AudioDevice.h>

#include <cutil/global.h>
#include <util/File.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <util/StringList.h>

#include <qtutil/SafeConnect.h>
#include <qtutil/UpQComboBox.h>

#include <QtGui/QtGui>

QtDialpad::QtDialpad(CDtmfThemeManager & cDtmfThemeManager, QtWengoPhone * qtWengoPhone)
	: QWidget(NULL),
	_cDtmfThemeManager(cDtmfThemeManager) {

	_qtWengoPhone = qtWengoPhone;

	_ui = new Ui::DialpadWidget();
	_ui->setupUi(this);

	SAFE_CONNECT(_ui->zeroButton, SIGNAL(clicked()), SLOT(zeroButtonClicked()));
	SAFE_CONNECT(_ui->oneButton, SIGNAL(clicked()), SLOT(oneButtonClicked()));
	SAFE_CONNECT(_ui->twoButton, SIGNAL(clicked()), SLOT(twoButtonClicked()));
	SAFE_CONNECT(_ui->threeButton, SIGNAL(clicked()), SLOT(threeButtonClicked()));
	SAFE_CONNECT(_ui->fourButton, SIGNAL(clicked()), SLOT(fourButtonClicked()));
	SAFE_CONNECT(_ui->fiveButton, SIGNAL(clicked()), SLOT(fiveButtonClicked()));
	SAFE_CONNECT(_ui->sixButton, SIGNAL(clicked()), SLOT(sixButtonClicked()));
	SAFE_CONNECT(_ui->sevenButton, SIGNAL(clicked()), SLOT(sevenButtonClicked()));
	SAFE_CONNECT(_ui->eightButton, SIGNAL(clicked()), SLOT(eightButtonClicked()));
	SAFE_CONNECT(_ui->nineButton, SIGNAL(clicked()), SLOT(nineButtonClicked()));
	SAFE_CONNECT(_ui->starButton, SIGNAL(clicked()), SLOT(starButtonClicked()));
	SAFE_CONNECT(_ui->poundButton, SIGNAL(clicked()), SLOT(poundButtonClicked()));

	//add UpComboBox
	_audioSmileysComboBox = new UpQComboBox(_ui->frameUpQComboBox);
	_audioSmileysComboBox->setMinimumWidth(150);
	SAFE_CONNECT(_audioSmileysComboBox, SIGNAL(activated(const QString &)), SLOT(audioSmileysComboBoxThemeChanged(const QString &)));
	SAFE_CONNECT(_audioSmileysComboBox, SIGNAL(popupDisplayed()), SLOT(refreshComboBox()));

	//add svg viewer
	_qtSVGDialpad = new QtSVGDialpad();
	_ui->stackedWidget->addWidget(_qtSVGDialpad);

	SAFE_CONNECT(_qtSVGDialpad, SIGNAL(keyZeroSelected()), SLOT(zeroButtonClicked()));
	SAFE_CONNECT(_qtSVGDialpad, SIGNAL(keyOneSelected()), SLOT(oneButtonClicked()));
	SAFE_CONNECT(_qtSVGDialpad, SIGNAL(keyTwoSelected()), SLOT(twoButtonClicked()));
	SAFE_CONNECT(_qtSVGDialpad, SIGNAL(keyThreeSelected()), SLOT(threeButtonClicked()));
	SAFE_CONNECT(_qtSVGDialpad, SIGNAL(keyFourSelected()), SLOT(fourButtonClicked()));
	SAFE_CONNECT(_qtSVGDialpad, SIGNAL(keyFiveSelected()), SLOT(fiveButtonClicked()));
	SAFE_CONNECT(_qtSVGDialpad, SIGNAL(keySixSelected()), SLOT(sixButtonClicked()));
	SAFE_CONNECT(_qtSVGDialpad, SIGNAL(keySevenSelected()), SLOT(sevenButtonClicked()));
	SAFE_CONNECT(_qtSVGDialpad, SIGNAL(keyEightSelected()), SLOT(eightButtonClicked()));
	SAFE_CONNECT(_qtSVGDialpad, SIGNAL(keyNineSelected()), SLOT(nineButtonClicked()));
	SAFE_CONNECT(_qtSVGDialpad, SIGNAL(keyStarSelected()), SLOT(starButtonClicked()));
	SAFE_CONNECT(_qtSVGDialpad, SIGNAL(keyPoundSelected()), SLOT(poundButtonClicked()));

	//comboBox Construction
	fillComboBox();

	qtWengoPhone->setQtDialpad(this);
}

QtDialpad::~QtDialpad() {
	OWSAFE_DELETE(_qtSVGDialpad);
	OWSAFE_DELETE(_audioSmileysComboBox);
	OWSAFE_DELETE(_ui);
}

void QtDialpad::fillComboBox() {

	//comboBox Construction
	std::list<std::string> themeList = _cDtmfThemeManager.getThemeList();
	for (std::list<std::string>::const_iterator it = themeList.begin(); it != themeList.end(); ++it) {
		std::string theme = *it;
		std::string icon = _cDtmfThemeManager.getDtmfTheme(theme)->getRepertory() + _cDtmfThemeManager.getDtmfTheme(theme)->getImageFile();
		_audioSmileysComboBox->addItem(QIcon(QString::fromStdString(icon)), QString::fromStdString(theme), QVariant());
	}
}

void QtDialpad::refreshComboBox(){
	if (_cDtmfThemeManager.refreshDtmfThemes()) {

		//save current text
		QString selectOne = _audioSmileysComboBox->currentText();

		//refill the combobox
		_audioSmileysComboBox->clear();
		fillComboBox();

		//load current text with is new position (if possible)
		int pos = _audioSmileysComboBox->findText(selectOne);
		if (pos != -1) {
			_audioSmileysComboBox->setCurrentIndex(pos);
		}
	}
}

void QtDialpad::playTone(const std::string & tone) {

	std::string soundFile;
	if (tone == "*") {
		soundFile = "star";
	} else if (tone == "#") {
		soundFile = "pound";
	} else {
		soundFile = tone;
	}

	_cDtmfThemeManager.playTone(_audioSmileysComboBox->currentText().toStdString(), soundFile);
	_qtWengoPhone->dialpad(tone);
}

void QtDialpad::oneButtonClicked() {
	playTone("1");
}

void QtDialpad::twoButtonClicked() {
	playTone("2");
}

void QtDialpad::threeButtonClicked() {
	playTone("3");
}

void QtDialpad::fourButtonClicked() {
	playTone("4");
}

void QtDialpad::fiveButtonClicked() {
	playTone("5");
}

void QtDialpad::sixButtonClicked() {
	playTone("6");
}

void QtDialpad::sevenButtonClicked() {
	playTone("7");
}

void QtDialpad::eightButtonClicked() {
	playTone("8");
}

void QtDialpad::nineButtonClicked() {
	playTone("9");
}

void QtDialpad::zeroButtonClicked() {
	playTone("0");
}

void QtDialpad::starButtonClicked() {
	playTone("*");
}

void QtDialpad::poundButtonClicked() {
	playTone("#");
}

void QtDialpad::audioSmileysComboBoxThemeChanged(const QString & newThemeName) {

	LOG_DEBUG(newThemeName.toStdString());

	const DtmfTheme * newTheme = _cDtmfThemeManager.getDtmfTheme(newThemeName.toStdString());

	if (newTheme->getDialpadMode() == DtmfTheme::svg) {

		//change theme
		_qtSVGDialpad->setNewTheme(newTheme->getRepertory());

		//active QtSVGDialpad
		_ui->stackedWidget->setCurrentIndex(1);

		//force repaint
		_qtSVGDialpad->update();

	} else {
		_ui->stackedWidget->setCurrentIndex(0);
	}
}
