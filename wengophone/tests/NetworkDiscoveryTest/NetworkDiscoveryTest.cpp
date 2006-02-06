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

#include <model/WengoPhone.h>

#include <model/imwrapper/IMWrapperFactory.h>
#include <model/sipwrapper/SipWrapperFactory.h>

#include <model/sipwrapper/phapi/PhApiFactory.h>

#include <global.h>

#if defined(CC_MSVC)
	#include <win32/MemoryDump.h>
#endif

#include <string>
#include <iostream>
using namespace std;

WengoPhone * wengoPhone = NULL;

void wengoLoginEventHandler(WengoPhone & sender, WengoPhone::LoginState state, const std::string & login, const std::string & password) {
		if (state == WengoPhone::LoginNoAccount) {
			sender.addWengoAccount("jerome.leleu@wengo.fr", "b1234b", true);
		}
}

void proxyNeedsAuthenticationEvent(NetworkDiscovery & sender, const std::string & proxyUrl, int proxyPort) {
	string url = proxyUrl;
	int port = proxyPort;
	string login;
	string password;
	string input;

	cout << "Proxy settings needed for " << proxyUrl << ":" << proxyPort << endl;
	cout << "Is proxy URL and proxy port valid? (y, n)" << endl;
	cin >> input;
	if ((input == "y") || (input == "Y") || (input == "o") || (input == "O")) {
		cout << "Please enter proxy url: ";
		cin >> url;
		cout << "Please enter proxy port: ";
		cin >> port;
	}
	cout << "Please enter proxy login: ";
	cin >> login;
	cout << "Please enter proxy password: ";
	cin >> password;
	
	wengoPhone->setProxySettings(url, port, login, password);
}


int main(int argc, char * argv[]) {

#if defined(CC_MSVC)
	new MemoryDump("WengoPhoneNG");
#endif

	//IM implementation
	IMWrapperFactory * imFactory = NULL;

	//SIP implementation
	SipWrapperFactory * sipFactory = NULL;


	PhApiFactory * phApiFactory = new PhApiFactory();
	sipFactory = phApiFactory;
	imFactory = phApiFactory;

	SipWrapperFactory::setFactory(sipFactory);
	IMWrapperFactory::setFactory(imFactory);

	wengoPhone = new WengoPhone();

	wengoPhone->wengoLoginEvent += &wengoLoginEventHandler;
	wengoPhone->getNetworkDiscovery().proxyNeedsAuthenticationEvent += &proxyNeedsAuthenticationEvent;

	wengoPhone->run();

	wengoPhone->join();

	return EXIT_SUCCESS;
}
