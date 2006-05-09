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

#include "QtPhoneCall.h"

#include "QtVideo.h"
#include "QtPhoneCallEventFilter.h"

#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/statusbar/QtStatusBar.h>

#include <model/phonecall/PhoneCall.h>
#include <model/phoneline/PhoneLine.h>


#include <control/phonecall/CPhoneCall.h>
#include <control/CWengoPhone.h>

#include <sipwrapper/WebcamVideoFrame.h>
#include <sipwrapper/CodecList.h>

#include <util/Logger.h>

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>

#include <pixertool/pixertool.h>

#include <QtGui>
#include <QSvgRenderer>

QtPhoneCall::QtPhoneCall(CPhoneCall & cPhoneCall)
	: QObjectThreadSafe(NULL),
	_cPhoneCall(cPhoneCall) {

	_qtWengoPhone = (QtWengoPhone *) _cPhoneCall.getCWengoPhone().getPresentation();
	_videoWindow = NULL;

	_hold = true;

	_encrustLocalWebcam = true;
	_showVideo = false;

	stateChangedEvent += boost::bind(&QtPhoneCall::stateChangedEventHandler, this, _1);
	videoFrameReceivedEvent += boost::bind(&QtPhoneCall::videoFrameReceivedEventHandler, this, _1, _2);

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneCall::initThreadSafe, this));
	postEvent(event);
}

void QtPhoneCall::initThreadSafe() {
	_phoneCallWidget = WidgetFactory::create(":/forms/phonecall/QtCallContactWidget.ui", _qtWengoPhone->getWidget());
	_phoneCallWidget->setAutoFillBackground(true);

	QString sipAddress = QString::fromStdString(_cPhoneCall.getPeerSipAddress());
	QString callAddress = QString::fromStdString(_cPhoneCall.getPeerDisplayName());
	if (callAddress.isEmpty()) {
		callAddress = QString::fromStdString(_cPhoneCall.getPeerUserName());
	}

	//phoneNumberLabel
	_nickNameLabel = Object::findChild<QLabel *>(_phoneCallWidget, "nickNameLabel");
	QString tmp = QString("<html><head><meta name='qrichtext' content='1'/></head><body "
	                      "style=white-space: pre-wrap; font-family:MS Shell Dlg; font-size:8.25pt;"
                          " font-weight:400; font-style:normal; text-decoration:none;'><p style=' margin-top:0px; "
                          " margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; "
                          "font-size:8pt;'><span style=' font-size:13pt; font-weight:600;'>%1</span></p></body></html>").arg(callAddress);

	_nickNameLabel->setText(tmp);



	/*

	*/
	_nickNameLabel->setToolTip(sipAddress);

	_statusLabel = Object::findChild<QLabel *>(_phoneCallWidget, "statusLabel");
	_statusLabel->setText("");
	_statusLabel->setToolTip(tr("Status"));

	_durationLabel = Object::findChild<QLabel *>(_phoneCallWidget, "durationLabel");
	_durationLabel->setText("00:00:00");
	_durationLabel->setToolTip(tr("Status"));

	_avatarLabel = Object::findChild<QLabel *>(_phoneCallWidget, "avatarLabel");

	QtPhoneCallEventFilter * filter = new QtPhoneCallEventFilter(_phoneCallWidget);
	_phoneCallWidget->installEventFilter(filter);

    // Accept call
	_actionAcceptCall = new QAction(tr("Accept"),this);
    connect(_actionAcceptCall, SIGNAL(triggered(bool)), SLOT(acceptActionTriggered(bool)));

    // Hand-up call
	_actionHangupCall = new QAction(tr("Hang-up"),this);
	connect(_actionHangupCall, SIGNAL(triggered(bool)), SLOT(rejectActionTriggered(bool)));

    // Hold
	_actionHold = new QAction(tr("Hold"),this);
    connect(_actionHold, SIGNAL(triggered(bool)), SLOT(holdResumeActionTriggered(bool)));

	//Resume
    _actionResume = new QAction(tr("Resume"),this);
    connect(_actionResume , SIGNAL(triggered(bool)), SLOT(holdResumeActionTriggered(bool)));
    _actionResume->setEnabled(false);

	//Invite to conference
	_actionInvite = new QAction(tr("Invite to conference"),this);
    connect(_actionInvite, SIGNAL(triggered(bool)), SLOT(inviteToConference(bool)));
    // _actionInvite->setEnabled(false);

    // Start / Stop video
	_actionSwitchVideo = new QAction(tr("Stop video"),this);
    connect(_actionSwitchVideo, SIGNAL(triggered(bool)), SLOT(switchVideo(bool)));

    // Add contact
	_actionAddContact = new QAction(tr("Add contact"),this);
    connect(_actionAddContact, SIGNAL(triggered(bool)), SLOT(addContactActionTriggered(bool)));

    _popup = createMenu();

	connect(filter, SIGNAL(openPopup(int, int)), SLOT(openPopup(int, int)));

    if ( ! _cPhoneCall.getPhoneCall().getConferenceCall() ){
        _qtWengoPhone->addPhoneCall(this);
    }
    else
    {
        _qtWengoPhone->addToConference(this);
    }
}

QMenu * QtPhoneCall::createMenu(){


	QMenu * menu = new QMenu(_phoneCallWidget);

    // Accept .... resume

    menu->addAction(_actionAcceptCall);

	menu->addAction(_actionHangupCall);

	menu->addAction(_actionHold);

	menu->addAction(_actionResume);

	//Separator
	menu->addSeparator();

	menu->addAction(_actionInvite);

	//Separator
	menu->addSeparator();

    menu->addAction(_actionSwitchVideo);

	//Separator
	menu->addSeparator();

    menu->addAction(_actionAddContact);

	menu->addAction(tr("Block contact"));

	return menu;

}

QMenu * QtPhoneCall::createInviteMenu(){

    PhoneLine & phoneLine = dynamic_cast<PhoneLine &> ( _cPhoneCall.getPhoneCall().getPhoneLine());

    QMenu * menu = new QMenu(tr("Invite to conference"));

    PhoneLine::PhoneCallList phoneCallList = phoneLine.getPhoneCallList();

    PhoneLine::PhoneCallList::iterator it;

    QString me = QString::fromStdString( _cPhoneCall.getPhoneCall().getPeerSipAddress().getUserName() );

    for ( it = phoneCallList.begin(); it != phoneCallList.end(); it++){

        if ( (*it)->getState() != EnumPhoneCallState::PhoneCallStateClosed ){

            QString str = QString::fromStdString( (*it)->getPeerSipAddress().getUserName()) ;
            if ( str != me ){
                QAction * action = menu->addAction(str);
                connect(action,SIGNAL(triggered(bool)),SLOT(inviteToConference(bool)));
                menu->addAction(action);
            }
        }
    }
    return menu;
}

void QtPhoneCall::updatePresentation() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneCall::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtPhoneCall::updatePresentationThreadSafe() {
}

void QtPhoneCall::stateChangedEventHandler(EnumPhoneCallState::PhoneCallState state) {
	typedef PostEvent1<void (EnumPhoneCallState::PhoneCallState), EnumPhoneCallState::PhoneCallState> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneCall::stateChangedEventHandlerThreadSafe, this, _1), state);
	postEvent(event);
}

void QtPhoneCall::stateChangedEventHandlerThreadSafe(EnumPhoneCallState::PhoneCallState state) {
	_qtWengoPhone->getStatusBar().showMessage(QString::fromStdString(
					CodecList::toString(_cPhoneCall.getAudioCodecUsed()) + "/" +
					CodecList::toString(_cPhoneCall.getVideoCodecUsed())));

	switch(state) {

	case EnumPhoneCallState::PhoneCallStateDefault:
		break;

	case EnumPhoneCallState::PhoneCallStateError:
		_statusLabel->setText(tr("Error"));
		break;

	case EnumPhoneCallState::PhoneCallStateResumed:
		_actionResume->setEnabled(false);
		_actionHold->setEnabled(true);
		_statusLabel->setText(tr("Talking"));
		_hold = false;
		break;

	case EnumPhoneCallState::PhoneCallStateTalking:
		_duration = 0;
		_timerId = startTimer(1000);
		if (_timerId == 0) {
			LOG_DEBUG("_timerId == 0");
		}
		_actionAcceptCall->setEnabled(false);
		_actionHangupCall->setEnabled(true);
		_statusLabel->setText(tr("Talking"));
		break;

	case EnumPhoneCallState::PhoneCallStateDialing:
		_actionAcceptCall->setEnabled(false);
		_actionHangupCall->setEnabled(true);
		_statusLabel->setText(tr("Dialing"));
		break;

	case EnumPhoneCallState::PhoneCallStateRinging:
		_actionAcceptCall->setEnabled(false);
		_actionHangupCall->setEnabled(true);
		_statusLabel->setText(tr("Ringing"));
		break;

	case EnumPhoneCallState::PhoneCallStateClosed:
		_qtWengoPhone->getStatusBar().showMessage(QString::null);
		killTimer(_timerId);
		_actionAcceptCall->setEnabled(false);
		_actionHangupCall->setEnabled(false);
		_statusLabel->setText(tr("Closed"));
		// stopConference();
		delete _phoneCallWidget;
		deleteMe(this);
		callRejected();
		break;

	case EnumPhoneCallState::PhoneCallStateIncoming:
		_actionAcceptCall->setEnabled(true);
		_actionHangupCall->setEnabled(true);
		_statusLabel->setText(tr("Incoming Call"));
		break;

	case EnumPhoneCallState::PhoneCallStateHold:
		_statusLabel->setText(tr("Hold"));
		_actionHold->setEnabled(false);
		_actionResume->setEnabled(true);
		_hold = false;
		break;

	case EnumPhoneCallState::PhoneCallStateMissed:
		_statusLabel->setText(tr("Missed"));
		break;

	case EnumPhoneCallState::PhoneCallStateRedirected:
		_statusLabel->setText(tr("Redirected"));
		break;

	default:
		LOG_FATAL("unknown PhoneCallState=" + EnumPhoneCallState::toString(state));
	}
}

void QtPhoneCall::videoFrameReceivedEventHandler(const WebcamVideoFrame & remoteVideoFrame, const WebcamVideoFrame & localVideoFrame) {
    // the best image quality is obtained when the interim image size is set according to the screen target size
    QSize size(640, 480); // will be the optimum interim resized image
   	if (_videoWindow) {
        QSize frameSize = _videoWindow->getFrameSize(); // screen target size
        if (frameSize.width() < size.width()) {
            size.setWidth(352);
            size.setHeight(288);
        }
    }

	//Image will be deleted in videoFrameReceivedThreadSafe. Here we resize the remote image to the interim size
	QImage *original = new QImage(remoteVideoFrame.getFrame(), remoteVideoFrame.getWidth(),
			remoteVideoFrame.getHeight(), QImage::Format_RGB32);

    QImage *image = new QImage(original->scaled(size.width(), size.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    delete original;

	//If we want to embed the local webcam picture, we do it here
	if (_encrustLocalWebcam) {
		const unsigned offset_x = 10;
		const unsigned offset_y = 10;
		const unsigned ratio = 5;
		const unsigned border_size = 1;
		const QBrush border_color = Qt::black;

        // we force the ratio of the remote frame on the webcam frame (ignoring the webcam's aspect ratio)
		unsigned width = size.width() / ratio;
		unsigned height = size.height() / ratio;
		unsigned posx = size.width() - width - offset_x;
		unsigned posy = size.height() - height - offset_y;

/*
		piximage originalImage;
		originalImage.palette = PIX_OSI_RGB32;
		originalImage.width = localVideoFrame.getWidth();
		originalImage.height = localVideoFrame.getHeight();
		originalImage.data = localVideoFrame.getFrame();

		piximage * resizedImage = pix_alloc(PIX_OSI_RGB32, width, height);

		pix_convert(PIX_NO_FLAG, resizedImage, &originalImage);

		QImage resizedQImage = QImage(resizedImage->data, resizedImage->width,
			resizedImage->height, QImage::Format_RGB32);

		QPainter painter(image);
		painter.drawImage(posx, posy, resizedQImage);

		pix_free(resizedImage);
*/

        // prepare the embedded image
		QImage localImage = QImage(localVideoFrame.getFrame(), localVideoFrame.getWidth(),
				localVideoFrame.getHeight(),
				QImage::Format_RGB32).scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

		QPainter painter;
        painter.begin(image); 
        // draw a 1-pixel border around the local embedded frame
        painter.fillRect(posx - border_size, posy - border_size, width + 2*border_size, height + 2*border_size, border_color);
        // embed the image
		painter.drawImage(posx, posy, localImage);
        painter.end();
	}

	typedef PostEvent1<void (QImage *), QImage *> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneCall::videoFrameReceivedEventHandlerThreadSafe, this, _1), image);
	postEvent(event);
}

void QtPhoneCall::videoFrameReceivedEventHandlerThreadSafe(QImage * image) {
	if (!_videoWindow) {
		_showVideo = true;
		_videoWindow = new QtVideo(_phoneCallWidget);
		showVideoWidget();
	}
	_videoWindow->showImage(*image);
	//Image was created in videoFrameReceived
	delete image;
}

void QtPhoneCall::acceptActionTriggered(bool) {
	_cPhoneCall.accept();
}

void QtPhoneCall::rejectActionTriggered(bool) {

    switch ( _cPhoneCall.getState())
    {
        case EnumPhoneCallState::PhoneCallStateResumed:
        case EnumPhoneCallState::PhoneCallStateTalking:
        case EnumPhoneCallState::PhoneCallStateDialing:
        case EnumPhoneCallState::PhoneCallStateRinging:
        case EnumPhoneCallState::PhoneCallStateIncoming:
        case EnumPhoneCallState::PhoneCallStateHold:
        case EnumPhoneCallState::PhoneCallStateRedirected:
            _cPhoneCall.hangUp();
            break;
        default:
            delete _phoneCallWidget;
            callRejected(); // Close the widget
    }
}

void QtPhoneCall::holdResumeActionTriggered(bool) {
	if (_hold) {
		_cPhoneCall.hold();
	} else {
		_cPhoneCall.resume();
	}
}

void QtPhoneCall::addContactActionTriggered(bool) {
	if (_cPhoneCall.getCWengoPhone().getCUserProfile()) {
		//CHECK: this method should not be called if no UserProfile has been
		// set
		std::string callAddress = _cPhoneCall.getPeerDisplayName();

		if(callAddress.empty()) {
			callAddress = _cPhoneCall.getPeerUserName();
		}

		/*QtAddContact * qtAddContact = new QtAddContact(*_cPhoneCall.getCWengoPhone().getCUserProfile(), _phoneCallWidget, callAddress);
		LOG_DEBUG("add contact=" + callAddress);*/
	}
}

void QtPhoneCall::transferButtonClicked() {
	static QLineEdit * transferPhoneNumberLineEdit = Object::findChild<QLineEdit *>(_phoneCallWidget, "transferPhoneNumberLineEdit");

	_cPhoneCall.blindTransfer(transferPhoneNumberLineEdit->text().toStdString());
}

void QtPhoneCall::openPopup(int x , int y) {
    QMenu * m = createInviteMenu();
    _actionInvite->setMenu(m);
	_popup->exec(QPoint(x, y));
	_actionInvite->setMenu(NULL);
	_actionInvite->setEnabled(true);
	delete m;

}

void QtPhoneCall::timerEvent(QTimerEvent * event) {
	_duration++;
	QTime time;
	time = time.addSecs(_duration);
	_durationLabel->setText(time.toString(Qt::TextDate));
}

void QtPhoneCall::showVideoWidget() {
	QGridLayout * layout = qobject_cast<QGridLayout *>(_phoneCallWidget->layout());

	//Remove the avatar from the widget
	layout->removeWidget(_avatarLabel);
	_avatarLabel->hide();

	//Insert the video widget
	_videoWindow->getWidget()->setParent(_phoneCallWidget);
	_videoWindow->getWidget()->setMaximumSize(QSize(210, 160));
	_videoWindow->getWidget()->setMinimumSize(QSize(210, 160));

	layout->addWidget(_videoWindow->getWidget(), 0 , 0);

	_videoWindow->getWidget()->show();
}

void QtPhoneCall::showAvatar() {
	QGridLayout * layout = dynamic_cast<QGridLayout *>(_phoneCallWidget->layout());

	//Remove the video widget
	layout->removeWidget(_videoWindow->getWidget());
	_videoWindow->getWidget()->hide();

	//Insert the avatar label
	layout->addWidget(_avatarLabel, 0 , 0);
	_avatarLabel->show();
}

void QtPhoneCall::switchVideo(bool) {
	if (_showVideo) {
		showAvatar();
		_showVideo = false;
		_actionSwitchVideo->setText(tr("Start video"));
	} else {
		_showVideo = true;
		_actionSwitchVideo->setText(tr("Stop video"));
		showVideoWidget();
	}
}

CPhoneCall & QtPhoneCall::getCPhoneCall() {
	return _cPhoneCall;
}

void QtPhoneCall::inviteToConference(bool) {
	// startConference(this);
    QObject * source = sender();

    QAction * action = dynamic_cast<QAction *> (source);

    QString addCall = action->text();

    // Search the call
    PhoneLine & phoneLine = dynamic_cast<PhoneLine &> ( _cPhoneCall.getPhoneCall().getPhoneLine());

    PhoneLine::PhoneCallList phoneCallList = phoneLine.getPhoneCallList();

    PhoneLine::PhoneCallList::iterator it;

    for ( it = phoneCallList.begin(); it != phoneCallList.end(); it++){

        if ( (*it)->getState() != EnumPhoneCallState::PhoneCallStateClosed ){
            PhoneCall & me = _cPhoneCall.getPhoneCall();
            PhoneCall * add = (*it);
            startConference(&me,add);
            QString str = QString::fromStdString( (*it)->getPeerSipAddress().getUserName()) ;
            if ( str == addCall ){

            }
        }
    }

}




















