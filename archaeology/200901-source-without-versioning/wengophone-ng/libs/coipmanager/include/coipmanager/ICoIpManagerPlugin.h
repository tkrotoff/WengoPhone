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

#ifndef OWICOIPMANAGERPLUGIN_H
#define OWICOIPMANAGERPLUGIN_H

#include <coipmanager/ICoIpManager.h>
#include <coipmanager_base/EnumAccountType.h>

#include <QtCore/QObject>

#include <vector>

class Account;
class Contact;

/**
 * Interface for managers implementation.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class COIPMANAGER_API ICoIpManagerPlugin : public QObject, NonCopyable {
public:

	/**
	 * Default constructor for all managers.
	 *
	 * @param coIpManager link to the CoIpManager
	 */
	ICoIpManagerPlugin(CoIpManager & coIpManager);

	virtual ~ICoIpManagerPlugin();

	/**
	 * @return true if the accountType is supported by this ICoIpManager.
	 */
	bool isProtocolSupported(EnumAccountType::AccountType accountType) const;

	virtual void initialize();

	virtual void uninitialize();

protected:

	/** List of supported protocols. */
	std::vector<EnumAccountType::AccountType> _supportedAccountType;

	CoIpManager & _coIpManager;

};

#endif //OWICOIPMANAGERPLUGIN_H
