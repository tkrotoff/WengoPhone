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

#ifndef OWQTCONFIGPANEL_H
#define OWQTCONFIGPANEL_H

#include <qtutil/QObjectThreadSafe.h>

#include <util/Trackable.h>

class Settings;
class CWengoPhone;

class QWidget;
namespace Ui { class WengoPhoneWindowConfigPanel; }

/**
 * Audio and video configuration panel inside the main window.
 *
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class QtConfigPanel : public QObjectThreadSafe, public Trackable {
	Q_OBJECT
public:

	QtConfigPanel(CWengoPhone & cWengoPhone, QWidget * parent);

	~QtConfigPanel();

	QWidget * getWidget() const {
		return _configPanelWidget;
	}

Q_SIGNALS:
	/**
	 * Emitted whenever the user adjusts the volume sliders.
	 * This signal is needed as long as the VolumeControl class can't emit
	 * events when it's modified.
	 */
	void volumeControlChanged();

private Q_SLOTS:

	void setInputVolume(int value);

	void setOutputVolume(int value);

	void muteRingingCheckBoxToggled(bool checked);

	void halfDuplexCheckBoxToggled(bool checked);

	void videoSettingsClicked();

	void audioSettingsClicked();

	void paintEvent(QEvent * event);

	void languageChanged();

private:

	void initThreadSafe() { }

	void configChangedEventHandler(const std::string & key);

	void configChangedEventHandlerThreadSafe(const std::string & key);

	Ui::WengoPhoneWindowConfigPanel * _ui;

	QWidget * _configPanelWidget;

	CWengoPhone & _cWengoPhone;
};

#endif	//OWQTCONFIGPANEL_H
