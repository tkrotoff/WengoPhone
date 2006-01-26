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

#ifndef NULLIMCHAT_H
#define NULLIMCHAT_H

#include "NullIMFactory.h"

#include <model/imwrapper/IMChat.h>

class IMAccount;

/**
 * Null IM chat (stub).
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class NullIMChat : public IMChat {
	friend class NullIMFactory;
public:

	int sendMessage(const std::string & to, const std::string & message);

private:

	NullIMChat(IMAccount & account);
};

#endif	//NULLIMCHAT_H
