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

#include <presentation/qt/contactlist/QtAddContact.h>
#include <presentation/qt/QtWengoPhone.h>
#include <control/phonecall/CPhoneCall.h>
#include <control/CWengoPhone.h>

#include <sipwrapper/WebcamVideoFrame.h>

#include <util/Logger.h>

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>

#include <QtGui>
#include <QSvgRenderer>

QtPhoneCall::QtPhoneCall(CPhoneCall & cPhoneCall)
	: QObjectThreadSafe(),
	_cPhoneCall(cPhoneCall) {

	_qtWengoPhone = (QtWengoPhone *) _cPhoneCall.getCWengoPhone().getPresentation();
	_videoWindow = NULL;
	_hold = true;
	_showVideo = false;

	stateChangedEvent += boost::bind(&QtPhoneCall::stateChangedEventHandler, this, _1);
	videoFrameReceivedEvent += boost::bind(&QtPhoneCall::videoFrameReceivedEventHandler, this, _1, _2);

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneCall::initThreadSafe, this));
	postEvent(event);
}

void QtPhoneCall::initThreadSafe() {

	_phoneCallWidget = WidgetFactory::create(":/forms/phonecall/QtCallContactWidget.ui", _qtWengoPhone->getWidget());
	_phoneCallWidget->setAutoFillBackground ( true );

	QtPhoneCallEventFilter * filter = new QtPhoneCallEventFilter(_phoneCallWidget);

	QString sipAddress = QString::fromStdString(_cPhoneCall.getPeerSipAddress());
	QString callAddress = QString::fromStdString(_cPhoneCall.getPeerDisplayName());
	if (callAddress.isEmpty()) {
		callAddress = QString::fromStdString(_cPhoneCall.getPeerUserName());
	}

	//phoneNumberLabel
	_nickNameLabel = Object::findChild<QLabel *>(_phoneCallWidget, "nickNameLabel");
	_nickNameLabel->setText(callAddress);
	_nickNameLabel->setToolTip(sipAddress);

	_statusLabel = Object::findChild<QLabel *>(_phoneCallWidget, "statusLabel");
	_statusLabel->setText("");
	_statusLabel->setToolTip("Status");

	_durationLabel = Object::findChild<QLabel *>(_phoneCallWidget, "durationLabel");
	_durationLabel->setText("00:00:00");
	_durationLabel->setToolTip("Status");

	_avatarLabel = Object::findChild<QLabel *>(_phoneCallWidget, "avatarLabel");


	/*
	QPixmap testPixmap( QSize(140,140) );
	QSvgRenderer svgRenderer(this);
	svgRenderer.load(QString("images/w_dark.svg"));
	QPainter painter(&testPixmap);
	svgRenderer.render(&painter);
	_avatarLabel->setPixmap(testPixmap);
	*/

	QAction * action;
	_popup = new QMenu(_phoneCallWidget);
	// Accept call
	_actionAcceptCall = _popup->addAction( tr ("Accept") );
	connect (_actionAcceptCall,SIGNAL ( triggered(bool) ), SLOT( acceptActionTriggered(bool) ) );

	// Hang-up call
	_actionHangupCall = _popup->addAction( tr ("Hang-up") );
	connect (_actionHangupCall,SIGNAL ( triggered(bool) ), SLOT ( rejectActionTriggered(bool) ) );

	// Hold
	_actionHold = _popup->addAction( tr ("Hold") );
	connect (_actionHold,SIGNAL ( triggered(bool) ), SLOT ( holdResumeActionTriggered(bool) ) );

	// Resume
	_actionResume = _popup->addAction( tr ("Resume") );
	connect (_actionResume ,SIGNAL ( triggered(bool) ), SLOT ( holdResumeActionTriggered(bool) ) );
	_actionResume->setEnabled(false);

	// Separator
	_popup->addSeparator();

	// Invite to conference
	_actionInvite = _popup->addAction( tr ("Invite to conference") );
	_popup->addSeparator();

	// Show / Hide video
	_actionSwitchVideo = _popup->addAction( tr ("Stop video") );
	connect (_actionSwitchVideo ,SIGNAL ( triggered(bool) ), SLOT ( switchVideo(bool) ) );


	_popup->addSeparator();

	// Add the contact
	_actionAddContact = _popup->addAction( tr ("Add contact") );
	connect (_actionAddContact,SIGNAL ( triggered(bool) ), SLOT ( addContactActionTriggered(bool) ) );

	_actionBlockContact = _popup->addAction( tr ("Block contact") );

	_phoneCallWidget->installEventFilter(filter);

	connect (filter, SIGNAL( openPopup( int, int ) ), SLOT(openPopup( int, int ) ) );


/*
	//acceptButton
	_acceptButton = Object::findChild<QPushButton *>(_phoneCallWidget, "acceptButton");
	_acceptButton->setEnabled(true);
	_acceptButton->disconnect();
	connect(_acceptButton, SIGNAL(clicked()), SLOT(acceptButtonClicked()));

	//phoneNumberLabel
	QLabel * phoneNumberLabel = Object::findChild<QLabel *>(_phoneCallWidget, "phoneNumberLabel");
	phoneNumberLabel->setText(callAddress);
	phoneNumberLabel->setToolTip(sipAddress);

	//rejectButton
	_rejectButton = Object::findChild<QPushButton *>(_phoneCallWidget, "rejectButton");
	_rejectButton->setEnabled(true);
	_rejectButton->disconnect();
	connect(_rejectButton, SIGNAL(clicked()), SLOT(rejectButtonClicked()));

	//holdResumeButton
	_holdResumeButton = Object::findChild<QPushButton *>(_phoneCallWidget, "holdResumeButton");
	_holdResumeButton->disconnect();
	connect(_holdResumeButton, SIGNAL(clicked()), SLOT(holdResumeButtonClicked()));

	//addContactButton
	_addContactButton = Object::findChild<QPushButton *>(_phoneCallWidget, "addContactButton");
	_addContactButton->disconnect();
	connect(_addContactButton, SIGNAL(clicked()), SLOT(addContactButtonClicked()));

	//transferButton
	QPushButton * transferButton = Object::findChild<QPushButton *>(_phoneCallWidget, "transferButton");
	transferButton->disconnect();
	connect(transferButton, SIGNAL(clicked()), SLOT(transferButtonClicked()));
*/
	_qtWengoPhone->addPhoneCall(this);
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

	switch(state) {

	case EnumPhoneCallState::PhoneCallStateDefault:
		break;

	case EnumPhoneCallState::PhoneCallStateError:

		_statusLabel->setText( tr ("error") );
		break;

	case EnumPhoneCallState::PhoneCallStateResumed:

		_actionResume->setEnabled( false );
		_actionHold->setEnabled( true );
		_hold = true;
		break;

	case EnumPhoneCallState::PhoneCallStateTalking:

		_duration = 0;
		_timerId = startTimer(1000);
		if ( _timerId == 0)
			LOG_DEBUG("TIMERID == 0 ?!!!");
		_actionAcceptCall->setEnabled( false );
		_actionHangupCall->setEnabled( true );

		_statusLabel->setText( tr ("talking") );


		break;

	case EnumPhoneCallState::PhoneCallStateDialing:

		_actionAcceptCall->setEnabled( false );
		_actionHangupCall->setEnabled( true );

		_statusLabel->setText( tr ("dialing") );
		break;

	case EnumPhoneCallState::PhoneCallStateRinging:

		_actionAcceptCall->setEnabled( false );
		_actionHangupCall->setEnabled( true );

		_statusLabel->setText( tr ("ringing") );

		break;

	case EnumPhoneCallState::PhoneCallStateClosed:

		killTimer(_timerId);
		_actionAcceptCall->setEnabled( false );
		_actionHangupCall->setEnabled( false );
		_statusLabel->setText( tr ("closed") );

		delete _phoneCallWidget;

		// This object will be deleted later by the Qt event manager
		deleteLater();

		break;

	case EnumPhoneCallState::PhoneCallStateIncoming:

		_actionAcceptCall->setEnabled( true );
		_actionHangupCall->setEnabled( true );

		_statusLabel->setText( tr ("incoming call") );

		break;

	case EnumPhoneCallState::PhoneCallStateHold:

		_statusLabel->setText( tr ("hold") );

		_actionHold->setEnabled( false );
		_actionResume->setEnabled( true );
		_hold = false;
		break;

	case EnumPhoneCallState::PhoneCallStateMissed:
		_statusLabel->setText( tr ("missed") );
		break;

	case EnumPhoneCallState::PhoneCallStateRedirected:
		_statusLabel->setText( tr ("redirected") );
		break;

	default:
		LOG_FATAL("unknown PhoneCallState=" + String::fromNumber(state));
	}
}

void QtPhoneCall::videoFrameReceivedEventHandler(const WebcamVideoFrame & remoteVideoFrame, const WebcamVideoFrame & localVideoFrame) {
	//image will be deleted in videoFrameReceivedThreadSafe
	QImage * image = new QImage(remoteVideoFrame.getFrame(), remoteVideoFrame.getWidth(),
			remoteVideoFrame.getHeight(), QImage::Format_RGB32);

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
	//image was created in videoFrameReceived
	delete image;
}

void QtPhoneCall::acceptActionTriggered ( bool  ) {
	_cPhoneCall.pickUp();
}

void QtPhoneCall::rejectActionTriggered ( bool ) {
	_cPhoneCall.hangUp();
}

void QtPhoneCall::holdResumeActionTriggered ( bool ){
	if (_hold) {
		_cPhoneCall.hold();
	} else {
		_cPhoneCall.resume();
	}
}

void QtPhoneCall::addContactActionTriggered ( bool  ){
	std::string callAddress = _cPhoneCall.getPeerDisplayName();
	if (callAddress.empty()) {
		callAddress = _cPhoneCall.getPeerUserName();
	}
	QtAddContact * qtAddContact = new QtAddContact(_cPhoneCall.getCWengoPhone(), _phoneCallWidget, callAddress);
	LOG_DEBUG("add contact=" + callAddress);
}

void QtPhoneCall::transferButtonClicked() {
	static QLineEdit * transferPhoneNumberLineEdit = Object::findChild<QLineEdit *>(_phoneCallWidget, "transferPhoneNumberLineEdit");

	_cPhoneCall.blindTransfer(transferPhoneNumberLineEdit->text().toStdString());
}

void QtPhoneCall::openPopup( int x , int y ){
	_popup->popup(QPoint(x,y));
}

void QtPhoneCall::timerEvent(QTimerEvent *event){

	_duration++;
	QTime time;

	time = time.addSecs(_duration);

	_durationLabel->setText(time.toString(Qt::TextDate));

}

void QtPhoneCall::showVideoWidget(){



	QGridLayout * layout = dynamic_cast<QGridLayout *> ( _phoneCallWidget->layout() );


	// Remove the avatar from the widget
	layout->removeWidget ( _avatarLabel );
	_avatarLabel->hide();

	// insert the video widget
	_videoWindow->getWidget()->setParent(_phoneCallWidget);
	_videoWindow->getWidget()->setMaximumSize( QSize( 210,160 ) );
	_videoWindow->getWidget()->setMinimumSize( QSize( 210,160 ) );

	layout->addWidget( _videoWindow->getWidget(), 0 , 0 );
	_videoWindow->getWidget()->show();

}

void QtPhoneCall::showAvatar(){


	QGridLayout * layout = dynamic_cast<QGridLayout *> ( _phoneCallWidget->layout() );

	// Remove the video widget
	layout->removeWidget ( _videoWindow->getWidget() );
	_videoWindow->getWidget()->hide();

	// insert the avatar label
	layout->addWidget( _avatarLabel, 0 , 0 );
	_avatarLabel->show();


}

void QtPhoneCall::switchVideo(bool ){

	if ( _showVideo ){
		showAvatar();
		_showVideo = false;
		_actionSwitchVideo->setText ( tr("Start video") );
	}
	else
	{
		_showVideo = true;
		_actionSwitchVideo->setText ( tr("Stop video") );
		showVideoWidget();
	}

}