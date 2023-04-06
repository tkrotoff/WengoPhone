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

#ifndef QTAUDIOSETTINGS_H
#define QTAUDIOSETTINGS_H

#include "QtISettings.h"

#include <QtGui/QWidget>

class CWengoPhone;
namespace Ui { class AudioSettings; }

/**
 * Audio configuration panel.
 *
 * @author Tanguy Krotoff
 * @author Xavier Desjardins
 */
class QtAudioSettings : public QWidget, public QtISettings {
	Q_OBJECT
public:

	QtAudioSettings(CWengoPhone & cWengoPhone, QWidget * parent);

	virtual ~QtAudioSettings();

	virtual QString getName() const;

	virtual QString getTitle() const;

	virtual void saveConfig();

	virtual QString getIconName() const;

	virtual QWidget *getWidget() const {
		return (QWidget*)this;
	}

private Q_SLOTS:

	void makeTestCallClicked();

	void AdvPressed();

	void UpPressed();

	void DownPressed();

	void ResetPressed();

private:

	virtual void readConfig();
	void fillAudioCodecList();

	Ui::AudioSettings * _ui;

	CWengoPhone & _cWengoPhone;
};

#endif	//QTAUDIOSETTINGS_H
