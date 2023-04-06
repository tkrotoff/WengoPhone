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

#include "MainWindow.h"
#include "config/Config.h"
#include "config/AdvancedConfig.h"
#include "Softphone.h"
#include "login/LoginWindow.h"
#include "login/User.h"
#include "login/Login.h"
#include "connect/Connect.h"

#ifdef WIN32
#include <MemoryDump.h>
#endif

#include <http/HttpRequest.h>
#include <thread/Thread.h>
#include <thread/NullThreadFactory.h>
#include <thread/QtThreadFactory.h>

#include <qapplication.h>
#include <qdialog.h>
#include <qmainwindow.h>
#include <qwidgetfactory.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <qobject.h>

#ifdef WIN32
#include <windows.h>
#include <tlhelp32.h>
#endif

/*
void loadPlugins(const QString & pluginFilename) {
	QLibrary * lib = new QLibrary(QDir::currentDirPath() + QDir::separator() + pluginFilename);
	lib->load();
	lib->setAutoUnload(false);

	typedef Plugin * (*createPlugin)();

	createPlugin create = (createPlugin) lib->resolve("create");
	if (create) {
		cout << "true" << endl;
		Plugin * plugin = create();
		PhonePageWidget & phonePageWidget = Softphone::getInstance().getMainWindow().getSessionWindow().getPhonePageWidget();
		plugin->init(Softphone::getInstance(), phonePageWidget);
		plugin->run();
	} else {
		cout << "false" << endl;
	}
}
*/

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

#include <stdio.h>

void faultHandler(char *buf, int size) {
	bool tunnel = Connect::isHttpTunnelEnabled();
	QString usr_login = Softphone::getInstance().getUser().getLogin();
	snprintf(buf, size, " -user \"tunnel=%s \t login=%s \" ", tunnel ? "yes" : "no", usr_login.ascii());
}

int main(int argc, char * argv[]) {
	Thread::setFactory(new QtThreadFactory());
	QApplication app(argc, argv);

#ifdef WIN32
	MemoryDump * memoryDump = new MemoryDump(QString("WengoPhone-") + Softphone::BUILD_ID_STRING);
#endif

	Softphone & softphone = Softphone::getInstance();
	softphone.setQApplication(app);
	softphone.loadConfig();

#ifdef WIN32
	//check if another instance is already running
	int index=0;
	int instance_counter = 0;
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	PROCESSENTRY32* processInfo =new PROCESSENTRY32;
	processInfo->dwSize = sizeof(PROCESSENTRY32);

	while(Process32Next(hSnapShot,processInfo)!=FALSE) {
		QString s(processInfo->szExeFile);
		if (s == "wengophone.exe") {
			instance_counter++;
		}
	}
	//if another instance is detected, exit
	if (instance_counter == 2) {
		QMessageBox::warning(0, QObject::tr("WengoPhone"), QObject::tr("WengoPhone is already running."), QMessageBox::Ok, 0);
		exit(0);
	}
#endif

#ifdef WIN32
	memoryDump->setStyle(Config::getInstance().getStyle());
	memoryDump->setLanguage(softphone.getLanguagesDir() + Config::getInstance().getLanguage());
	memoryDump->SetUserNotify(faultHandler);
	
#endif
	//configure HttpRequest
	AdvancedConfig & config = AdvancedConfig::getInstance();
	if(!config.getHttpProxyUrl().isEmpty() && !config.getHttpProxyPort().isEmpty()) {
		HttpRequest::setUseProxy(true);
		if(!config.getHttpProxyUserID().isEmpty() && !config.getHttpProxyUserPassword().isEmpty()) {
			HttpRequest::setUseProxyAuthentication(true);
			HttpRequest::setProxy(config.getHttpProxyUrl(), config.getHttpProxyPort().toInt(),
				config.getHttpProxyUserID(), config.getHttpProxyUserPassword());
		} else {
			HttpRequest::setUseProxyAuthentication(false);
			HttpRequest::setProxy(config.getHttpProxyUrl(), config.getHttpProxyPort().toInt(),"", "");
		}
	} else {
		HttpRequest::setUseProxy(false);
	}
	
	//To do at the beginning of the application in order to receive calls
	MainWindow mainWindow;
	if (QString(argv[1]) != "-background") {
		mainWindow.getWidget()->show();
	}

	//Login Window
	//Login after the creation of the MainWindow
	//Login procedure is asynchronous (seems to be faster to the user)
	LoginWindow * loginWindow = new LoginWindow(mainWindow.getWidget());
	const User * user = softphone.loadUser();
	if (user == NULL) {
		loginWindow->getWidget()->show();
	} else if (!user->autoLogin()) {
		loginWindow->getWidget()->show();
	} else {
		new Login(*user);
	}

	//changeProfileAction
	QAction * changeProfileAction = (QAction *) mainWindow.getWidget()->child("changeProfileAction", "QAction");
	QObject::connect(changeProfileAction, SIGNAL(activated()), loginWindow->getWidget(), SLOT(show()));
	softphone.loadPlugins();

	return app.exec();
}
