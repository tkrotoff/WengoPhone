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

#include "QtAbout.h"

#include <WengoPhoneBuildId.h>

#include <qtutil/Object.h>
#include <qtutil/WidgetFactory.h>

#include <QtGui>

QtAbout::QtAbout(QWidget * parent) : QObject() {
	_aboutWindow = WidgetFactory::create(":/forms/AboutWindow.ui", parent);

	QLabel * wengoPhoneBuildIdStringLabel = Object::findChild<QLabel *>(_aboutWindow, "wengoPhoneBuildIdStringLabel");
	wengoPhoneBuildIdStringLabel->setText(WengoPhoneBuildId::SOFTPHONE_NAME + QString(" ") +
					WengoPhoneBuildId::VERSION +
					QString(" rev") + QString::number(WengoPhoneBuildId::REVISION) +
					QString("-") + QString::number(WengoPhoneBuildId::BUILDID));
}
