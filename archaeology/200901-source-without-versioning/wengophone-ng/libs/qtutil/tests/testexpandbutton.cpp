/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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
#include <QtGui/QApplication>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

#include <qtutil/ExpandButton.h>

int main(int argc, char ** argv) {
	Q_INIT_RESOURCE(qtutil);
	QApplication app(argc, argv);

	QWidget* widget = new QWidget();

	ExpandButton* button = new ExpandButton(widget);
	button->setText("&An expand button");
	QLabel* label = new QLabel(widget);
	label->setText("Some random content");
	label->hide();

	QVBoxLayout* layout = new QVBoxLayout(widget);
	layout->addWidget(button);
	layout->addWidget(label);

	QObject::connect(button, SIGNAL(toggled(bool)), label, SLOT(setVisible(bool)));

	widget->show();

	return app.exec();
}
