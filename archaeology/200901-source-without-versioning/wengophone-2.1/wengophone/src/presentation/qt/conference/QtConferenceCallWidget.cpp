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

#include "QtConferenceCallWidget.h"

#include "ui_ConferenceCallWidget.h"

#include <presentation/qt/contactlist/QtContactMenu.h>

#include <model/phoneline/IPhoneLine.h>
#include <model/phonecall/ConferenceCall.h>

#include <util/SafeDelete.h>
#include <util/Logger.h>
#include <qtutil/WidgetBackgroundImage.h>
#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

QtConferenceCallWidget::QtConferenceCallWidget(QWidget * parent, CWengoPhone & cWengoPhone, IPhoneLine * phoneLine)
	: QDialog(parent), _phoneLine(phoneLine), _cWengoPhone(cWengoPhone)  {

	_currentPeer = 1;

	_ui = new Ui::ConferenceCallWidget();
	_ui->setupUi(this);

	_menu = new QMenu(this);
	_wengoMenu = new QMenu(_menu);
	_wengoMenu->setTitle(tr("Call Wengo"));
	_sipMenu = new QMenu(_menu);
	_sipMenu->setTitle(tr("Call SIP"));
	_landLineMenu = new QMenu(_menu);
	_landLineMenu->setTitle(tr("Call land line"));
	_mobileMenu = new QMenu(_menu);
	_mobileMenu->setTitle(tr("Call Mobile"));
	_menu->addMenu(_wengoMenu);
	_menu->addMenu(_landLineMenu);
	_menu->addMenu(_mobileMenu);
	_menu->addMenu(_sipMenu);

	WidgetBackgroundImage::setBackgroundImage(_ui->headerLabel, ":pics/headers/conference.png", WidgetBackgroundImage::AdjustSize);

	SAFE_CONNECT(_ui->startButton, SIGNAL(clicked()), SLOT(startClicked()));
	SAFE_CONNECT(_ui->cancelButton, SIGNAL(clicked()), SLOT(reject()));
	SAFE_CONNECT(_ui->peer1ToolButton, SIGNAL(clicked()), SLOT(peer1ToolButtonClicked()));
	SAFE_CONNECT(_ui->peer2ToolButton, SIGNAL(clicked()), SLOT(peer2ToolButtonClicked()));

	SAFE_CONNECT(_wengoMenu, SIGNAL(triggered(QAction *)), SLOT(updateLineEdit(QAction *)));
	SAFE_CONNECT(_landLineMenu, SIGNAL(triggered(QAction *)), SLOT(updateLineEdit(QAction *)));
	SAFE_CONNECT(_mobileMenu, SIGNAL(triggered(QAction *)), SLOT(updateLineEdit(QAction *)));
	SAFE_CONNECT(_sipMenu, SIGNAL(triggered(QAction *)), SLOT(updateLineEdit(QAction *)));
}

QtConferenceCallWidget::~QtConferenceCallWidget() {
	OWSAFE_DELETE(_ui);
}

void QtConferenceCallWidget::startClicked() {

	if ((!_ui->phoneNumber1LineEdit->text().isEmpty()) && (!_ui->phoneNumber2LineEdit->text().isEmpty())) {

		ConferenceCall * confCall = new ConferenceCall(*_phoneLine);
		confCall->addPhoneNumber(_ui->phoneNumber1LineEdit->text().toStdString());
		confCall->addPhoneNumber(_ui->phoneNumber2LineEdit->text().toStdString());
		accept();

	} else {
		if (_ui->phoneNumber1LineEdit->text().isEmpty()) {
			_ui->phoneNumber1LineEdit->setFocus();
		} else {
			_ui->phoneNumber2LineEdit->setFocus();
		}
	}
}

void QtConferenceCallWidget::peer1ToolButtonClicked() {
	_currentPeer = 1;
	showMenu(_ui->peer1ToolButton->pos());
}

void QtConferenceCallWidget::peer2ToolButtonClicked() {
	_currentPeer = 2;
	showMenu(_ui->peer2ToolButton->pos());
}

void QtConferenceCallWidget::showMenu(QPoint point) {
	point.setX(point.x() + _ui->peer2ToolButton->rect().width());
	populateMenus();
	_menu->popup(mapToGlobal(point));
}

void QtConferenceCallWidget::populateMenus() {
	_sipMenu->clear();
	_wengoMenu->clear();
	_landLineMenu->clear();
	_mobileMenu->clear();

	QtContactMenu::populateMobilePhoneMenu(_mobileMenu, _cWengoPhone);
	QtContactMenu::populateWengoCallMenu(_wengoMenu, _cWengoPhone);
	QtContactMenu::populateHomePhoneMenu(_landLineMenu, _cWengoPhone);
	QtContactMenu::populateSipCallMenu(_sipMenu, _cWengoPhone);
}

void QtConferenceCallWidget::updateLineEdit(QAction * action) {
	if (action) {
		QString data = action->data().toString();
		switch (_currentPeer) {
		case 1:
			_ui->phoneNumber1LineEdit->setText(data);
			break;
		case 2:
			_ui->phoneNumber2LineEdit->setText(data);
			break;
		default:
			LOG_FATAL("Unknown _currentPeer");
		}
	}
}
