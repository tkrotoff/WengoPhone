/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWQTSTATUSBAR_H
#define OWQTSTATUSBAR_H

#include <sipwrapper/EnumPhoneLineState.h>
#include <util/Trackable.h>

#include <QtGui/QWidget>

class DropDownWindow;
class Settings;
class QSlider;
class QStatusBar;
class QMovie;
class QWidget;

class QtWengoPhone;

namespace Ui { class StatusBarWidget; }

/**
 * Main window status bar.
 *
 * Contains status icons for Internet connection, sound status, SIP (network) status...
 *
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class QtStatusBar : public QWidget, public Trackable {
	Q_OBJECT
public:

	QtStatusBar(QtWengoPhone* qtWengoPhone, QStatusBar * statusBar);

	~QtStatusBar();

	/**
	 * @see QStatusBar::showMessage()
	 */
	void showMessage(const QString & message, int timeout = 0);

	/**
	 * Update sip connection icon.
	 * @param state the new state of the PhoneLine
	 */
	void updatePhoneLineState(EnumPhoneLineState::PhoneLineState state);

Q_SIGNALS:
	void audioOutputConfigChanged();

	void audioInputConfigChanged();

private Q_SLOTS:

	/**
	 * Re-initializes the status bar.
	 */
	void userProfileDeleted();

	void languageChanged();

	void updateAudioOutputUi();

	void updateAudioInputUi();

	void setVolume(int);

private:

	/**
	 * Initializes widgets content.
	 */
	void init();

	void initVolumeSlider(QSlider*);

	/**
	 * @see QtStatusBar::updateSoundIcon
	 */
	void checkSoundConfig(const std::string & key);

	void createAudioOutputWindow();

	void createAudioInputWindow();

	QStatusBar * _statusBar;

	Ui::StatusBarWidget * _ui;

	QMovie * _sipConnectionMovie;

	DropDownWindow * _audioOutputWindow;
	QSlider * _outputVolumeSlider;
	DropDownWindow * _audioInputWindow;
	QSlider * _inputVolumeSlider;
};

#endif	//OWQTSTATUSBAR_H
