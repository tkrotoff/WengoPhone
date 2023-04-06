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


#include "ConfigWindow.h"

#include "Config.h"
#include "StyleWidget.h"
#include "LanguageWidget.h"
#include "SipProfileWidget.h"
#include "AdvancedConfigWidget.h"
#include "CodecListWidget.h"
#include "GeneralConfigWidget.h"
#include "AudioWidget.h"
#include "VideoWidget.h"
#include "AudioCallManager.h"
#include "Authentication.h"
#include "connect/Connect.h"
#include "MyWidgetFactory.h"
#include "AdvancedConfig.h"
#include "util/EventFilter.h"

#include <http/HttpRequest.h>

#include <qstring.h>
#include <qwidgetstack.h>
#include <qlistbox.h>
#include <qheader.h>
#include <qdialog.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qmessagebox.h>

#include <iostream>
using namespace std;

ConfigWindow::ConfigWindow(QWidget * parent, MainWindow & mainWindow) : QObject(parent) {

	_configWindow = (QDialog *) MyWidgetFactory::create("ConfigWindowForm.ui", this, parent);

	CloseEventFilter * closeEventFilter = new CloseEventFilter(this, SLOT(close()));
	_configWindow->installEventFilter(closeEventFilter);

	_widgetStack = (QWidgetStack *) _configWindow->child("widgetStack", "QWidgetStack");

	//ListBox
	QListBox * listBox = (QListBox *) _configWindow->child("listBox", "QListBox");
	listBox->setMinimumWidth(175);
	connect(listBox, SIGNAL(selectionChanged(QListBoxItem *)),
		this, SLOT(selectionChanged(QListBoxItem *)));

	//GeneralConfigWidget
	_generalConfigWidget = new GeneralConfigWidget(_configWindow);
	_widgetStack->addWidget(_generalConfigWidget->getWidget());
	_widgetStack->raiseWidget(_generalConfigWidget->getWidget());

	//LanguageWidget
	_languageWidget = new LanguageWidget(_configWindow);
	_widgetStack->addWidget(_languageWidget->getWidget());

	//StyleWidget
	_styleWidget = new StyleWidget(_configWindow);
	_widgetStack->addWidget(_styleWidget->getWidget());

	//AudioWidget
	_audioWidget = new AudioWidget(*this, mainWindow);
	_widgetStack->addWidget(_audioWidget->getWidget());

#ifdef ENABLE_VIDEO
	//VideoWidget
	_videoWidget = new VideoWidget(*this, mainWindow);
	_widgetStack->addWidget(_videoWidget->getWidget());
#endif

	//AdvancedConfigWidget
	_advancedConfigWidget = new AdvancedConfigWidget(_configWindow);
	_widgetStack->addWidget(_advancedConfigWidget->getWidget());

	_audioCodecListWidget = NULL;
	_videoCodecListWidget = NULL;
	_sipProfileWidget = NULL;

	//SipProfileWidget
	_sipProfileWidget = new SipProfileWidget(_configWindow);
	_widgetStack->addWidget(_sipProfileWidget->getWidget());

	//AudioCodecListWidget
	_audioCodecListWidget = new CodecListWidget(
		Config::getInstance().getAudioCodecList(), _configWindow);
	_widgetStack->addWidget(_audioCodecListWidget->getWidget());

#ifdef ENABLE_VIDEO
	//VideoCodecListWidget
	_videoCodecListWidget = new CodecListWidget(
		Config::getInstance().getVideoCodecList(), _configWindow);
	_widgetStack->addWidget(_videoCodecListWidget->getWidget());
#endif

#ifndef ENABLE_VIDEO
    QListBoxItem *_bItem;

    _bItem = listBox->findItem("Video Settings");
    if (_bItem) {
        delete _bItem;
    }

    _bItem = listBox->findItem("Video Codecs");
    if (_bItem) {
        delete _bItem;
    }
#endif

	//Save button
	_saveButton = (QPushButton *) _configWindow->child("saveButton", "QPushButton");
	_cancelButton = (QPushButton *) _configWindow->child("closeButton", "QPushButton");
	connect(_saveButton, SIGNAL(clicked()),
		this, SLOT(saveButtonClicked()));
	connect(_cancelButton, SIGNAL(clicked()),
		this, SLOT(cancelButtonClicked()));
		
	QCheckBox * c1 = _generalConfigWidget->getUseHttpCheckBox();
	QCheckBox * c2 = _advancedConfigWidget->getUseHttpCheckBox();
	connect(c1, SIGNAL(toggled(bool)), c2, SLOT(setChecked(bool)));
	connect(c2, SIGNAL(toggled(bool)), c1, SLOT(setChecked(bool)));
}

ConfigWindow::~ConfigWindow() {
	delete _styleWidget;
	delete _languageWidget;
	delete _widgetStack;
	delete _configWindow;
}

AudioWidget & ConfigWindow::getAudioWidget() const {
	assert(_audioWidget && "_audioWidget cannot be NULL");
	return *_audioWidget;
}

VideoWidget & ConfigWindow::getVideoWidget() const {
	assert(_videoWidget && "_audioWidget cannot be NULL");
	return *_videoWidget;
}

void ConfigWindow::selectionChanged(QListBoxItem * itemSelected) {
	/*
	Do not remove QObject::tr() or there will be a bug
	in the translation process
	*/

	QString item = itemSelected->text();

	QWidget * widget = NULL;

	if (item == QObject::tr("General")) {
		if (_generalConfigWidget) {
			widget = _generalConfigWidget->getWidget();
		}
	}

	if (item == QObject::tr("Language")) {
		if (_languageWidget) {
			widget = _languageWidget->getWidget();
		}
	}

	if (item == QObject::tr("Advanced")) {
		if (_advancedConfigWidget) {
			widget = _advancedConfigWidget->getWidget();
		}
	}

	if (item == QObject::tr("Audio Codecs")) {
		if (_audioCodecListWidget) {
			widget = _audioCodecListWidget->getWidget();
		}
	}

	if (item == QObject::tr("Video Codecs")) {
		if (_videoCodecListWidget) {
			widget = _videoCodecListWidget->getWidget();
		}
	}

	if (item == QObject::tr("Audio Settings")) {
		if (_audioWidget) {
			widget = _audioWidget->getWidget();
		}
	}
	if (item == QObject::tr("Video Settings")) {
		if (_videoWidget) {
			widget = _videoWidget->getWidget();
		}
	}

	if (item == QObject::tr("Style")) {
		if (_styleWidget) {
			widget = _styleWidget->getWidget();
		}
	}

	if (item == QObject::tr("SIP Profile")) {
		if (_sipProfileWidget) {
			widget = _sipProfileWidget->getWidget();
		}
	}

	if (!widget) {
		//Raises the default page
		_widgetStack->raiseWidget(0);
	} else {
		_widgetStack->raiseWidget(widget);
	}
}

void ConfigWindow::saveButtonClicked() {
	close();
	_advancedConfigWidget->save();
	_audioWidget->save();
	/*
	AdvancedConfig & config = AdvancedConfig::getInstance();
	if(!config.getHttpProxyUrl().isEmpty() && !config.getHttpProxyPort().isEmpty()) {
		if(!config.getHttpProxyUserID().isEmpty() && !config.getHttpProxyUserPassword().isEmpty()) {
			HttpRequest::setProxy(config.getHttpProxyUrl(), config.getHttpProxyPort().toInt(),
				config.getHttpProxyUserID(), config.getHttpProxyUserPassword());
		} else {
			HttpRequest::setProxy(config.getHttpProxyUrl(), config.getHttpProxyPort().toInt(),
				"", "");
		}
	} else {
		HttpRequest::setProxy("", 0, "", "");
	}
	*/
#ifdef ENABLE_VIDEO
	_videoWidget->save();
#endif

	_audioCodecListWidget->save();
	_sipProfileWidget->save();
	Authentication::getInstance().saveAsXml();

	Config::getInstance().saveAsXml();

	Connect & connect = Connect::getInstance();
	connect.disconnect();
	connect.connect();
}

void ConfigWindow::cancelButtonClicked() {
	close();
}

void ConfigWindow::close() {
#ifdef ENABLE_VIDEO
	_videoWidget->stopCapture();
#endif
}

void ConfigWindow::raiseAudioConfig() {
	_configWindow->show();
	_widgetStack->raiseWidget(_audioWidget->getWidget());
}

void ConfigWindow::raiseVideoConfig() {
	_configWindow->show();
	_widgetStack->raiseWidget(_videoWidget->getWidget());
}
