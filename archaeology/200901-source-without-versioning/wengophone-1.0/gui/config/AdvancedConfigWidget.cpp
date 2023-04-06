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

#include "AdvancedConfigWidget.h"

#include "MyWidgetFactory.h"

#include <observer/Event.h>
#include <http/HttpRequest.h>

#include <netlib/get_net_conf.h>

#include <qwidget.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <cassert>

struct s_nat_type {
	const char * name;
	AdvancedConfigWidget::index index;
};

static const struct s_nat_type tc_nat_type[] = {
	{AdvancedConfig::AUTOMATIC, AdvancedConfigWidget::AUTOMATIC_INDEX},
	{AdvancedConfig::NONE, AdvancedConfigWidget::NONE_INDEX},
	{AdvancedConfig::FULL_CONE, AdvancedConfigWidget::FULL_CONE_INDEX},
	{AdvancedConfig::RESTRICT_CONE, AdvancedConfigWidget::RESTRICT_CONE_INDEX},
	{AdvancedConfig::PORT_RESTRICT_CONE, AdvancedConfigWidget::PORT_RESTRICT_CONE_INDEX},
	{AdvancedConfig::PORT_RESTRICT_CONE, AdvancedConfigWidget::SYMMETRIC_INDEX},
};

/**
 * @param parent widget used for free
 * @brief Ctor
 */
AdvancedConfigWidget::AdvancedConfigWidget(QWidget * parent)
: QObject(parent), _advancedConfig(AdvancedConfig::getInstance()) {

	_advancedConfigWidget = MyWidgetFactory::create("AdvancedConfigWidgetForm.ui", this, parent);
	_portLineEdit = (QLineEdit *) _advancedConfigWidget->child("portLineEdit");
	_useHttpCheckBox = (QCheckBox *) _advancedConfigWidget->child("useHttpCheckBox");
	connect(_useHttpCheckBox, SIGNAL(toggled(bool)), this, SLOT(setUseHttp(bool)));

	//hide nat configuration
	_natComboBox = (QComboBox *) _advancedConfigWidget->child("natComboBox");
	QLabel * _natLabel = (QLabel *) _advancedConfigWidget->child("textLabel1");
	_natComboBox->hide();
	_natLabel->hide();
	
	QPushButton * getProxyButton = (QPushButton *) _advancedConfigWidget->child("getProxyButton");
	connect(getProxyButton, SIGNAL(clicked()),
		this, SLOT(getProxySettings()));

	_httpUrlLineEdit = (QLineEdit *) _advancedConfigWidget->child("httpUrlLineEdit");

	_httpPortLineEdit = (QLineEdit *) _advancedConfigWidget->child("httpPortLineEdit");
	
	_proxyUserIdLineEdit = (QLineEdit *) _advancedConfigWidget->child("proxyUserIdLineEdit");

	_proxyUserPasswordLineEdit = (QLineEdit *) _advancedConfigWidget->child("proxyPasswordLineEdit");

	_echoCancellerCheckBox = (QCheckBox *) _advancedConfigWidget->child("echoCancellerCheckBox");

	_authenticationCheckBox = (QCheckBox *) _advancedConfigWidget->child("authenticationCheckBox");
	if(!_advancedConfig.isProxyAuthentication()) {
		_proxyUserIdLineEdit->setEnabled(false);
		_proxyUserPasswordLineEdit->setEnabled(false);
	}

	_sslCheckBox = (QCheckBox *) _advancedConfigWidget->child("SSLCheckBox");
	
	_defaultButton = (QPushButton *) _advancedConfigWidget->child("defaultButton");
	connect(_defaultButton, SIGNAL(clicked()),
		this, SLOT(defaultButtonClicked()));
	
	//Update widget
	update(_advancedConfig, Event());
}

/**
 * @brief set default settings
 */
void AdvancedConfigWidget::defaultButtonClicked() {
	_advancedConfig.defaultSettings();
}

void AdvancedConfigWidget::getProxySettings() {
	net_info_t netInfo;
	//Get the network configuration
	GetNetConf(&netInfo, 0);
	QString proxyUrl = netInfo.Proxy_IP;
	QString proxyPort = QString::number(netInfo.Proxy_Port);

	if (!proxyUrl.isEmpty() && !proxyPort.isEmpty()) {
		_advancedConfig.setHttpProxyUrl(proxyUrl);
		_advancedConfig.setHttpProxyPort(proxyPort);
		update(_advancedConfig, Event());
	}
}

/**
 * @param event occuring
 * @param subject modified
 * @brief apply changes on the window and apply modifications
 * inherits from class Observer
 */
void AdvancedConfigWidget::update(const Subject &, const Event &) {
	_portLineEdit->setText(_advancedConfig.getSipPort());
	_natComboBox->setCurrentItem(nameToIndex(_advancedConfig.getNat()));
	_useHttpCheckBox->setChecked(_advancedConfig.isHttpProxyUsed());
	_httpUrlLineEdit->setText(_advancedConfig.getHttpProxyUrl());
	_httpPortLineEdit->setText(_advancedConfig.getHttpProxyPort());
	_echoCancellerCheckBox->setChecked(_advancedConfig.hasEchoCanceller());
	_authenticationCheckBox->setChecked(_advancedConfig.isProxyAuthentication());
	_sslCheckBox->setChecked(_advancedConfig.useSSL());
	_proxyUserIdLineEdit->setText(_advancedConfig.getHttpProxyUserID());
	_proxyUserPasswordLineEdit->setText(_advancedConfig.getHttpProxyUserPassword());
}

/**
 * @brief save changes
 */
void AdvancedConfigWidget::save() {
	QString error_message;

	QString port = _portLineEdit->text();
	QString nat = indexToName(_natComboBox->currentItem());
	bool echoCanceller = _echoCancellerCheckBox->isChecked();
	bool useHttpProxy = _useHttpCheckBox->isChecked();
	bool proxyAuthentication = _authenticationCheckBox->isChecked();
	bool ssl = _sslCheckBox->isChecked();
	QString httpProxyUrl = _httpUrlLineEdit->text();
	QString httpProxyPort = _httpPortLineEdit->text();
	QString proxyUserID = _proxyUserIdLineEdit->text();
	QString proxyUserPassword = _proxyUserPasswordLineEdit->text();

	_advancedConfig.setSipPort(port);
	_advancedConfig.setNat(nat);
	_advancedConfig.setEchoCanceller(echoCanceller);
	_advancedConfig.setUseHttpProxy(useHttpProxy);	
	_advancedConfig.setHttpProxyUrl(httpProxyUrl);
	_advancedConfig.setHttpProxyPort(httpProxyPort);
	_advancedConfig.setHttpProxyUserID(proxyUserID);
	_advancedConfig.setHttpProxyUserPassword(proxyUserPassword);
	_advancedConfig.setProxyAuthentication(proxyAuthentication);
	_advancedConfig.setSSL(ssl);
	
	//configure Http request class
	if(!_advancedConfig.getHttpProxyUrl().isEmpty() && !_advancedConfig.getHttpProxyPort().isEmpty()) {
		HttpRequest::setUseProxy(true);
		if(!_advancedConfig.getHttpProxyUserID().isEmpty() && !_advancedConfig.getHttpProxyUserPassword().isEmpty()) {
			HttpRequest::setProxy(_advancedConfig.getHttpProxyUrl(), _advancedConfig.getHttpProxyPort().toInt(),
				_advancedConfig.getHttpProxyUserID(), _advancedConfig.getHttpProxyUserPassword());
			HttpRequest::setUseProxyAuthentication(true);
		} else {
			HttpRequest::setProxy(_advancedConfig.getHttpProxyUrl(), _advancedConfig.getHttpProxyPort().toInt(),
				"", "");
			HttpRequest::setUseProxyAuthentication(false);
		}
	} else {
		HttpRequest::setUseProxy(false);
		HttpRequest::setUseProxyAuthentication(false);
	}
	
	//FIXME remove it when auto will be made
	if (_advancedConfig.getNat() == AdvancedConfig::AUTOMATIC) {
		_advancedConfig.defaultSettings();
	}
	apply();
}

/**
 * @brief apply changes
 */
void AdvancedConfigWidget::apply() {
	emit askForInit();
}

/**
 * @param natType to translare
 * @brief translate the name of a nat type
 * into an index for the combo box
 * @return index for combo box
 */
AdvancedConfigWidget::index AdvancedConfigWidget::nameToIndex(const QString & natType) {
	for (unsigned short i = 0; i < AdvancedConfig::NB_NAT_CONFIG; ++i) {
		if (natType == tc_nat_type[i].name) {
			return tc_nat_type[i].index;
		}
	}
	assert(NULL && "Unknown NAT Configuration");
	return AdvancedConfigWidget::AUTOMATIC_INDEX;
} 

/**
 * @param index of the combo box
 * @brief translate an index
 * into a name of nat type
 */
const char * AdvancedConfigWidget::indexToName(int index) {
	for (unsigned short i = 0; i < AdvancedConfig::NB_NAT_CONFIG; ++i) {
		if (index == tc_nat_type[i].index) {
			return tc_nat_type[i].name;
		}
	}
	assert(NULL && "Unknown index");
	return AdvancedConfig::NONE;
}

void AdvancedConfigWidget::setUseHttp(bool ) {
	_advancedConfig.setUseHttpProxy(_useHttpCheckBox->isChecked());
}

