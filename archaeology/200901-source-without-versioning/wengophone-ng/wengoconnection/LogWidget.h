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

#ifndef OWLOGWIDGET_H
#define OWLOGWIDGET_H

#include <coipmanager/connectmanager/EnumConnectionError.h>

#include <coipmanager_base/EnumConnectionState.h>

#include <QtGui/QWidget>

#include <string>

class TCoIpManager;

namespace Ui { class LogWidget; }

/**
 * Test program for Connection service of CoIpManager.
 *
 * @author Philippe Bernery
 */
class LogWidget : public QWidget {
	Q_OBJECT
public:

	LogWidget(TCoIpManager &tCoIpManager, const std::string & accountId);

	~LogWidget();

private Q_SLOTS:

	void accountConnectionStateSlot(std::string accountId,
		EnumConnectionState::ConnectionState state);

	void accountConnectionErrorSlot(std::string accountId,
		EnumConnectionError::ConnectionError errorCode);

	void accountConnectionProgressSlot(std::string accountId, unsigned currentStep,
		unsigned totalSteps, std::string infoMessage);

private:

	Ui::LogWidget *_ui;

	TCoIpManager &_tCoIpManager;

	std::string _accountId;
};

#endif //OWLOGWIDGET_H
