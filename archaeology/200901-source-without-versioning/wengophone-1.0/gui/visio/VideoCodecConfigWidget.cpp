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

#include "VideoCodecConfigWidget.h"
#include "MyWidgetFactory.h"

#include <phapi.h>

#include <qcombobox.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qtextedit.h>
#include <qtimer.h>

#include <stdio.h>

/**
 * Video codec configuration
 *
 * @author Mathieu Stute
 */
 
VideoCodecConfigWidget::VideoCodecConfigWidget(QWidget *parent, AudioCall & call) :
	QObject(parent), _audioCall(call) {
	_videoCodecConfigWidget = MyWidgetFactory::create("VideoCodecConfigWidgetForm.ui", this, parent);
		
	_videoCodecConfigStruct = (struct phVideoCodecConfig *)malloc(sizeof(struct phVideoCodecConfig));
	phVideoControlCodecGet(call.getCallId(), _videoCodecConfigStruct);
	
	_parametersValueLineEdit = (QLineEdit*)_videoCodecConfigWidget->child("ParameterLineEdit", "QLineEdit");
	_parametersComboBox = (QComboBox*)_videoCodecConfigWidget->child("ParameterComboBox", "QComboBox");
	_console = (QTextEdit*)_videoCodecConfigWidget->child("Console", "QTextEdit");
	_console->clear();
	_parametersComboBox->setCurrentText(tr("Quality"));
	connect(_parametersValueLineEdit, SIGNAL(returnPressed()), this, SLOT(updateConfig()));
	connect(_parametersComboBox, SIGNAL(activated(const QString &)), this, SLOT(updateValue(const QString &)));
	
	//int or float validator
	QRegExpValidator * validator = new QRegExpValidator(QRegExp("([0-9])+(\.)?([0-9])*"), _videoCodecConfigWidget);
	_parametersValueLineEdit->setValidator(validator);
	
	QTimer *timer = new QTimer(this);
	timer->singleShot(250, this, SLOT(updateStruct()));
}

QWidget * VideoCodecConfigWidget::getWidget() const {
	return _videoCodecConfigWidget;
}

void VideoCodecConfigWidget::updatePhVideoCodecConfig() {
	phVideoControlCodecSet(_audioCall.getCallId(), _videoCodecConfigStruct);
	printConfig();
}

void VideoCodecConfigWidget::updateStruct() {
	phVideoControlCodecGet(_audioCall.getCallId(), _videoCodecConfigStruct);
	printConfig();
}

void VideoCodecConfigWidget::printConfig() {
	_console->clear();
	QString mess = "";
	mess += "Wengo Codec configuration H263\n";
	mess +=	"\nQuality:" + QString::number(_videoCodecConfigStruct->f_quality) +
			"\tGop size:" + QString::number(_videoCodecConfigStruct->gopsize) +
			"\nMin Rate:" + QString::number(_videoCodecConfigStruct->minrate) +
			"\tMax Rate:" + QString::number(_videoCodecConfigStruct->maxrate) +
			"\nCompress:" + QString::number(_videoCodecConfigStruct->compress) +
			"\tBFrame:" + QString::number(_videoCodecConfigStruct->max_b_frame) +
			"\nQMin:" + QString::number(_videoCodecConfigStruct->qmin) +
			"\t\tQMax:" + QString::number(_videoCodecConfigStruct->qmax) +
			"\ni_offset:" + QString::number(_videoCodecConfigStruct->i_offset) +
			"\ti_factor:" + QString::number(_videoCodecConfigStruct->i_factor) +
			"\nb_offset:" + QString::number(_videoCodecConfigStruct->b_offset) +
			"\tb_factor:" + QString::number(_videoCodecConfigStruct->b_factor);
	_console->setText(mess);
}

void VideoCodecConfigWidget::updateConfig() {
	QString param = _parametersComboBox->currentText();
	int value = _parametersValueLineEdit->text().toFloat();
	if(param == tr("Quality")) {
		_videoCodecConfigStruct->f_quality = (int)value;
	} else if(param == tr("Compress")) {
		_videoCodecConfigStruct->compress = value;
	}else if(param == tr("Gop size")) {
		_videoCodecConfigStruct->gopsize = (int)value;
	}else if(param == tr("QMin")) {
		_videoCodecConfigStruct->qmin = value;
	}else if(param == tr("QMax")) {
		_videoCodecConfigStruct->qmax = value;
	}else if(param == tr("IFrame Offset")) {
		_videoCodecConfigStruct->i_offset = value;
	}else if(param == tr("IFrame Factor")) {
		_videoCodecConfigStruct->i_factor = value;
	}else if(param == tr("BFrame Factor")) {
		_videoCodecConfigStruct->b_factor = value;
	}else if(param == tr("BFrame Offset")) {
		_videoCodecConfigStruct->b_offset = value;
	}else if(param == tr("Max  BFrame")) {
		_videoCodecConfigStruct->max_b_frame = value;
	}else if(param == tr("Min Rate")) {
		_videoCodecConfigStruct->minrate = (int)value;
	}else if(param == tr("Max Rate")) {
		_videoCodecConfigStruct->maxrate = (int)value;
	}
	updatePhVideoCodecConfig();
}

void VideoCodecConfigWidget::updateValue(const QString & param) {
	phVideoControlCodecGet(_audioCall.getCallId(), _videoCodecConfigStruct);
	if(param == tr("Quality")) {
		_parametersValueLineEdit->setText(QString::number(_videoCodecConfigStruct->f_quality));
	} else if(param == tr("Compress")) {
		_parametersValueLineEdit->setText(QString::number(_videoCodecConfigStruct->compress));
	}else if(param == tr("Gop size")) {
		_parametersValueLineEdit->setText(QString::number(_videoCodecConfigStruct->gopsize));
	}else if(param == tr("QMin")) {
		_parametersValueLineEdit->setText(QString::number(_videoCodecConfigStruct->qmin));
	}else if(param == tr("QMax")) {
		_parametersValueLineEdit->setText(QString::number(_videoCodecConfigStruct->qmax));
	}else if(param == tr("IFrame Offset")) {
		_parametersValueLineEdit->setText(QString::number(_videoCodecConfigStruct->i_offset));
	}else if(param == tr("IFrame Factor")) {
		_parametersValueLineEdit->setText(QString::number(_videoCodecConfigStruct->i_factor));
	}else if(param == tr("BFrame Factor")) {
		_parametersValueLineEdit->setText(QString::number(_videoCodecConfigStruct->b_factor));
	}else if(param == tr("BFrame Offset")) {
		_parametersValueLineEdit->setText(QString::number(_videoCodecConfigStruct->b_offset));
	}else if(param == tr("Max  BFrame")) {
		_parametersValueLineEdit->setText(QString::number(_videoCodecConfigStruct->max_b_frame));
	}else if(param == tr("Min Rate")) {
		_parametersValueLineEdit->setText(QString::number(_videoCodecConfigStruct->minrate));
	}else if(param == tr("Max Rate")) {
		_parametersValueLineEdit->setText(QString::number(_videoCodecConfigStruct->maxrate));
	}
	printConfig();
}
