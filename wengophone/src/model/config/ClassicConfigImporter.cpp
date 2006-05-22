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
 
#include "ClassicConfigImporter.h"


#include "ConfigManager.h"
#include "Config.h"

#include <cutil/global.h>
#include <util/Path.h>
#include <util/File.h>
#include <util/Logger.h>
#include <util/String.h>
#include <model/account/wengo/WengoAccount.h>
#include <model/account/wengo/WengoAccountDataLayer.h>
#include <model/account/wengo/WengoAccountXMLLayer.h>
#include <model/profile/UserProfile.h>
#include <model/WengoPhone.h>

using namespace std;

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

typedef struct telNumber_s
{
	string	key;
	string	value;
}			telNumber_t;

typedef std::list<telNumber_t *> telNumberList;
typedef std::list<telNumber_t *>::iterator telNumberIt;

typedef struct address_s
{
	string	street;
	string	city;
	string	post_code;
	string	state;
	string	country;
}			address_t;

typedef struct birthday_s
{
	int	day;
	int	month;
	int year;
}		birthday_t;

#define MALE	0
#define FEMALE	1
#define	UNKNOWN	2

typedef struct vcard_s
{
	string			id;
	string			fname;
	string			lname;
	int				gender;
	string			company;
	string			website;
	birthday_t		birthday;
	string			note;
	address_t		address;
	telNumberList	numbers;
	StringList		emails;
	string			owner;
	bool			blocked;
}					vcard_t;

typedef std::list<vcard_t *> vcardList;
typedef std::list<vcard_t *>::iterator vcardIt;

telNumber_t *CreateNewNodeNumber(const std::string & key, const std::string & value)
{
	telNumber_t *number = new telNumber_t();
	
	memset(number, 0, sizeof(telNumber_t));
	number->key = key;
	number->value = value;

	return number;
}

StringList mySplit(const std::string & str, char sep)
{
	string			word;
	StringList		wordList;
	istringstream	strStream(str);
	
	while (std::getline(strStream, word, sep))
		wordList += word;

	return wordList;
}

string myTrim(std::string str)
{
	string newstr;

	string::size_type pos1 = str.find_first_not_of(' ');
	string::size_type pos2 = str.find_last_not_of(' ');
	newstr = str.substr(pos1 == string::npos ? 0 : pos1, 
		pos2 == string::npos ? str.length() - 1 : pos2 - pos1 + 1);

	return newstr;
}

#define CONFIG_UNKNOWN	0
#define CONFIG_VERSION1 1
#define CONFIG_VERSION2 2
#define CONFIG_VERSION3 3

ClassicConfigImporter::ClassicConfigImporter(WengoPhone & wengoPhone) 
: _wengoPhone(wengoPhone), _userProfile(wengoPhone.getCurrentUserProfile()) {
}

bool ClassicConfigImporter::importConfig(string str) {

#if (defined(OS_WINDOWS) || defined(OS_LINUX))

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	int localVersion = detectLastVersion();

	if (localVersion != CONFIG_UNKNOWN && localVersion < config.CONFIG_VERSION) {
		makeImportConfig(localVersion, config.CONFIG_VERSION);
		return true;
	}

#endif

	return false;
}

int ClassicConfigImporter::detectLastVersion()
{
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	string ConfigPathV1 = getWengoClassicConfigPath();
	string ConfigPathV2 = config.getConfigDir();
	bool dirV1Exists = File::exists(ConfigPathV1.substr(0, ConfigPathV1.size() - 1));
	bool dirV2Exists = File::exists(ConfigPathV2.substr(0, ConfigPathV2.size() - 1));

	if (dirV2Exists)
	{
		if (File::exists(profilesPath + "profiles"))
			return CONFIG_VERSION3;
		else if (File::exists(ConfigPathV2 + "contactlist.xml"))
			return CONFIG_VERSION2;
		else if (dirV1Exists)
			return CONFIG_VERSION1;
	}
	else if (dirV1Exists)
		return CONFIG_VERSION1;

	return CONFIG_UNKNOWN;
}

void ClassicConfigImporter::makeImportConfig(int from, int to) 
{
	if (from == CONFIG_VERSION1 && to == CONFIG_VERSION2)
		ImportConfigFromV1toV2();
	else if (from == CONFIG_VERSION1 && to == CONFIG_VERSION3)
		ImportConfigFromV1toV3();
	
	// Todo: import config files from version 2 to version 3
	//else if (from == CONFIG_VERSION2 && to == CONFIG_VERSION3)
	//	ImportConfigFromV2toV3();

}

string ClassicConfigImporter::getWengoClassicConfigPath() {
	string result;

#if defined(OS_WINDOWS)

	result = File::convertPathSeparators(Path::getHomeDirPath() + "wengo/");

#elif defined(OS_LINUX)

	result = File::convertPathSeparators(Path::getHomeDirPath() + ".wengo/");

#endif

	return result;
}

bool ClassicConfigImporter::ClassicVcardParser(const string & vcardFile, void *structVcard)
{
	vcard_t *mVcard = (vcard_t *) structVcard;
	std::ifstream fileStream;
	string lastLine;
	
	fileStream.open(vcardFile.c_str());
	if (!fileStream) 
	{
		LOG_ERROR("cannot open the file: " + vcardFile);
		return false;
	}
	
	std::getline(fileStream, lastLine);
	if (lastLine != "BEGIN:VCARD")
	{
		fileStream.close();
		return false;
	}

	string key, value, tmp;

	std::getline(fileStream, tmp);

	while (!lastLine.empty()) 
	{
		int pos = lastLine.find(":", 0);
		key = lastLine.substr(0, pos);
		value = lastLine.substr(pos + 1, lastLine.length() - (pos + 1));

		if (!key.compare("N"))
		{
			StringList mList = mySplit(value, ';');
			mVcard->lname = mList[0];
			mVcard->fname = mList[1];

			if (!mList[4].compare("Mme."))
				mVcard->gender = FEMALE;
			else if (!mList[4].compare("Mr."))
				mVcard->gender = MALE;
			else
				mVcard->gender = UNKNOWN;
		}
		else if (!key.compare("TEL;TYPE=home"))
			mVcard->numbers.push_back(CreateNewNodeNumber("home", value));

		else if (!key.compare("TEL;TYPE=work"))
			mVcard->numbers.push_back(CreateNewNodeNumber("work", value));

		else if (!key.compare("TEL;TYPE=cell"))
			mVcard->numbers.push_back(CreateNewNodeNumber("cell", value));
		
		else if (!key.compare("TEL;TYPE=pref"))
			mVcard->numbers.push_back(CreateNewNodeNumber("pref", value));

		else if (!key.compare("TEL;TYPE=fax"))
			mVcard->numbers.push_back(CreateNewNodeNumber("fax", value));
		
		else if (!key.compare("TEL;TYPE=other"))
			mVcard->numbers.push_back(CreateNewNodeNumber("other", value));

		else if (!key.compare("EMAIL"))
			mVcard->emails += value;

		else if (!key.compare("ORG"))
			mVcard->company = value;

		else if (!key.compare("URL"))
			mVcard->website = value;

		else if (!key.compare("BDAY"))
		{
			StringList mList = mySplit(value, '-');
			mVcard->birthday.year = atoi(mList[0].c_str());
			mVcard->birthday.month = atoi(mList[1].c_str());
			mVcard->birthday.day = atoi(mList[2].c_str());
		}
		else if (!key.compare("NOTE"))
			mVcard->note = value;

		else if (!key.compare("ADR;TYPE=home;TYPE=pref"))
		{
			StringList mList = mySplit(value, ';');
			mVcard->address.street = mList[2];
			mVcard->address.city = mList[3];
			mVcard->address.state = mList[4];
			mVcard->address.post_code = mList[5];
			mVcard->address.country = mList[6];
		}
		else
			LOG_DEBUG("KEY " + key + " not supported");

		lastLine = myTrim(tmp);
		std::getline(fileStream, tmp);

		if (!tmp.empty())
		{
			if (tmp.find(":", 0) == -1)
			{
				lastLine += myTrim(tmp);
				std::getline(fileStream, tmp);
			}
		}
	}

	fileStream.close();
	return true;
}

bool ClassicConfigImporter::ClassicXMLParser(const string & xmlFile, void *structVcard)
{
	vcard_t *mVcard = (vcard_t *) structVcard;
	std::ifstream fileStream;
	string lastLine;

	mVcard->blocked = false;
	
	fileStream.open(xmlFile.c_str());
	if (!fileStream) 
	{
		LOG_ERROR("cannot open the file: " + xmlFile);
		return false;
	}

	std::getline(fileStream, lastLine);

	while (!lastLine.empty())
	{
		lastLine = myTrim(lastLine);

		if (!strncmp(lastLine.c_str(), "<blocked>", 9))
		{
			int pos1 = lastLine.find_first_of('>');
			int pos2 = lastLine.find_last_of('<');
			string resp = myTrim(lastLine.substr(pos1 + 1, pos2 - (pos1 + 1) ));

			if (resp == "true")
				mVcard->blocked = true;
		}

		std::getline(fileStream, lastLine);
	}

	fileStream.close();
	return true;
}

string ClassicConfigImporter::ClassicVCardToString(void *structVcard)
{
	vcard_t *mVcard = (vcard_t *) structVcard;
	string res = "<wgcard version=\"1.0\" xmlns=\"http://www.openwengo.org/wgcard/1.0\">\n";

	// Todo: look at ProfileXMLSerializer and try to use the same serializer
	if (!mVcard->id.empty())
		res += ("<wengoid>" + mVcard->id + "</wengoid>\n");

	res += "<name>\n";
	if (!mVcard->fname.empty()) 
		res += ("<first><![CDATA[" + mVcard->fname + "]]></first>\n");
	if (!mVcard->lname.empty()) 
		res += ("<last><![CDATA[" + mVcard->lname + "]]></last>\n");
	res += "</name>\n";

	if (mVcard->gender == MALE)
		res += ("<sex>male</sex>\n");
	else if (mVcard->gender == FEMALE)
		res += ("<sex>female</sex>\n");

	if (!mVcard->website.empty())
		res += ("<url type=\"website\">" + mVcard->website + "</url>\n");

	if (mVcard->birthday.day && mVcard->birthday.month && mVcard->birthday.year)
	{
		res += "<birthday>\n<date>\n";
		res += ("<day>" + String::fromNumber(mVcard->birthday.day) + "</day>\n");
		res += ("<month>" + String::fromNumber(mVcard->birthday.month) + "</month>\n");
		res += ("<year>" + String::fromNumber(mVcard->birthday.year) + "</year>\n");
		res += "</date>\n</birthday>\n";
	}

	if (!mVcard->company.empty())
		res += "<organization>" + mVcard->company + "</organization>\n";

	telNumberIt it;
	for (it = mVcard->numbers.begin(); it != mVcard->numbers.end(); it++)
	{
		if (!(*it)->key.compare("home") && !((*it)->value.empty()))
			res += "<tel type=\"home\">" + (*it)->value + "</tel>\n";
		else if (!(*it)->key.compare("work") && !((*it)->value.empty()))
			res += "<tel type=\"work\">" + (*it)->value + "</tel>\n";
		else if (!(*it)->key.compare("cell") && !((*it)->value.empty()))
			res += "<tel type=\"cell\">" + (*it)->value + "</tel>\n";
		else if (!(*it)->key.compare("fax") && !((*it)->value.empty()))
			res += "<fax type=\"home\">" + (*it)->value + "</fax>\n";
	}

	res += "<address type=\"home\">\n";
	if (!mVcard->address.street.empty())
		res += ("<street><![CDATA[" + mVcard->address.street + "]]></street>\n");
	if (!mVcard->address.city.empty())
		res += ("<locality><![CDATA[" + mVcard->address.city + "]]></locality>\n");
	if (!mVcard->address.state.empty())
		res += ("<region><![CDATA[" + mVcard->address.state + "]]></region>\n");
	if (!mVcard->address.post_code.empty())
		res += ("<postcode><![CDATA[" + mVcard->address.post_code + "]]></postcode>\n");
	if (!mVcard->address.country.empty())
		res += ("<country><![CDATA[" + mVcard->address.country + "]]></country>\n");
	res += "</address>\n";

	if (!mVcard->id.empty())
	{
		res += "<im protocol=\"SIP/SIMPLE\">\n";
		res += ("<id>" + mVcard->id + "</id>\n");
		res += ("<account>" + mVcard->owner + "</account>");
		res += "</im>\n";
	}

	if (mVcard->emails.size() >= 1 && !mVcard->emails[0].empty())
		res += ("<email type=\"home\">" + mVcard->emails[0] + "</email>\n");
	if (mVcard->emails.size() >= 2 && !mVcard->emails[1].empty())
		res += ("<email type=\"work\">" + mVcard->emails[1] + "</email>\n");	
	if (mVcard->emails.size() >= 3 && !mVcard->emails[2].empty())
		res += ("<email type=\"other\">" + mVcard->emails[2] + "</email>\n");	

	if (!mVcard->note.empty())
		res += ("<notes><![CDATA[" + mVcard->note + "]]></notes>\n");

	res += ("<group><![CDATA[Wengo]]></group>\n");

	res += "</wgcard>\n";
	return res;
}


bool ClassicConfigImporter::ImportContactsFromV1toV3(const string & fromDir, 
	const string & toDir, const string & owner) {

	File mDir(fromDir);
	StringList fileList = mDir.getFileList();
	vcardList vList;

	for (int i = 0; i < fileList.size(); i++)
	{
		File mFile(fromDir + fileList[i]);
		string Id = fileList[i].substr(0, fileList[i].find("_", 0));
		vcard_t *mVcard;

		if (!mFile.getExtension().compare("vcf"))
		{
			mVcard = new vcard_t();
			mVcard->id = Id;
			mVcard->owner = owner;
			
			if (ClassicVcardParser(fromDir + fileList[i], mVcard) == false)
			{
				delete mVcard;
				continue;
			}

			int extPos = fileList[i].find_last_of('.');
			string fileWoExt = fileList[i].substr(0, extPos + 1);
			
			ClassicXMLParser(fromDir + fileWoExt + "xml", mVcard);
			vList.push_back(mVcard);
		}
	}

	vcardIt	vIt;
	FileWriter xmlFile(toDir + "contactlist.xml");
	if (!xmlFile.open())
	{
		LOG_ERROR("cannot open the file: " + toDir + "contactlist.xml");
		return false;
	}
	
	// Write all informations to the xml conf file
	xmlFile.write("<contactlist>\n");
	for (vIt = vList.begin(); vIt != vList.end(); vIt++)
	{
		xmlFile.write(ClassicVCardToString((*vIt)));
	}
	xmlFile.write("</contactlist>\n");
	xmlFile.close();

	// Clean VCard list
	vcardIt	vIt2;
	telNumberIt tIt;
	telNumberIt tIt2;
	for (vIt = vList.begin(); vIt != vList.end(); )
	{
		vIt2 = vIt++;
		for (tIt = (*vIt2)->numbers.begin(); tIt != (*vIt2)->numbers.end(); )
		{
			tIt2 = tIt++;
			(*vIt2)->numbers.erase(tIt2);
		}
		vList.erase(vIt2);
	}

	return true;
}

typedef struct last_user_s
{
	string	login;
	string	password;
	bool	auto_login;
}			last_user_t;

void * ClassicConfigImporter::GetLastClassicWengoUser() {
	string classicPath = getWengoClassicConfigPath();
	std::ifstream fileStream;
	std::string lastLine;

	last_user_t * lastUser = new last_user_t();
	fileStream.open((classicPath +	"user.config").c_str());
	if (!fileStream) 
	{
		LOG_ERROR("cannot open the file: " + (classicPath +	"user.config"));
		return NULL;
	}

	std::getline(fileStream, lastLine);

	while (!lastLine.empty())
	{
		lastLine = myTrim(lastLine);

		if (!strncmp(lastLine.c_str(), "<login>", 7))
		{
			int pos2 = lastLine.find_first_of(']');
			int pos1 = lastLine.find_last_of('[');
			lastUser->login = myTrim(lastLine.substr(pos1 + 1, pos2 - (pos1 + 1) ));
		}
		else if (!strncmp(lastLine.c_str(), "<password>", 10))
		{
			int pos2 = lastLine.find_first_of(']');
			int pos1 = lastLine.find_last_of('[');
			lastUser->password = myTrim(lastLine.substr(pos1 + 1, pos2 - (pos1 + 1) ));
		}
		else if (!strncmp(lastLine.c_str(), "<autoLogin>", 11))
		{
			int pos1 = lastLine.find_first_of('>');
			int pos2 = lastLine.find_last_of('<');
			string resp = myTrim(lastLine.substr(pos1 + 1, pos2 - (pos1 + 1) ));

			if (resp == "true")
				lastUser->auto_login = true;
			else
				lastUser->auto_login = false;
		}

		std::getline(fileStream, lastLine);
	}

	fileStream.close();
	return lastUser;
}

bool ClassicConfigImporter::ImportConfigFromV1toV2() {

	last_user_t * lastUser = (last_user_t *) GetLastClassicWengoUser();
	if (lastUser == NULL)
		return false;

	//WengoAccount wAccount(lastUser->login, lastUser->password, lastUser->auto_login);
	//WengoAccountDataLayer * wAccountDL = new WengoAccountXMLLayer(wAccount);
	//WengoAccountDataLayer wAccountDL(wAccount);
	_userProfile.loginStateChangedEvent +=
		boost::bind(&ClassicConfigImporter::loginStateChangedEventHandler, this, _1, _2);
	_userProfile.addSipAccount(lastUser->login, lastUser->password, lastUser->auto_login);
	//wAccountDL->save();
	
	return true;
}

void ClassicConfigImporter::loginStateChangedEventHandler(SipAccount & sender, SipAccount::LoginState state) {
	switch (state) {
	case SipAccount::LoginStateReady:
		try {
			_userProfile.loginStateChangedEvent -=
				boost::bind(&ClassicConfigImporter::loginStateChangedEventHandler, this, _1, _2);

			const WengoAccount & wengoAccount = dynamic_cast<const WengoAccount &>(sender);
			IMAccount imAccount(wengoAccount.getIdentity(), 
				wengoAccount.getPassword(), EnumIMProtocol::IMProtocolSIPSIMPLE);
			_userProfile.addIMAccount(imAccount);
			_wengoPhone.saveUserProfile();

			Config & config = ConfigManager::getInstance().getCurrentConfig();
			string confV1 = getWengoClassicConfigPath();
			string confV2 = config.getConfigDir();
			string sep = File::getPathSeparator();

			string contactsPathV1 = confV1 + wengoAccount.getWengoLogin() + sep + "contacts" + sep;
			ImportContactsFromV1toV3(contactsPathV1, confV2, wengoAccount.getIdentity());

		} catch (bad_cast) {
			LOG_DEBUG("can't cast the SipAccount to a WengoAccount");
		}

	default:
		;
	}
}

bool ClassicConfigImporter::ImportConfigFromV1toV3() {

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	string classicPath = getWengoClassicConfigPath();
	File mDir(classicPath);
	StringList dirList = mDir.getDirectoryList();
	string sep = mDir.getPathSeparator();

	for (int i = 0; i < dirList.size(); i++)
	{
		String newDir(config.getConfigDir() + sep + "profiles" + sep + dirList[i] + sep);
		File::createPath(newDir);
		string path = classicPath + dirList[i] + sep + "contacts" + sep;
		ImportContactsFromV1toV3(path, newDir, dirList[i]);
	}

	return true;
}