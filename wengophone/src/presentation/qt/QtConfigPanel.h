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

#ifndef QTCONFIGPANEL_H
#define QTCONFIGPANEL_H

#include <qtutil/QObjectThreadSafe.h>

#include <util/Trackable.h>

class Settings;
class QtVolumeSlider;
class CWengoPhone;

class QWidget;
namespace Ui { class WengoPhoneWindowConfigPanel; }

/**
 * Audio and video configuration panel inside the main window.
 *
 * @author Tanguy Krotoff
 */
class QtConfigPanel : public QObjectThreadSafe, public Trackable {
	Q_OBJECT
public:

	QtConfigPanel(CWengoPhone & cWengoPhone, QWidget * parent);

	~QtConfigPanel();

	QWidget * getWidget() const {
		return _configPanelWidget;
	}

private Q_SLOTS:

	void inputVolumeSliderValueChanged(int value);

	void outputVolumeSliderValueChanged(int value);

	void enableWenboxCheckBoxToggled(bool checked);

	void halfDuplexCheckBoxToggled(bool checked);

	void videoSettingsClicked();

	void audioSettingsClicked();

	void slotTranslationChanged();

private:

	void initThreadSafe() { }

	void configChangedEventHandler(Settings & sender, const std::string & key);

	void configChangedEventHandlerThreadSafe(Settings & sender, const std::string & key);

	Ui::WengoPhoneWindowConfigPanel * _ui;

	QWidget * _configPanelWidget;

	QtVolumeSlider * _inputVolumeSlider;

	QtVolumeSlider * _outputVolumeSlider;

	CWengoPhone & _cWengoPhone;
};

#endif	//QTCONFIGPANEL_H
