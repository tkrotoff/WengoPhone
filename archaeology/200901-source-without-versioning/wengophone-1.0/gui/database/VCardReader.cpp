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

#include "VCardReader.h"
using namespace database;

#include "contact/Contact.h"
#include "Source.h"

#include <VCard.h>
using namespace VCARD;

#include <qdatetime.h>
#include <qurl.h>

#include <iostream>
using namespace std;

VCardReader::VCardReader(const QString & data) {
	_contact = new Contact();

	if (data.isEmpty()) {
		return;
	}

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

	VCardEntity e(data.utf8());
	VCardListIterator it(e.cardList());
	for (; it.current(); ++it) {
		VCard vcard(*it.current());

		QPtrList<ContentLine> contentLines = vcard.contentLineList();
		for (ContentLine * cl = contentLines.first(); cl; cl = contentLines.next()) {
			EntityType type = cl->entityType();

			switch(type) {

			case EntityUID:
				//contact.setUid(readTextValue(cl));
				break;

			case EntityEmail:
				if (_contact->getPersonalEmail().isEmpty()) {
					_contact->setPersonalEmail(readTextValue(cl));
				}
				else if (_contact->getWorkEmail().isEmpty()) {
					_contact->setWorkEmail(readTextValue(cl));
				}
				else if (_contact->getOtherEmail().isEmpty()) {
					_contact->setOtherEmail(readTextValue(cl));
				}
				break;

			case EntityURL:
				_contact->setWebsite(QUrl(readTextValue(cl)));
				break;

			case EntityOrganisation:
				_contact->setCompany(readTextValue(cl));
				break;

			case EntityNote:
				_contact->setNotes(readTextValue(cl));
				break;

			case EntityN:
				readNValue(cl, *_contact);
				break;

			case EntityAddress:
				_contact->setStreetAddress(readAddressValue(cl));
				break;

			case EntityTelephone:
				readTelephoneValue(cl, *_contact);
				break;

			case EntityBirthday:
				_contact->setBirthdate(readDateValue(cl));
				break;

			case EntityVersion:
				break;

			default:
				cerr << "readvcard.cpp: Unsupported entity: "
					<< int(type) << ": " << cl->asString() << endl;
				break;
			}
		}
	}
}

QString VCardReader::readTextValue(ContentLine * cl) {
	if (!cl) {
		return QString::null;
	}

	Value * value = cl->value();
	if (value) {
		return QString::fromUtf8(value->asString());
	} else {
		cerr << "No value: " << cl->asString() << endl;
		return QString::null;
	}
}

QDate VCardReader::readDateValue(ContentLine * cl) {
	if (!cl) {
		return QDate();
	}

	DateValue * dateValue = (DateValue *) cl->value();
	if (!dateValue) {
		return QDate();
	}

	return dateValue->qdate();
}

void VCardReader::readNValue(ContentLine * cl, Contact & contact) {
	if (!cl) {
		return;
	}

	NValue * v = (NValue *) cl->value();
	if (!v) {
		return;
	}

	contact.setLastName(QString::fromUtf8(v->family()));
	contact.setFirstName(QString::fromUtf8(v->given()));
	//contact.setAdditionalName(QString::fromUtf8(v->middle()));
	//contact.setPrefix(QString::fromUtf8(v->prefix()));
	//contact.setSuffix(QString::fromUtf8(v->suffix()));
	QString sex = QString::fromUtf8(v->prefix());
	if (sex == "Mr.") {
		contact.setSex(Contact::Male);
	} else if (sex == "Mme.") {
		contact.setSex(Contact::Female);
	}
}

StreetAddress VCardReader::readAddressValue(ContentLine * cl) {
	StreetAddress address;
	if (!cl) {
		return address;
	}

	AdrValue * v = (AdrValue *) cl->value();
	if (!v) {
		return address;
	}

	//address.setPostOfficeBox(QString::fromUtf8(v->poBox()));
	//address.setExtended(QString::fromUtf8(v->extAddress()));
	address.setStreet(QString::fromUtf8(v->street()));
	address.setCity(QString::fromUtf8(v->locality()));
	address.setStateProvince(QString::fromUtf8(v->region()));
	address.setZipCode(QString::fromUtf8(v->postCode()));
	address.setCountry(QString::fromUtf8(v->countryName()));
	return address;
}

void VCardReader::readTelephoneValue(ContentLine * cl, Contact & contact) {
	if (!cl) {
		return;
	}

	TelValue * value = (TelValue *) cl->value();
	if (!value) {
		return;
	}

	ParamList params = cl->paramList();
	for (ParamListIterator it(params); it.current(); ++it) {
		if ((*it)->name() == "TYPE") {
			if ((*it)->value() == "home") {
				contact.setHomePhone(QString::fromUtf8(value->asString()));
			}
			else if ((*it)->value() == "work") {
				contact.setWorkPhone(QString::fromUtf8(value->asString()));
			}
			else if ((*it)->value() == "fax") {
				contact.setFax(QString::fromUtf8(value->asString()));
			}
			else if ((*it)->value() == "cell") {
				contact.setMobilePhone(QString::fromUtf8(value->asString()));
			}
			else if ((*it)->value() == "pref") {
				contact.setWengoPhoneWithoutSubscribe(QString::fromUtf8(value->asString()));
			}
			else if ((*it)->value() == "other") {
				contact.setOtherPhone(QString::fromUtf8(value->asString()));
			}
		}
	}
}
