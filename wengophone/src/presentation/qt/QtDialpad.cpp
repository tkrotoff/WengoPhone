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

#include "QtWengoPhone.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <sound/Sound.h>
#include <sound/AudioDevice.h>

#include <util/File.h>
#include <util/Logger.h>
#include <util/StringList.h>

#include <QtGui>

QtDialpad::QtDialpad(QtWengoPhone * qtWengoPhone)
	: QObject(NULL) {

	_qtWengoPhone = qtWengoPhone;

	_dialpadWidget = new QWidget(NULL);

	_ui = new Ui::DialpadWidget();
	_ui->setupUi(_dialpadWidget);

	connect(_ui->zeroButton, SIGNAL(clicked()), SLOT(zeroButtonClicked()));
	connect(_ui->oneButton, SIGNAL(clicked()), SLOT(oneButtonClicked()));
	connect(_ui->twoButton, SIGNAL(clicked()), SLOT(twoButtonClicked()));
	connect(_ui->threeButton, SIGNAL(clicked()), SLOT(threeButtonClicked()));
	connect(_ui->fourButton, SIGNAL(clicked()), SLOT(fourButtonClicked()));
	connect(_ui->fiveButton, SIGNAL(clicked()), SLOT(fiveButtonClicked()));
	connect(_ui->sixButton, SIGNAL(clicked()), SLOT(sixButtonClicked()));
	connect(_ui->sevenButton, SIGNAL(clicked()), SLOT(sevenButtonClicked()));
	connect(_ui->eightButton, SIGNAL(clicked()), SLOT(eightButtonClicked()));
	connect(_ui->nineButton, SIGNAL(clicked()), SLOT(nineButtonClicked()));
	connect(_ui->starButton, SIGNAL(clicked()), SLOT(starButtonClicked()));
	connect(_ui->poundButton, SIGNAL(clicked()), SLOT(poundButtonClicked()));
	connect(_ui->audioSmileysComboBox, SIGNAL(activated(int)), SLOT(audioSmileysComboBoxActivated(int)));

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	QStringList listAudioSmileys = getListAudioSmileys();
	for (int i = 0; i < listAudioSmileys.size(); ++i) {
		std::string theme = listAudioSmileys[i].toStdString();
		std::string icon = config.getAudioSmileysDir() + theme + File::getPathSeparator() + theme + ".png";
		_ui->audioSmileysComboBox->addItem(QIcon(QString::fromStdString(icon)), listAudioSmileys[i]);
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

	if (_ui->audioSmileysComboBox->currentIndex() == 0) {
		_qtWengoPhone->dialpad(tone, String::null);
		Sound::play(File::convertPathSeparators(config.getAudioSmileysDir() + soundFile + ".wav"), config.getAudioRingerDeviceId());
	} else {
		soundFile = config.getAudioSmileysDir() + _ui->audioSmileysComboBox->currentText().toStdString()
				+ File::getPathSeparator() + soundFile + ".raw";
		_qtWengoPhone->dialpad(tone, File::convertPathSeparators(soundFile));
		Sound::play(File::convertPathSeparators(soundFile), config.getAudioRingerDeviceId());
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
	/*
	static const QString originalZeroButtonText = _ui->zeroButton->text();
	static const QString originalOneButtonText = _ui->oneButton->text();
	static const QString originalTwoButtonText = _ui->twoButton->text();
	static const QString originalThreeButtonText = _ui->threeButton->text();
	static const QString originalFourButtonText = _ui->fourButton->text();
	static const QString originalFiveButtonText = _ui->fiveButton->text();
	static const QString originalSixButtonText = _ui->sixButton->text();
	static const QString originalSevenButtonText = _ui->sevenButton->text();
	static const QString originalEightButtonText = _ui->eightButton->text();
	static const QString originalNineButtonText = _ui->nineButton->text();
	static const QString originalStarButtonText = _ui->starButton->text();
	static const QString originalSharpButtonText = _ui->poundButton->text();

	if (index == 0) {
		_ui->zeroButton->setIcon(QIcon());
		_ui->zeroButton->setText(originalZeroButtonText);
		_ui->oneButton->setIcon(QIcon());
		_ui->oneButton->setText(originalOneButtonText);
		_ui->twoButton->setIcon(QIcon());
		_ui->twoButton->setText(originalTwoButtonText);
		_ui->threeButton->setIcon(QIcon());
		_ui->threeButton->setText(originalThreeButtonText);
		_ui->fourButton->setIcon(QIcon());
		_ui->fourButton->setText(originalFourButtonText);
		_ui->fiveButton->setIcon(QIcon());
		_ui->fiveButton->setText(originalFiveButtonText);
		_ui->sixButton->setIcon(QIcon());
		_ui->sixButton->setText(originalSixButtonText);
		_ui->sevenButton->setIcon(QIcon());
		_ui->sevenButton->setText(originalSevenButtonText);
		_ui->eightButton->setIcon(QIcon());
		_ui->eightButton->setText(originalEightButtonText);
		_ui->nineButton->setIcon(QIcon());
		_ui->nineButton->setText(originalNineButtonText);
		_ui->starButton->setIcon(QIcon());
		_ui->starButton->setText(originalStarButtonText);
		_ui->poundButton->setIcon(QIcon());
		_ui->poundButton->setText(originalSharpButtonText);
	} else {
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		QString iconFile = QString::fromStdString(config.getAudioSmileysDir() +
			_ui->audioSmileysComboBox->currentText().toStdString() + File::getPathSeparator());

		_ui->zeroButton->setIcon(QIcon(iconFile + "0.png"));
		_ui->zeroButton->setText(QString::null);
		_ui->oneButton->setIcon(QIcon(iconFile + "1.png"));
		_ui->oneButton->setText(QString::null);
		_ui->twoButton->setIcon(QIcon(iconFile + "2.png"));
		_ui->twoButton->setText(QString::null);
		_ui->threeButton->setIcon(QIcon(iconFile + "3.png"));
		_ui->threeButton->setText(QString::null);
		_ui->fourButton->setIcon(QIcon(iconFile + "4.png"));
		_ui->fourButton->setText(QString::null);
		_ui->fiveButton->setIcon(QIcon(iconFile + "5.png"));
		_ui->fiveButton->setText(QString::null);
		_ui->sixButton->setIcon(QIcon(iconFile + "6.png"));
		_ui->sixButton->setText(QString::null);
		_ui->sevenButton->setIcon(QIcon(iconFile + "7.png"));
		_ui->sevenButton->setText(QString::null);
		_ui->eightButton->setIcon(QIcon(iconFile + "8.png"));
		_ui->eightButton->setText(QString::null);
		_ui->nineButton->setIcon(QIcon(iconFile + "9.png"));
		_ui->nineButton->setText(QString::null);
		_ui->starButton->setIcon(QIcon(iconFile + "star.png"));
		_ui->starButton->setText(QString::null);
		_ui->poundButton->setIcon(QIcon(iconFile + "pound.png"));
		_ui->poundButton->setText(QString::null);
	}
	*/
}
