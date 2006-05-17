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

#ifndef QTAUDIOSETTINGS_H
#define QTAUDIOSETTINGS_H

#include "QtISettings.h"

#include <QObject>

class QWidget;
class QString;
class CWengoPhone;
namespace Ui { class AudioSettings; }

/**
 * Audio configuration panel.
 *
 * @author Tanguy Krotoff
 */
class QtAudioSettings : public QObject, public QtISettings {
	Q_OBJECT
public:

	QtAudioSettings(CWengoPhone & cWengoPhone, QWidget * parent);

	virtual ~QtAudioSettings();

	QWidget * getWidget() const {
		return _audioSettingsWidget;
	}

	QString getName() const;

	void saveConfig();

private Q_SLOTS:

	void makeTestCallClicked();

private:

	void readConfig();

	Ui::AudioSettings * _ui;

	QWidget * _audioSettingsWidget;

	CWengoPhone & _cWengoPhone;
};

#endif	//QTAUDIOSETTINGS_H
