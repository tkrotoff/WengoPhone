/*
 *  serialize-test.cpp
 *  WengoPhoneNG
 *
 *  Created by Philippe BERNERY on 16/03/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <imwrapper/IMAccountHandler.h>
#include <imwrapper/IMAccountParameters.h>

#include <SettingsXMLSerializer.h>

#include <fstream>
#include <sstream>

using namespace std;

int main(int /*argc*/, char ** /*argv*/) {

	IMAccountHandler handler;
	IMAccount account1("login1", "pass1", EnumIMProtocol::IMProtocolMSN);
	account1.getIMAccountParameters().set(IMAccountParameters::MSN_USE_HTTP_KEY, true);

	handler.insert(account1);
   	handler.insert(IMAccount("login2", "pass2", EnumIMProtocol::IMProtocolYahoo));

	handler.save("test.xml");

	IMAccountHandler handler2;
	handler2.load("test.xml");
	handler2.save("test2.xml");

	Settings settings;
	settings.set("hoho.riri", string("popo"));
	settings.set("hoho.fifi", true);
	settings.set("hoho.loulou", 123);

	StringList list;
	list += "hello";
	list += "hihi";
	list += "hoho";
	settings.set("mysuperstrlist", list);

	ofstream of("test2.xml");
	SettingsXMLSerializer serializer(settings);
	of << serializer.serialize();
	of.close();

	Settings settings2;
	stringstream data;
	ifstream ifile("test2.xml");
	ifile.get(*data.rdbuf(), EOF);
	ifile.close();
	SettingsXMLSerializer serializer2(settings2);
	serializer2.unserialize(data.str());

	ofstream of2("test3.xml");
	SettingsXMLSerializer serializer3(settings2);
	of2 << serializer3.serialize();
	of2.close();

	return 0;
}
