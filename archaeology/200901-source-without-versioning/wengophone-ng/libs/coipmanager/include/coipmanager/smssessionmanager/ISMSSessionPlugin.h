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

#ifndef OWISMSSESSIONPLUGIN_H
#define OWISMSSESSIONPLUGIN_H

#include <coipmanager/session/ISession.h>
#include <coipmanager/smssessionmanager/EnumSMSState.h>
#include <coipmanager/smssessionmanager/ISMSSession.h>

/**
 * ISMSSessionPlugin proxy class.
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 */
class COIPMANAGER_API ISMSSessionPlugin : public ISession, public ISMSSession {
	Q_OBJECT
public:

	ISMSSessionPlugin(CoIpManager & coIpManager);

	virtual ~ISMSSessionPlugin();

Q_SIGNALS:

	/** Emitted after SMS send to get send result. */
	void smsStateSignal(std::string phoneNumber, EnumSMSState::SMSState state);

public Q_SLOTS:

	// Inherited from Session
	virtual void stop();

	virtual void pause();

	virtual void resume();
	////

private:

	/** This methods is replaced by addPhoneNumber. */
	virtual void addIMContact(const IMContact & contact);

	/** Cannot remove IMContact in this kind of Session. */
	virtual void removeIMContact(const IMContact & contact);

};

#endif //OWISMSSESSIONPLUGIN_H
