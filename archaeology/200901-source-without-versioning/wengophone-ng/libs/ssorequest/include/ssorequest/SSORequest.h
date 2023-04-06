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

#ifndef OWSSOREQUEST_H
#define OWSSOREQUEST_H

#include <ssorequest/ssorequestdll.h>

#include <webservice/WengoWebService.h>

#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>

/**
 * Make a SSO (Single Sign On) request on the Wengo platform.
 *
 * @author Mathieu Stute
 * @author Philippe Bernery
 */
class SSOREQUEST_API SSORequest : public WengoWebService {
public:

	enum SSORequestError {
		/** Request emitted and answer received and no error occured. */
		SSORequestErrorOk,

		/** Request emitted and answer received but invalid login/password set. */
		SSORequestErrorInvalidLoginPassword,

		/** Request emitted but no answer received. */
		SSORequestErrorCannotConnect,
	};

	SSORequest(const NetworkProxy & networkProxy, WengoAccount & wengoAccount);

	virtual ~SSORequest();

	/** Launch the SSO request and blocks until result. */
	SSORequestError launch();

protected:

	virtual void answerReceived(int requestId, const std::string & answer);

	SSORequestError _lastError;

	QWaitCondition _condition;

	QMutex _mutex;
};

#endif	//OWSSOREQUEST_H
