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

#include "CWengoPhoneLogger.h"

#include "model/WengoPhoneLogger.h"
#include "presentation/PFactory.h"
#include "presentation/PWengoPhoneLogger.h"
#include "CWengoPhone.h"

CWengoPhoneLogger::CWengoPhoneLogger(WengoPhoneLogger & logger, CWengoPhone & cWengoPhone)
	: _logger(logger),
	_cWengoPhone(cWengoPhone) {

	_pWengoPhoneLogger = PFactory::getFactory().createPresentationLogger(*this);

	_logger.messageAddedEvent += boost::bind(&CWengoPhoneLogger::messageAddedEventHandler, this, _1);
}

void CWengoPhoneLogger::messageAddedEventHandler(const std::string & message) {
	//Presentation is maybe not implemented
	if (_pWengoPhoneLogger) {
		_pWengoPhoneLogger->addMessage(message);
	}
}
