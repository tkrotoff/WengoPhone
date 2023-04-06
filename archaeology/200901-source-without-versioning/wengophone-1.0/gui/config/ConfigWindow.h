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

#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include "MainWindow.h"

#include <qobject.h>
#include <qlistview.h>

class GeneralConfigWidget;
class SipProfileWidget;
class StyleWidget;
class LanguageWidget;
class AdvancedConfigWidget;
class CodecListWidget;
class AudioWidget;
class VideoWidget;
class QDialog;
class QListBoxItem;
class QWidgetStack;
class QPushButton;

/**
 * Window for the configuration/preferences of the softphone.
 *
 * @author Tanguy Krotoff
 */
class ConfigWindow : public QObject {
	Q_OBJECT
public:

	/**
	 * mainWindow: ugly hack so that winAudioWidget gets the sound sliders
	 */
	ConfigWindow(QWidget * parent, MainWindow & mainWindow);

	~ConfigWindow();

	/**
	 * Gets the low-level widget.
	 *
	 * @return the low-level widget for this gui component
	 */
	QDialog * getWidget() const {
		return _configWindow;
	}
	
	/**
	 * Show the audio configuration panel
	 */
	void raiseAudioConfig();
	
	/**
	 * Show the video configuration panel
	 */
	void raiseVideoConfig();
	
	AudioWidget & getAudioWidget() const;
	VideoWidget & getVideoWidget() const;

private slots:

	/**
	 * A new QListViewItem has been selected.
	 *
	 * @param itemSelected QListBoxItem that has been selected
	 * @see
	 */
	void selectionChanged(QListBoxItem * itemSelected);

	/**
	 * Button save has been clicked.
	 *
	 * Saves the configuration of the application.
	 */
	void saveButtonClicked();
	void cancelButtonClicked();

	void close();

private:

	ConfigWindow(const ConfigWindow &);
	ConfigWindow & operator=(const ConfigWindow &);

	/**
	 * The low-level widget of this gui component.
	 */
	QDialog * _configWindow;

	QWidgetStack * _widgetStack;

	/**
	 * Widget used for the SIP profile.
	 */
	SipProfileWidget * _sipProfileWidget;
 
	/**
	 * Widget for the translation settings.
	 */
	LanguageWidget * _languageWidget;

	/**
	 * Widget used to modify the advanced settings.
	 */
	AdvancedConfigWidget * _advancedConfigWidget;

	/**
	 * Widget for the codecs settings.
	 */
	CodecListWidget * _audioCodecListWidget;
	CodecListWidget * _videoCodecListWidget;

	/**
	 * Widget for the audio settings.
	 */
	AudioWidget * _audioWidget;

	VideoWidget * _videoWidget;

	/**
	 * Widget for the Qt style to use.
	 */
	StyleWidget * _styleWidget;

	/**
	 * Widget used to modify the general settings.
	 */
	GeneralConfigWidget * _generalConfigWidget;

	/**
	 * Save button.
	 */
	QPushButton * _saveButton;
	QPushButton * _cancelButton;
};

#endif	//CONFIGWINDOW_H
