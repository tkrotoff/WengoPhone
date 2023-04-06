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

#include "VCardWriter.h"
using namespace database;

#include "contact/Contact.h"
#include "Target.h"

#include <VCard.h>
using namespace VCARD;

#include <qfile.h>
#include <qdatetime.h>
#include <qurl.h>
#include <qstring.h>
#include <qdir.h>

#include <iostream>
using namespace std;

VCardWriter::VCardWriter(const Contact & contact) {
	/*
	nickname
	first name
	last name
	sex
	birthdate
	website

	phone wengo
	phone mobile
	phone home
	phone work
	phone other
	phone fax

	mail personal
	mail work
	mail other

	address
	street
	city
	zip code
	state/province
	country

	picture
	group

	note
	*/

	VCard vcard;

	addNValue(vcard, contact);
	addTextValue(vcard, EntityFullName, contact.getFullName());
	//addTextValue(vcard, EntityName, contact.getFirstName());

	addTelephoneValue(vcard, contact);

	addTextValue(vcard, EntityEmail, contact.getPersonalEmail());
	addTextValue(vcard, EntityEmail, contact.getWorkEmail());
	addTextValue(vcard, EntityEmail, contact.getOtherEmail());

	addTextValue(vcard, EntityOrganisation, contact.getCompany());

	addTextValue(vcard, EntityURL, contact.getWebsite());

	addDateValue(vcard, EntityBirthday, contact.getBirthdate());
	addTextValue(vcard, EntityNote, contact.getNotes());

	addAddressValue(vcard, contact.getStreetAddress());

	_data = vcard.asString();
}

void VCardWriter::addTextValue(VCard & vcard, EntityType type, const QString & txt) {
	if (txt.isEmpty()) return;

	ContentLine cl;
	cl.setName(EntityTypeToParamName(type));
	cl.setValue(new TextValue(txt.utf8()));
	vcard.add(cl);
}

void VCardWriter::addDateValue(VCard & vcard, EntityType type, const QDate & date) {
	if (!date.isValid()) return;

	ContentLine cl;
	cl.setName(EntityTypeToParamName(type));
	DateValue * v = new DateValue(date);
	cl.setValue(v);
	vcard.add(cl);
}

void VCardWriter::addNValue(VCard & vcard, const Contact & contact) {
	ContentLine cl;
	cl.setName(EntityTypeToParamName(EntityN));
	NValue * v = new NValue;
	v->setFamily(contact.getLastName().utf8());
	v->setGiven(contact.getFirstName().utf8());
	//v->setMiddle(contact.additionalName().utf8());
	//v->setPrefix(contact.prefix().utf8());
	if (contact.getSex() == Contact::Male) v->setSuffix("Mr.");
	if (contact.getSex() == Contact::Female) v->setSuffix("Mme.");

	cl.setValue(v);
	vcard.add(cl);
}

void VCardWriter::addTelephoneValue(VCard & vcard, const Contact & contact) {
	//FIXME factorize this ugly code

	ContentLine cl1;
	cl1.setName(EntityTypeToParamName(EntityTelephone));
	cl1.setValue(new TelValue(contact.getHomePhone().utf8()));
	ParamList params1;
	params1.append(new Param("TYPE", "home"));
	cl1.setParamList(params1);
	vcard.add(cl1);

	ContentLine cl2;
	cl2.setName(EntityTypeToParamName(EntityTelephone));
	cl2.setValue(new TelValue(contact.getWorkPhone().utf8()));
	ParamList params2;
	params2.append(new Param("TYPE", "work"));
	cl2.setParamList(params2);
	vcard.add(cl2);

	ContentLine cl3;
	cl3.setName(EntityTypeToParamName(EntityTelephone));
	cl3.setValue(new TelValue(contact.getMobilePhone().utf8()));
	ParamList params3;
	params3.append(new Param("TYPE", "cell"));
	cl3.setParamList(params3);
	vcard.add(cl3);

	ContentLine cl4;
	cl4.setName(EntityTypeToParamName(EntityTelephone));
	cl4.setValue(new TelValue(contact.getWengoPhone().utf8()));
	ParamList params4;
	params4.append(new Param("TYPE", "pref"));
	cl4.setParamList(params4);
	vcard.add(cl4);

	ContentLine cl5;
	cl5.setName(EntityTypeToParamName(EntityTelephone));
	cl5.setValue(new TelValue(contact.getFax().utf8()));
	ParamList params5;
	params5.append(new Param("TYPE", "fax"));
	cl5.setParamList(params5);
	vcard.add(cl5);

	ContentLine cl6;
	cl6.setName(EntityTypeToParamName(EntityTelephone));
	cl6.setValue(new TelValue(contact.getOtherPhone().utf8()));
	ParamList params6;
	params6.append(new Param("TYPE", "other"));
	cl6.setParamList(params6);
	vcard.add(cl6);
}

void VCardWriter::addAddressValue(VCard & vcard, const StreetAddress & address) {
	ContentLine cl;
	cl.setName(EntityTypeToParamName(EntityAddress));
	AdrValue * v = new AdrValue;
	//v->setPOBox(address.getStreet().utf8());
	//v->setExtAddress(address.extended().utf8());
	v->setStreet(address.getStreet().utf8());
	v->setLocality(address.getCity().utf8());
	v->setRegion(address.getStateProvince().utf8());
	v->setPostCode(address.getZipCode().utf8());
	v->setCountryName(address.getCountry().utf8());
	cl.setValue(v);
	ParamList params;
	params.append(new Param("TYPE", "home"));
	params.append(new Param("TYPE", "pref"));
	cl.setParamList(params);
	vcard.add(cl);
}

void VCardWriter::addCustomValue(VCard & vcard, const QString & txt) {
	if (txt.isEmpty()) return;

	ContentLine cl;
	cl.setName("X-" + txt.left(txt.find(":")).utf8());
	cl.setValue(new TextValue(txt.mid(txt.find(":") + 1).utf8()));
	vcard.add(cl);
}

/*
void VCardWriter::addPictureValue(VCard & vcard, EntityType type, const Picture & pic, const Addressee & addr, bool intern) {
	ContentLine cl;
	cl.setName(EntityTypeToParamName(type));

	if (pic.isIntern() && pic.data().isNull())
		return;

	if (!pic.isIntern() && pic.url().isEmpty())
		return;

	ParamList params;
	if (pic.isIntern()) {
		QImage img = pic.data();

		if (intern) {
			//only for vCard export we really write the data inline

			QByteArray data;
			QDataStream s(data, IO_WriteOnly);
			s.setVersion(4);	//to produce valid png files
			s << img;
			cl.setValue(new TextValue(KCodecs::base64Encode(data)));

			//char * VCARD::encodeBase64(const char * src, unsigned long srcl, unsigned long & destl)
		} else {
			//save picture in cache

			QString dir;
			if (type == EntityPhoto)
				dir = "photos";
			if (type == EntityLogo)
				dir = "logos";

			img.save(locateLocal("data", "kabc/" + dir + "/" + addr.uid()), pic.type().utf8());
			cl.setValue(new TextValue("<dummy>"));
		}
		params.append(new Param("ENCODING", "b"));
		if (!pic.type().isEmpty())
			params.append(new Param("TYPE", pic.type().utf8()));
	} else {
		cl.setValue(new TextValue(pic.url().utf8()));
		params.append(new Param("VALUE", "uri"));
	}

	cl.setParamList(params);
	vcard.add(cl);
}
*/
