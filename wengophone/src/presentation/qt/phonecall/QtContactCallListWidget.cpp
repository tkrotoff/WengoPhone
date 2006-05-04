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

#include "QtContactCallListWidget.h"
#include "QtContactCallList.h"
#include "QtPhoneCall.h"

#include <model/phonecall/ConferenceCall.h>
#include <control/phonecall/CPhoneCall.h>

QtContactCallListWidget::QtContactCallListWidget (CWengoPhone & cWengoPhone,QWidget * parent, Qt::WFlags f) :
QWidget (parent,f), _cWengoPhone(cWengoPhone)
{
	_layout = new QGridLayout(this);
	_conferenceCall = NULL;
	_listWidget = new QtContactCallList(this);

	// Setup the list widget
	_layout->addWidget(_listWidget,0,0);
	setAttribute(Qt::WA_DeleteOnClose,true);

}


void QtContactCallListWidget::addPhoneCall(QtPhoneCall * qtPhoneCall){
	connect (qtPhoneCall,SIGNAL(startConference(QtPhoneCall *)),SLOT(startConference(QtPhoneCall *)));
	connect (qtPhoneCall,SIGNAL(stopConference() ),SLOT(stopConference()));
	connect (qtPhoneCall,SIGNAL(callRejected()), SLOT(callRejected()));
	_listWidget->addPhoneCall(qtPhoneCall);
}

void QtContactCallListWidget::startConference(QtPhoneCall * qtPhoneCall){

	if ( _conferenceCall )
		return;

	IPhoneLine * phoneLine = _cWengoPhone.getWengoPhone().getCurrentUserProfile().getActivePhoneLine();

	if (phoneLine == NULL)
		return;

	if ( _listWidget->getPhoneCallList().size() != 2 ){
		return;
	}
	_conferenceCall = new ConferenceCall(*phoneLine);

	QtPhoneCall * phone;

	phone = _listWidget->getPhoneCallList()[0];
	_conferenceCall->addPhoneCall(phone->getCPhoneCall().getPhoneCall() );

	phone = _listWidget->getPhoneCallList()[1];
	_conferenceCall->addPhoneCall(phone->getCPhoneCall().getPhoneCall() );

}

void QtContactCallListWidget::stopConference(){

	if ( ! _conferenceCall )
		return;

	QtPhoneCall * phone;

	phone = _listWidget->getPhoneCallList()[0];
	_conferenceCall->removePhoneCall(phone->getCPhoneCall().getPhoneCall() );

	phone = _listWidget->getPhoneCallList()[1];
	_conferenceCall->removePhoneCall(phone->getCPhoneCall().getPhoneCall() );

	delete _conferenceCall;
	_conferenceCall = NULL;

}

void QtContactCallListWidget::hangup(){
    QMutexLocker locker(&_mutex);
    QtPhoneCall * phone;

    QtContactCallList::QtPhoneCallList phoneCallList;
    QtContactCallList::QtPhoneCallList::iterator iter;

    phoneCallList = _listWidget->getPhoneCallList();

    for ( iter = phoneCallList.begin(); iter != phoneCallList.end(); iter++){
        phone =  (*iter); // _listWidget->getPhoneCallList()[0];
        phone->rejectActionTriggered(true);
    }

    // phone = _listWidget->getPhoneCallList()[0];
    //phone->rejectActionTriggered(true);
}
void QtContactCallListWidget::callRejected(){
    close();
}
