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

#ifndef VIDEOCODECCONFIG_H
#define VIDEOCODECCONFIG_H

#include "AudioCall.h"

#include <qobject.h>
#include <qwidget.h>

class QComboBox;
class QLineEdit;
class QTextEdit;

/**
 * Video codec configuration
 *
 * @author Mathieu Stute
 */
class VideoCodecConfigWidget : public QObject {
	Q_OBJECT
public:
	
	VideoCodecConfigWidget(QWidget *parent, AudioCall & call);
	
	QWidget * getWidget() const;
	
private slots:
	
	void updateConfig();
	
	void printConfig();
	
	void updateValue(const QString &);
	
	void updateStruct();

private:
	
	void updatePhVideoCodecConfig();
	
	struct phVideoCodecConfig * _videoCodecConfigStruct;
	
	AudioCall & _audioCall; 
	
	QWidget *_videoCodecConfigWidget;
	
	QComboBox * _parametersComboBox;
	
	QLineEdit * _parametersValueLineEdit;
	
	QTextEdit * _console;
	
	VideoCodecConfigWidget(const VideoCodecConfigWidget &);
	VideoCodecConfigWidget & operator=(const VideoCodecConfigWidget &);
};
#endif	//VIDEOCODECCONFIG_H
