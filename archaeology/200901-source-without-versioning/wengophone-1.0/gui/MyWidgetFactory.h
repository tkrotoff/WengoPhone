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

#ifndef MYWIDGETFACTORY_H
#define MYWIDGETFACTORY_H

#include <qstring.h>
#include <qobject.h>
#include <qwidget.h>
#include <qmessagebox.h>

#include <cassert>

#ifdef DYNAMIC_GUI
#include <qwidgetfactory.h>
#else	//!DYNAMIC_UI
#include "config/AdvancedConfigWidgetForm.h"
#include "config/CodecListWidgetForm.h"
#include "config/ConfigWindowForm.h"
#include "config/LanguageWidgetForm.h"
#include "config/StyleWidgetForm.h"
#include "config/AudioWidgetForm.h"
#include "config/VideoWidgetForm.h"
#include "config/SipProfileWidgetForm.h"
#include "config/GeneralConfigWidgetForm.h"
#include "contact/ContactListWidgetForm.h"
#include "contact/ContactWidgetForm.h"
#include "contact/ContactWindowForm.h"
#include "callhistory/CallHistoryWidgetForm.h"
#include "login/LoginWindowForm.h"
#include "sms/SmsPageWidgetForm.h"
#include "systray/SystrayPopupWindowForm.h"
#include "chat/ChatPageWidgetForm.h"
#include "chat/EmoticonsWidgetForm.h"
#include "visio/VisioPageWidgetForm.h"
#include "visio/VideoCodecConfigWidgetForm.h"
#include "AboutWindowForm.h"
#include "DialPadWidgetForm.h"
#include "MainWindowForm.h"
#include "PhonePageWidgetForm.h"
#include "SessionWindowForm.h"

#ifdef WIN32
#include "HomePageWidgetForm.h"
#include "search/SearchPageWidgetForm.h"
#else
#include "HomePageWidgetForm_posix.h"
#endif

#endif	//DYNAMIC_GUI

/**
 * Replacement for QWidgetFactory::create().
 *
 * Creates widgets dynamically (from XML using libqui) or compiles them using the uic program
 *
 * @author Tanguy Krotoff
 */
class MyWidgetFactory {
public:

	/**
	 * Creates a widget given its XML file descriptor.
	 *
	 * @see QWidgetFactory::create()
	 */
	static QWidget * create(const QString & uiFile, QObject * connector = 0,
				QWidget * parent = 0, const char * name = 0) {

		const QString errorMsg = "Wrong ui file: " + uiFile;

#ifdef DYNAMIC_GUI

		static QStringList pathList = getUiPathList();

		QWidget * widget = 0;
		for (unsigned int i = 0; i < pathList.size(); i++) {
			widget = QWidgetFactory::create(pathList[i] + uiFile, connector, parent, name);
			if (widget != 0) {
				break;
			}
		}
		if (widget == 0) {
			QMessageBox::critical(parent, "Critical Error", errorMsg);
			assert(widget && "Wrong ui file");
		}
		return widget;

#else	//!DYNAMIC_UI

		//To suppress the warning from the compiler
		connector = 0;

		if (uiFile == "AboutWindowForm.ui") return new AboutWindowForm(parent, name);
		if (uiFile == "AdvancedConfigWidgetForm.ui") return new AdvancedConfigWidgetForm(parent, name);
		if (uiFile == "CallHistoryWidgetForm.ui") return new CallHistoryWidgetForm(parent, name);
		if (uiFile == "CallHistoryWidgetForm.ui") return new CallHistoryWidgetForm(parent, name);
		if (uiFile == "CodecListWidgetForm.ui") return new CodecListWidgetForm(parent, name);
		if (uiFile == "ConfigWindowForm.ui") return new ConfigWindowForm(parent, name);
		if (uiFile == "ContactListWidgetForm.ui") return new ContactListWidgetForm(parent, name);
		if (uiFile == "ContactWidgetForm.ui") return new ContactWidgetForm(parent, name);
		if (uiFile == "ContactWindowForm.ui") return new ContactWindowForm(parent, name);
		if (uiFile == "DialPadWidgetForm.ui") return new DialPadWidgetForm(parent, name);
		if (uiFile == "HomePageWidgetForm.ui") return new HomePageWidgetForm(parent, name);
		if (uiFile == "HomePageWidgetForm_posix.ui") return new HomePageWidgetForm(parent, name);
		if (uiFile == "LanguageWidgetForm.ui") return new LanguageWidgetForm(parent, name);
		if (uiFile == "LoginWindowForm.ui") return new LoginWindowForm(parent, name);
		if (uiFile == "MainWindowForm.ui") return new MainWindowForm(parent, name);
		if (uiFile == "PhonePageWidgetForm.ui") return new PhonePageWidgetForm(parent, name);
		if (uiFile == "SipProfileWidgetForm.ui") return new SipProfileWidgetForm(parent, name);
		if (uiFile == "SessionWindowForm.ui") return new SessionWindowForm(parent, name);
		if (uiFile == "SmsPageWidgetForm.ui") return new SmsPageWidgetForm(parent, name);
		if (uiFile == "StyleWidgetForm.ui") return new StyleWidgetForm(parent, name);
		if (uiFile == "AudioWidgetForm.ui") return new AudioWidgetForm(parent, name);
		if (uiFile == "VideoWidgetForm.ui") return new VideoWidgetForm(parent, name);
		if (uiFile == "SystrayPopupWindowForm.ui") return new SystrayPopupWindowForm(parent, name);
		if (uiFile == "GeneralConfigWidgetForm.ui") return new GeneralConfigWidgetForm(parent, name);
		if (uiFile == "ChatPageWidgetForm.ui") return new ChatPageWidgetForm(parent, name);
		if (uiFile == "EmoticonsWidgetForm.ui") return new EmoticonsWidgetForm(parent, name);
		if (uiFile == "VisioPageWidgetForm.ui") return new VisioPageWidgetForm(parent, name);
		if (uiFile == "VideoCodecConfigWidgetForm.ui") return new VideoCodecConfigWidgetForm(parent, name);
#ifdef WIN32
		if (uiFile == "SearchPageWidgetForm.ui") return new SearchPageWidgetForm(parent, name);
#endif
		QMessageBox::critical(parent, "Critical Error", errorMsg);
		assert(0 && "Wrong ui file");
		//To suppress the warning from the compiler
		return 0;

#endif	//DYNAMIC_GUI

	}

private:

	MyWidgetFactory();
	MyWidgetFactory(const MyWidgetFactory &);
	MyWidgetFactory & operator=(const MyWidgetFactory &);

	static QStringList getUiPathList() {
		static QStringList pathList;
		pathList += "";
		pathList += "contact/";
		pathList += "callhistory/";
		pathList += "config/";
		pathList += "connect/";
		pathList += "database/";
		pathList += "login/";
		pathList += "sip/";
		pathList += "sms/";
		pathList += "systray/";
		pathList += "update/";
		pathList += "chat/";
		pathList += "visio/";
		pathList += "search/";
		return pathList;
	}
};

#endif	//MYWIDGETFACTORY_H
