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

#include "CSoftUpdate.h"

#include <model/webservices/softupdate/WsSoftUpdate.h>
#include <presentation/PFactory.h>
#include <presentation/PSoftUpdate.h>

#include <util/SafeDelete.h>

CSoftUpdate::CSoftUpdate(WsSoftUpdate & wsSoftUpdate, CWengoPhone & cWengoPhone)
	: _wsSoftUpdate(wsSoftUpdate),
	_cWengoPhone(cWengoPhone) {

	_pSoftUpdate = PFactory::getFactory().createPresentationSoftUpdate(*this);

	_wsSoftUpdate.updateWengoPhoneEvent +=
		boost::bind(&CSoftUpdate::updateWengoPhoneEventHandler, this, _1, _2, _3, _4, _5);
}

CSoftUpdate::~CSoftUpdate() {
	OWSAFE_DELETE(_pSoftUpdate);
}

void CSoftUpdate::updateWengoPhoneEventHandler(WsSoftUpdate & sender,
				const std::string & downloadUrl,
				unsigned long long buildId,
				const std::string & version,
				unsigned fileSize) {

	_pSoftUpdate->updateWengoPhoneEvent(downloadUrl, buildId, version, fileSize);
}

void CSoftUpdate::checkForUpdate() {
	_wsSoftUpdate.checkForUpdate();
}
