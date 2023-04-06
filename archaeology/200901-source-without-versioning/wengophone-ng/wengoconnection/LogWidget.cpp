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

#include "LogWidget.h"

#include "ui_LogWidget.h"

#include <coipmanager_threaded/TCoIpManager.h>
#include <coipmanager_threaded/connectmanager/TConnectManager.h>

#include <util/SafeConnect.h>

LogWidget::LogWidget(TCoIpManager &tCoIpManager, const std::string &accountId)
	: _tCoIpManager(tCoIpManager), _accountId(accountId) {

	_ui = new Ui::LogWidget();
	_ui->setupUi(this);

	SAFE_CONNECT(&_tCoIpManager.getTConnectManager(),
		SIGNAL(accountConnectionStateSignal(std::string, EnumConnectionState::ConnectionState)),
		SLOT(accountConnectionStateSlot(std::string, EnumConnectionState::ConnectionState)));
	SAFE_CONNECT(&_tCoIpManager.getTConnectManager(),
		SIGNAL(accountConnectionErrorSignal(std::string, EnumConnectionError::ConnectionError)),
		SLOT(accountConnectionErrorSlot(std::string, EnumConnectionError::ConnectionError)));
	SAFE_CONNECT(&_tCoIpManager.getTConnectManager(),
		SIGNAL(accountConnectionProgressSignal(std::string, unsigned, unsigned, std::string)),
		SLOT(accountConnectionProgressSlot(std::string, unsigned, unsigned, std::string)));

	_tCoIpManager.getTConnectManager().connect(_accountId);
}

LogWidget::~LogWidget() {
	OWSAFE_DELETE(_ui);
}

void LogWidget::accountConnectionStateSlot(std::string accountId,
	EnumConnectionState::ConnectionState state) {

	if (accountId == _accountId) {
		QString msg = "** State change: %1";
		msg = msg.arg(QString::fromStdString(EnumConnectionState::toString(state)));

		_ui->logTextEdit->append(msg);
	}
}

void LogWidget::accountConnectionErrorSlot(std::string accountId,
	EnumConnectionError::ConnectionError errorCode) {

	if (accountId == _accountId) {
		QString msg = "!! Error: %1";
		msg = msg.arg(QString::fromStdString(EnumConnectionError::toString(errorCode)));

		_ui->logTextEdit->append(msg);
	}
}

void LogWidget::accountConnectionProgressSlot(std::string accountId, unsigned currentStep,
	unsigned totalSteps, std::string infoMessage) {

	if (accountId == _accountId) {
		QString msg = "** Connection progress: step %1/%2 => %3";
		msg = msg.arg(currentStep);
		msg = msg.arg(totalSteps);
		msg = msg.arg(QString::fromStdString(infoMessage));

		_ui->logTextEdit->append(msg);
	}
}
