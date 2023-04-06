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

#include "MemoryDumpWindow.h"

#include <qapplication.h>
#include <qtranslator.h>
#include <qdialog.h>

#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	if (argc < 3) {
		cout << "Usage: " << argv[0]
			<< " memory dump path" << "applicationName" << endl;
		exit(EXIT_FAILURE);
	}
	
	//Translation file
	if (argv[3] != NULL) {
		QString lang = argv[3];
		QTranslator * translator = new QTranslator(0);
		translator->load(lang);
		app.installTranslator(translator);
	}
	
	
	QString memoryDumpPath = argv[1];
	QString applicationName = argv[2];
	QString userData;
	
	if (argc > 5 && !strcmp(argv[4], "-user")) {
		userData = argv[5];
	}
	
	MemoryDumpWindow * memoryDumpWindow = new MemoryDumpWindow(0, applicationName, memoryDumpPath, userData);
	app.setMainWidget(memoryDumpWindow->getWidget());
	memoryDumpWindow->getWidget()->show();

	return app.exec();
}
