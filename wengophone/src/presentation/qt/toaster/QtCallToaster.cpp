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

#include "QtCallToaster.h"

#include "ui_QtCallToaster.h"

#include <QtGui>

QtCallToaster::QtCallToaster(QWidget * parent)
	: QObject(parent) {

	_callToasterWidget = new QWidget(parent, Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

	_ui = new Ui::CallToaster();
	_ui->setupUi(_callToasterWidget);

	_closeTimerId = -1;
	_closeTimer = 5000;
	_show = true;

	_callToasterWidget->setAttribute(Qt::WA_DeleteOnClose, true);

	_ui->hangupLabel->setPixmaps(QPixmap(":/pics/toaster/raccrocher.png"),
			QPixmap(),
			QPixmap(),
			QPixmap(":/pics/toaster/raccrocher.png"),
			QPixmap(),
			QPixmap());

	_ui->callLabel->setPixmaps(QPixmap(":/pics/toaster/decrocher.png"),
			QPixmap(),
			QPixmap(),
			QPixmap(":/pics/toaster/decrocher.png"),
			QPixmap(),
			QPixmap());

	_ui->callLabel->setMinimumSize(QSize(48, 56));
	_ui->callLabel->setMaximumSize(QSize(48, 56));

	_ui->hangupLabel->setMinimumSize(QSize(28, 56));
	_ui->hangupLabel->setMaximumSize(QSize(28, 56));

	connect(_ui->hangupLabel, SIGNAL(clicked()), SLOT(hangupButtonSlot()));
	connect(_ui->callLabel, SIGNAL(clicked()), SLOT(callButtonSlot()));

	_callToasterWidget->resize(1, 1);

	QRect r = _ui->frame_3->rect();

	QLinearGradient lg(QPointF(1, r.top()), QPointF(1, r.bottom()));
	lg.setColorAt(0, qApp->palette().color(QPalette::Window));

	QColor dest = qApp->palette().color(QPalette::Window);
	float red = ((float) dest.red()) / 1.3f;
	float blue = ((float) dest.blue()) / 1.3f;
	float green = ((float) dest.green()) / 1.3f;
	dest = QColor((int) red, (int) green, (int) blue);
	lg.setColorAt(1, dest);

	QPalette palette = _ui->frame_3->palette();
	palette.setBrush(_ui->frame_3->backgroundRole(), QBrush(lg));
	_ui->frame_3->setPalette(palette);
}

void QtCallToaster::setTitle(const QString & title) {
	_ui->titleLabel->setText(title);
}

void QtCallToaster::setMessage(const QString & message) {
	_ui->messageLabel->setText(message);
}

void QtCallToaster::close() {
	killTimer(_timerId);
	killTimer(_closeTimerId);
	_callToasterWidget->close();
}

void QtCallToaster::show() {
	QDesktopWidget * desktop = QApplication::desktop();

	QRect screenGeometry = desktop->screenGeometry(desktop->primaryScreen());

	_startPosition.setY(screenGeometry.bottom());
	_startPosition.setX(screenGeometry.right() - _callToasterWidget->size().width());

	_callToasterWidget->move(_startPosition);

	_callToasterWidget->show();

	_startPosition.setY(screenGeometry.bottom());
	_startPosition.setX(screenGeometry.right() - _callToasterWidget->size().width());
	_callToasterWidget->move(_startPosition);

	_timerId = startTimer(20);
}

void QtCallToaster::setCloseTimer(int timer) {
	_closeTimer = timer;
}

void QtCallToaster::setPixmap(const QPixmap & pixmap) {
	_ui->pixmapLabel->setPixmap(pixmap);
}

void QtCallToaster::timerEvent(QTimerEvent * event) {

	QDesktopWidget * desktop = QApplication::desktop();
	QRect desktopGeometry = desktop->availableGeometry(desktop->primaryScreen());
	QRect screenGeometry = desktop->screenGeometry(desktop->primaryScreen());

	if (event->timerId() == _timerId) {
		if (_show) {
			QPoint p = _callToasterWidget->pos();

			_callToasterWidget->move(p.x(), p.y() - 3);

			if (p.y() < (desktopGeometry.bottom() - _callToasterWidget->size().height() - 5)) {
				killTimer(_timerId);
				_closeTimerId = startTimer(_closeTimer);
			}
		}
		else {
			QPoint p = _callToasterWidget->pos();

			_callToasterWidget->move(p.x(), p.y() + 3);

			if (p.y() > (screenGeometry.bottom())) {
				close();
			}
		}
	}

	if (event->timerId() == _closeTimerId) {
		_show = false;
		_timerId = startTimer(20);
	}
}

void QtCallToaster::hangupButtonSlot() {
	hangupButtonClicked();
	_callToasterWidget->close();
}

void QtCallToaster::callButtonSlot() {
	callButtonClicked();
	_callToasterWidget->close();
}
