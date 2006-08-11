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

#include "QtToaster.h"

#include <QtGui/QtGui>

QtToaster::QtToaster(QWidget * toaster, QFrame * toasterWindowFrame)
	: QObject(toaster) {

	_timer = NULL;
	_show = true;

	_toaster = toaster;
	_toaster->setParent(_toaster->parentWidget(), Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	_toaster->setAttribute(Qt::WA_DeleteOnClose, true);

	_toaster->resize(190, 150);

	QRect r = toasterWindowFrame->rect();

	QLinearGradient lg(QPointF(1, r.top()), QPointF(1, r.bottom()));
	lg.setColorAt(0, qApp->palette().color(QPalette::Window));

	QColor dest = qApp->palette().color(QPalette::Window);
	float red = ((float) dest.red()) / 1.3f;
	float blue = ((float) dest.blue()) / 1.3f;
	float green = ((float) dest.green()) / 1.3f;
	dest = QColor((int) red, (int) green, (int) blue);
	lg.setColorAt(1, dest);

	QPalette palette = toasterWindowFrame->palette();
	palette.setBrush(toasterWindowFrame->backgroundRole(), QBrush(lg));
	toasterWindowFrame->setPalette(palette);
}

void QtToaster::close() {
	_timer->stop();
	_toaster->close();
}

void QtToaster::show() {
	QDesktopWidget * desktop = QApplication::desktop();
	QRect screenGeometry = desktop->screenGeometry(desktop->primaryScreen());

	_toaster->move(screenGeometry.bottom(), screenGeometry.right() - _toaster->size().width());
	//_toaster->move(1280 + 50, 1024 + 50);

	_toaster->show();

	_timer = new QTimer(this);
	connect(_timer, SIGNAL(timeout()), this, SLOT(changeToasterPosition()));
	_timer->start(20);
}

void QtToaster::changeToasterPosition() {
	QDesktopWidget * desktop = QApplication::desktop();
	QPoint p = _toaster->pos();

	//Toaster is showing slowly
	if (_show) {
		_toaster->move(p.x(), p.y() - 3);

		QRect desktopGeometry = desktop->availableGeometry(desktop->primaryScreen());

		if (p.y() < (desktopGeometry.bottom() - _toaster->size().height() - 5)) {
			//Toaster should be hidden now
			_show = false;
			_timer->stop();
			//Waits 5 seconds with the toaster on top
			_timer->start(5000);
		}
	}

	//Toaster is hiding slowly
	else {
		_toaster->move(p.x(), p.y() + 3);

		QRect screenGeometry = desktop->screenGeometry(desktop->primaryScreen());

		_timer->stop();
		_timer->start(20);

		if (p.y() > (screenGeometry.bottom())) {
			//Closes the toaster -> hide it completely
			close();
		}
	}
}
