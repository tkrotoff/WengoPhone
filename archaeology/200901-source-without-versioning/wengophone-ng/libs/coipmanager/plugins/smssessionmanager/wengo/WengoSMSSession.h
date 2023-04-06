/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007 Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef OWWENGOSMSSESSION_H
#define OWWENGOSMSSESSION_H

#include <coipmanager/smssessionmanager/ISMSSessionPlugin.h>

#include <webservice/WengoWebService.h>

#include <map>

/**
 * Wengo implementation of ISMSSessionPlugin.
 *
 * @author Philippe Bernery
 */
class WengoSMSSession : public ISMSSessionPlugin, public WengoWebService {
public:

	WengoSMSSession(CoIpManager & coIpManager, NetworkProxy networkProxy);

	virtual ~WengoSMSSession();

public Q_SLOTS:

	// Inherited from Session
	virtual void start();
	////

private:

	virtual void answerReceived(int requestId, const std::string & answer);

	/** Map with <requestId, phoneNumber>. */
	std::map<int, std::string> _requestIdMap;

};

#endif //OWWENGOSMSSESSION_H
