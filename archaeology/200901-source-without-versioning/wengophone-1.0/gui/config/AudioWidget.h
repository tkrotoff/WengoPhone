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

#ifndef AUDIOWIDGET_H
#define AUDIOWIDGET_H

#include "ConfigWindow.h"

#include <observer/Observer.h>

#include <qobject.h>

class Audio;
class Event;
class Subject;
class MainWindow;
class StringList;
class SoundMixer;
class QWidget;
class QPushButton;
class QRadioButton;
class QComboBox;
class QCheckBox;

/**
 * GUI Widget used to select audio devices.
 *
 * @author Philippe Kajmar
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class AudioWidget : public QObject, public Observer {
	Q_OBJECT
public:

	AudioWidget(ConfigWindow & configWindow, MainWindow & mainWindow);

	~AudioWidget();

	/**
	 * Gets the low-level widget.
	 *
	 * @return the low-level widget for this gui component
	 */
	QWidget * getWidget() const {
		return _audioWidget;
	}

	/**
	 * Callback, called each time a Subject has changed.
	 *
	 * @param subject Subject that has been changed
	 * @param event Event
	 * @see Observer::update();
	 */
	void update(const Subject &, const Event & event);

	/**
	 * Saves changes.
	 */
	void save();

private slots:

	/**
	 * Sets default settings.
	 */
	void defaultButtonClicked();

	void setInputVolume(int volume);

	void setOutputVolume(int volume);
	
	void setMicPlayBack(bool mute);

private:

	AudioWidget(const AudioWidget &);
	AudioWidget & operator=(const AudioWidget &);

	void updateDevices();

	void updateSoundSliders();

	/// apply changes
	void apply();

	/// init combo boxes
	void initComboBoxes();

	/**
	 * Fills combo box.
	 *
	 * @param comboBox to fill
	 * @param list of audio devices
	 */
	static void fillComboBox(QComboBox * comboBox, const StringList & listAudioDevices);

	/// graphical component
	QWidget * _audioWidget;

	/// input device combo box
	QComboBox * _inputComboBox;

	/// output device combo box
	QComboBox * _outputComboBox;

	/// ringing device combo box
	QComboBox * _ringingComboBox;

	/// default button
	QPushButton * _defaultButton;

	QRadioButton * _audioConfigRadioButton;
	
	QCheckBox * _muteMicPlayBackCheckBox;

	/// subject
	Audio & _audio;

	ConfigWindow & _configWindow;

	MainWindow & _mainWindow;

	SoundMixer * _soundMixer;
};

#endif	//AUDIOWIDGET_H 
