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

#include "QtWengoPhone.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <Sound.h>
#include <AudioDevice.h>
#include <util/File.h>
#include <util/Logger.h>
#include <util/StringList.h>

#include <Object.h>
#include <WidgetFactory.h>

#include <QtGui>

QtDialpad::QtDialpad(QtWengoPhone * qtWengoPhone) : QObject() {
	_qtWengoPhone = qtWengoPhone;

	_dialpadWidget = WidgetFactory::create(":/forms/DialpadWidget.ui", NULL);

	_zeroButton = Object::findChild<QPushButton *>(_dialpadWidget, "zeroButton");
	connect(_zeroButton, SIGNAL(clicked()), SLOT(zeroButtonClicked()));

	_oneButton = Object::findChild<QPushButton *>(_dialpadWidget, "oneButton");
	connect(_oneButton, SIGNAL(clicked()), SLOT(oneButtonClicked()));

	_twoButton = Object::findChild<QPushButton *>(_dialpadWidget, "twoButton");
	connect(_twoButton, SIGNAL(clicked()), SLOT(twoButtonClicked()));

	_threeButton = Object::findChild<QPushButton *>(_dialpadWidget, "threeButton");
	connect(_threeButton, SIGNAL(clicked()), SLOT(threeButtonClicked()));

	_fourButton = Object::findChild<QPushButton *>(_dialpadWidget, "fourButton");
	connect(_fourButton, SIGNAL(clicked()), SLOT(fourButtonClicked()));

	_fiveButton = Object::findChild<QPushButton *>(_dialpadWidget, "fiveButton");
	connect(_fiveButton, SIGNAL(clicked()), SLOT(fiveButtonClicked()));

	_sixButton = Object::findChild<QPushButton *>(_dialpadWidget, "sixButton");
	connect(_sixButton, SIGNAL(clicked()), SLOT(sixButtonClicked()));

	_sevenButton = Object::findChild<QPushButton *>(_dialpadWidget, "sevenButton");
	connect(_sevenButton, SIGNAL(clicked()), SLOT(sevenButtonClicked()));

	_eightButton = Object::findChild<QPushButton *>(_dialpadWidget, "eightButton");
	connect(_eightButton, SIGNAL(clicked()), SLOT(eightButtonClicked()));

	_nineButton = Object::findChild<QPushButton *>(_dialpadWidget, "nineButton");
	connect(_nineButton, SIGNAL(clicked()), SLOT(nineButtonClicked()));

	_starButton = Object::findChild<QPushButton *>(_dialpadWidget, "starButton");
	connect(_starButton, SIGNAL(clicked()), SLOT(starButtonClicked()));

	_poundButton = Object::findChild<QPushButton *>(_dialpadWidget, "poundButton");
	connect(_poundButton, SIGNAL(clicked()), SLOT(poundButtonClicked()));

	_audioSmileysComboBox = Object::findChild<QComboBox *>(_dialpadWidget, "audioSmileysComboBox");
	connect(_audioSmileysComboBox, SIGNAL(activated(int)), SLOT(audioSmileysComboBoxActivated(int)));

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	QStringList listAudioSmileys = getListAudioSmileys();
	for (int i = 0; i < listAudioSmileys.size(); ++i) {
		std::string theme = listAudioSmileys[i].toStdString();
		std::string icon = config.getAudioSmileysDir() + theme + File::getPathSeparator() + theme + ".png";
		_audioSmileysComboBox->addItem(QIcon(QString::fromStdString(icon)), listAudioSmileys[i]);
	}
}

QStringList QtDialpad::getListAudioSmileys() const {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	QDir dir(QString::fromStdString(config.getAudioSmileysDir()));
	QStringList listAudioSmileys = dir.entryList(QDir::Dirs);

	int index = listAudioSmileys.indexOf(".");
	if (index != -1) {
		listAudioSmileys.removeAt(index);
	}
	index = listAudioSmileys.indexOf("..");
	if (index != -1) {
		listAudioSmileys.removeAt(index);
	}

	return listAudioSmileys;
}

void QtDialpad::playTone(const std::string & tone) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	std::string soundFile;
	if (tone == "*") {
		soundFile = "star";
	} else if (tone == "#") {
		soundFile = "pound";
	} else {
		soundFile = tone;
	}

	if (_audioSmileysComboBox->currentIndex() == 0) {
		_qtWengoPhone->dialpad(tone, String::null);
		Sound::play(File::convertPathSeparators(config.getAudioSmileysDir() + soundFile + ".wav"), config.getAudioRingerDeviceName());
	} else {
		soundFile = config.getAudioSmileysDir() + _audioSmileysComboBox->currentText().toStdString()
				+ File::getPathSeparator() + soundFile + ".raw";
		_qtWengoPhone->dialpad(tone, File::convertPathSeparators(soundFile));
		Sound::play(File::convertPathSeparators(soundFile), config.getAudioRingerDeviceName());
	}

	LOG_DEBUG("sound file=" + soundFile);
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

void QtDialpad::audioSmileysComboBoxActivated(int index) {
	static const QString originalZeroButtonText = _zeroButton->text();
	static const QString originalOneButtonText = _oneButton->text();
	static const QString originalTwoButtonText = _twoButton->text();
	static const QString originalThreeButtonText = _threeButton->text();
	static const QString originalFourButtonText = _fourButton->text();
	static const QString originalFiveButtonText = _fiveButton->text();
	static const QString originalSixButtonText = _sixButton->text();
	static const QString originalSevenButtonText = _sevenButton->text();
	static const QString originalEightButtonText = _eightButton->text();
	static const QString originalNineButtonText = _nineButton->text();
	static const QString originalStarButtonText = _starButton->text();
	static const QString originalSharpButtonText = _poundButton->text();

	if (index == 0) {
		_zeroButton->setIcon(QIcon());
		_zeroButton->setText(originalZeroButtonText);
		_oneButton->setIcon(QIcon());
		_oneButton->setText(originalOneButtonText);
		_twoButton->setIcon(QIcon());
		_twoButton->setText(originalTwoButtonText);
		_threeButton->setIcon(QIcon());
		_threeButton->setText(originalThreeButtonText);
		_fourButton->setIcon(QIcon());
		_fourButton->setText(originalFourButtonText);
		_fiveButton->setIcon(QIcon());
		_fiveButton->setText(originalFiveButtonText);
		_sixButton->setIcon(QIcon());
		_sixButton->setText(originalSixButtonText);
		_sevenButton->setIcon(QIcon());
		_sevenButton->setText(originalSevenButtonText);
		_eightButton->setIcon(QIcon());
		_eightButton->setText(originalEightButtonText);
		_nineButton->setIcon(QIcon());
		_nineButton->setText(originalNineButtonText);
		_starButton->setIcon(QIcon());
		_starButton->setText(originalStarButtonText);
		_poundButton->setIcon(QIcon());
		_poundButton->setText(originalSharpButtonText);
	} else {
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		QString iconFile = QString::fromStdString(config.getAudioSmileysDir() +
			_audioSmileysComboBox->currentText().toStdString() + File::getPathSeparator());

		_zeroButton->setIcon(QIcon(iconFile + "0.png"));
		_zeroButton->setText(QString::null);
		_oneButton->setIcon(QIcon(iconFile + "1.png"));
		_oneButton->setText(QString::null);
		_twoButton->setIcon(QIcon(iconFile + "2.png"));
		_twoButton->setText(QString::null);
		_threeButton->setIcon(QIcon(iconFile + "3.png"));
		_threeButton->setText(QString::null);
		_fourButton->setIcon(QIcon(iconFile + "4.png"));
		_fourButton->setText(QString::null);
		_fiveButton->setIcon(QIcon(iconFile + "5.png"));
		_fiveButton->setText(QString::null);
		_sixButton->setIcon(QIcon(iconFile + "6.png"));
		_sixButton->setText(QString::null);
		_sevenButton->setIcon(QIcon(iconFile + "7.png"));
		_sevenButton->setText(QString::null);
		_eightButton->setIcon(QIcon(iconFile + "8.png"));
		_eightButton->setText(QString::null);
		_nineButton->setIcon(QIcon(iconFile + "9.png"));
		_nineButton->setText(QString::null);
		_starButton->setIcon(QIcon(iconFile + "star.png"));
		_starButton->setText(QString::null);
		_poundButton->setIcon(QIcon(iconFile + "pound.png"));
		_poundButton->setText(QString::null);
	}
}
