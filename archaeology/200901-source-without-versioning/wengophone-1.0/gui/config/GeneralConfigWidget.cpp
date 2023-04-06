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

#include "GeneralConfigWidget.h"
#include "AdvancedConfigWidget.h"

#include "MyWidgetFactory.h"
#include "AdvancedConfig.h"

#include <Startup.h>

#include <qcheckbox.h>
#include <qbuttongroup.h> 
#include <qdir.h>
#include <qapplication.h>

GeneralConfigWidget::GeneralConfigWidget(QWidget * parent) : QObject(parent) {
	_generalConfigWidget = MyWidgetFactory::create("GeneralConfigWidgetForm.ui", this, parent);

#ifdef WIN32
	_startup = new Startup("Wengo", QString("\"") + qApp->applicationDirPath() + QString("/wengophone.exe\"") + " -background");
	
	AdvancedConfig & advancedConfig = AdvancedConfig::getInstance();
	
	QCheckBox * startupCheckBox = (QCheckBox *) _generalConfigWidget->child("startupCheckBox", "QCheckBox");
	if ((_startup->isStartup()) || (advancedConfig.getAutoStart())) {
		startupCheckBox->toggle();
	}
	
	_startup->setStartup(advancedConfig.getAutoStart());
	
	connect(startupCheckBox, SIGNAL(toggled(bool)),
		this, SLOT(setStartup(bool)));
#else
	QButtonGroup * bg = (QButtonGroup*)_generalConfigWidget->child("StartButtonGroup", "QButtonGroup");
	bg->hide();
#endif
	
	_useHttpCheckBox = (QCheckBox *) _generalConfigWidget->child("useHttpCheckBox", "QCheckBox");
	if (advancedConfig.isHttpProxyUsed()) {
		_useHttpCheckBox->toggle();
	}
}

GeneralConfigWidget::~GeneralConfigWidget() {
#ifdef WIN32
	delete _startup;
#endif
}

void GeneralConfigWidget::setStartup(bool startup) {
	AdvancedConfig & advancedConfig = AdvancedConfig::getInstance();
	advancedConfig.setAutoStart(startup);
	_startup->setStartup(startup);
}
