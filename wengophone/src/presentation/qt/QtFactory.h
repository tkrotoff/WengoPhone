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

#ifndef OWQTFACTORY_H
#define OWQTFACTORY_H

#include <presentation/PFactory.h>

#include <QtCore/QObject>

class PWengoPhone;
class CWengoPhone;
class PUserProfile;
class CUserProfile;
class PUserProfileHandler;
class CUserProfileHandler;
class PPhoneLine;
class CPhoneLine;
class PPhoneCall;
class CPhoneCall;
class PContactList;
class CContactList;
class PWenboxPlugin;
class CWenboxPlugin;
class PChatHandler;
class CChatHandler;
class PSms;
class CSms;
class CSubscribe;
class PWsDirectory;
class CWsDirectory;
class PSoftUpdate;
class CSoftUpdate;
class PHistory;
class CHistory;
class PConferenceCall;
class CConferenceCall;
class CDtmfThemeManager;

class QtContactList;
class QtChatHandler;
class QtWenboxPlugin;
class QtSms;
class QtWsDirectory;
class QtSoftUpdate;
class QtHistory;
class CWengoPhone;
class QtWengoPhone;
class QtUserProfileHandler;
class QtDialpad;

class QEvent;
class QApplication;

class QtFactory : public QObject, public PFactory {
	Q_OBJECT
public:

	QtFactory(int & argc, char * argv[]);

	void processEvents();

	int exec();

	void reset();

	PWengoPhone * createPresentationWengoPhone(CWengoPhone & cWengoPhone);

	PUserProfile * createPresentationUserProfile(CUserProfile & cUserProfile);

	PUserProfileHandler * createPresentationUserProfileHandler(CUserProfileHandler & cUserProfileHandler);

	PPhoneLine * createPresentationPhoneLine(CPhoneLine & cPhoneLine);

	PPhoneCall * createPresentationPhoneCall(CPhoneCall & cPhoneCall);

	PContactList * createPresentationContactList(CContactList & cContactList);

	PWenboxPlugin * createPresentationWenboxPlugin(CWenboxPlugin & cWenboxPlugin);

	PChatHandler * createPresentationChatHandler(CChatHandler & cChatHandler);

	PSms * createPresentationSms(CSms & cSms);

	PWsDirectory * createPresentationWsDirectory(CWsDirectory & cWsDirectory);

	PSoftUpdate * createPresentationSoftUpdate(CSoftUpdate & cSoftUpdate);

	PHistory * createPresentationHistory(CHistory & cHistory);

	PConferenceCall * createPresentationConferenceCall(CConferenceCall & cConferenceCall);

	//DtmfThemeManager
	void createPresentationDtmfThemeManager(CDtmfThemeManager & _cDtmfThemeManager);

private:

	void postEventImpl(IThreadEvent * event);

	QApplication * _app;

	QtContactList * _qtContactList;

	QtChatHandler * _qtChatHandler;

	QtWenboxPlugin * _qtWenboxPlugin;

	QtSms * _qtSms;

	QtWsDirectory * _qtWsDirectory;

	QtSoftUpdate * _qtSoftUpdate;

	QtHistory * _qtHistory;

	CWengoPhone * _cWengoPhone;

	QtWengoPhone * _qtWengoPhone;

	QtUserProfileHandler * _qtUserProfileHandler;

	//DtmfThemeManager
	QtDialpad * _qtDialpad;
};

#endif	//OWQTFACTORY_H
