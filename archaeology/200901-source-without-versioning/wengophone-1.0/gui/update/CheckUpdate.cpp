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

#include "CheckUpdate.h"

#include "database/Database.h"
#include "database/HttpSource.h"
using namespace database;
#include "UpdateBuildId.h"
#include "Softphone.h"
#include "StatusBar.h"
#include "Update.h"
#include "config/AdvancedConfig.h"
#include "softphone-config.h"

#include <timer/Timer.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qapplication.h>
#include <qevent.h>
#include <qmessagebox.h>

#include <iostream>
using namespace std;

const int CheckUpdate::UPDATE_TIMEOUT = 24 * 60 * 60 * 1000;
const int CheckUpdate::UPDATERECEIVED_TIMEOUT = 15000;
const int CheckUpdate::LIMIT_NB_RETRY = 5;
Q_ULLONG CheckUpdate::_lastBuildIdAvailable = 0;

/**
 * Custom event for check update.
 *
 * @author Mathieu Stute
 */
class CheckUpdateEvent : public QCustomEvent {
	public:
		enum Type { CheckUpdate = (QEvent::User + 203) };
		CheckUpdateEvent(Type t, UpdateBuildId updateBuildId) : QCustomEvent(t) , _updateBuildId(updateBuildId) {
		}
		~CheckUpdateEvent() {}

		UpdateBuildId _updateBuildId;
};


/**
 * Checks for WengoPhone updates
 *
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class CheckUpdateTask : public TimerTask {
public:
	CheckUpdateTask(HttpRequest * source, bool sslProtocol, const std::string & hostname, unsigned int hostPort,
			const std::string & path, const std::string & data, bool postMethod)
	{
		_source = source;
		_sslProtocol = sslProtocol;
		_hostname = hostname;
		_hostPort = hostPort;
		_path = path;
		_data = data;
		_postMethod = postMethod;
	}

	/**
	 * Sends the login/password to the server.
	 */
	virtual void run() {
		cerr << "CheckUpdateTask: check update" << endl;

		//Sends the HTTP request
		_source->sendRequest(_sslProtocol, _hostname, _hostPort, _path, _data, _postMethod);
	}

private:

	HttpRequest * _source;
	bool _sslProtocol;
	std::string _hostname;
	unsigned int _hostPort;
	std::string _path;
	std::string _data;
	bool _postMethod;
};

UpdateDialog::UpdateDialog(QWidget * parent, Update * updateInstance) : QDialog(parent) {
	_updateInstance = updateInstance;

	setCaption(tr("Update available"));

	QGridLayout * gridLayout = new QGridLayout(this, 1, 1, 11, 6);

	QHBoxLayout * hboxLayout = new QHBoxLayout(0, 0, 6);
	QSpacerItem * horizontalSpacing1 = new QSpacerItem(50, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	hboxLayout->addItem(horizontalSpacing1);

	QPushButton * updatebutton = new QPushButton(tr("&Update"), this);
	updatebutton->setAutoDefault(true);
	updatebutton->setDefault(true);
	hboxLayout->addWidget(updatebutton);
	connect(updatebutton, SIGNAL(clicked()),
		this, SLOT(update()));

	QPushButton * cancelButton = new QPushButton(tr("&Cancel"), this);
	cancelButton->setAutoDefault(true);
	hboxLayout->addWidget(cancelButton);
	QSpacerItem * horizontalSpacing2 = new QSpacerItem( 50, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	hboxLayout->addItem(horizontalSpacing2);
	connect(cancelButton, SIGNAL(clicked()),
		this, SLOT(cancel()));

	gridLayout->addLayout(hboxLayout, 1, 0);

	QLabel * textLabel = new QLabel(this);
	textLabel->setText(tr("Wengo needs to be updated, do you want to do it now?"));
	textLabel->setAlignment(int(QLabel::AlignCenter));

	gridLayout->addWidget(textLabel, 0, 0);
	resize(QSize(300, 98).expandedTo(minimumSizeHint()));
	clearWState(WState_Polished);
}

void UpdateDialog::update() {
	_updateInstance->update();
	hide();
}

void UpdateDialog::cancel() {
	_updateInstance->cancel();
	hide();
}


CheckUpdate::CheckUpdate(const Q_ULLONG buildId, const QString & installer_type, const QString & lang) {
	_buildId = buildId;
	_os = installer_type;
	_lang = lang;
	_nbRetry = 0;
	_auto = true;
	//Dynamic since Update launches an external process
	_update = new Update();

	_updateDialog = new UpdateDialog(Softphone::getInstance().getActiveWindow(), _update);

	QString data = "buildid=" + QString::number(_buildId) + "&installer_type=" + _os + "&lang=" + _lang + "&wl=" + QString(WL_TAG);

	//Timer to update the softphone every 24hours
	//without limite of retry
	_timer24 = new Timer(new CheckUpdateTask(this, false, Softphone::WENGO_SERVER_HOSTNAME, 80, Softphone::WENGO_DOWNLOAD_UPDATE_PATH, data, false));
	_timer24->start(UPDATE_TIMEOUT, 0);

	AdvancedConfig & conf = AdvancedConfig::getInstance();
	if( conf.useSSL() ) {
		sendRequest(true, Softphone::WENGO_SERVER_HOSTNAME, 443, Softphone::WENGO_DOWNLOAD_UPDATE_PATH, data, false);
	} else {
		sendRequest(false, Softphone::WENGO_SERVER_HOSTNAME, 80, Softphone::WENGO_DOWNLOAD_UPDATE_PATH, data, false);
	}
}

CheckUpdate::~CheckUpdate() {
	delete _timer;
	delete _timer24;
}

void CheckUpdate::checkUpdate() {
	//_timer->singleShotNow();
	_auto = false;
	QString data = "buildid=" + QString::number(_buildId) + "&installer_type=" + _os + "&lang=" + _lang + "&wl=" + QString(WL_TAG);

	AdvancedConfig & conf = AdvancedConfig::getInstance();
	if( conf.useSSL() ) {
		sendRequest(true, Softphone::WENGO_SERVER_HOSTNAME, 443, Softphone::WENGO_DOWNLOAD_UPDATE_PATH, data, false);
	} else {
		sendRequest(false, Softphone::WENGO_SERVER_HOSTNAME, 80, Softphone::WENGO_DOWNLOAD_UPDATE_PATH, data, false);
	}
}

void CheckUpdate::answerReceived(const std::string & answer, Error error){

	if( error == NoError ) {
		cerr << "CheckUpdate: update build id received" << endl;
		UpdateBuildId updateBuildId = Database::getInstance().transformBuilIdFromXml(answer);
		QApplication::postEvent(this, new CheckUpdateEvent(CheckUpdateEvent::CheckUpdate, updateBuildId));
	}
	else {
		_nbRetry++;
		if( _nbRetry > LIMIT_NB_RETRY ) {
			cerr << "Update : max number of request" << endl;
			return;
		}
		cerr << "Check update Error" << endl;
		QString data = "buildid=" + QString::number(_buildId) + "&installer_type=" + _os + "&lang=" + _lang + "&wl=" + QString(WL_TAG);
		AdvancedConfig & conf = AdvancedConfig::getInstance();
		if( conf.useSSL() ) {
			sendRequest(true, Softphone::WENGO_SERVER_HOSTNAME, 443, Softphone::WENGO_DOWNLOAD_UPDATE_PATH, data, false);
		} else {
			sendRequest(false, Softphone::WENGO_SERVER_HOSTNAME, 80, Softphone::WENGO_DOWNLOAD_UPDATE_PATH, data, false);
		}
	}

}

void CheckUpdate::customEvent(QCustomEvent *ev) {
	CheckUpdateEvent *e = (CheckUpdateEvent *) ev;

	UpdateBuildId updateBuildId = e->_updateBuildId;
	_lastBuildIdAvailable = updateBuildId.getBuildId();

	if (updateBuildId.getBuildId() != 0 && !updateBuildId.getUrl().isEmpty() ) {
		if (Softphone::BUILD_ID >= updateBuildId.getBuildId()) {
			//No update to make
			Softphone::getInstance().getStatusBar().setUpToDateStatus(true);
			if(!_auto) {
				QMessageBox::information(Softphone::getInstance().getActiveWindow(), tr("WengoPhone update"), tr("You have the latest version installed"), QMessageBox::Ok);
			}
		} else {
				//Dynamic since Update launches an external process
			_update->setUpdate(updateBuildId.getUrl(),
							   updateBuildId.isCompulsery(),
							   updateBuildId.getName());
			_updateDialog->show();
		}
	}
}
