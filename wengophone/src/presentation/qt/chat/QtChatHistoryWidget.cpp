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

#include "QtChatHistoryWidget.h"

#include <util/Logger.h>

#include <QtGui/QtGui>

QtChatHistoryWidget::QtChatHistoryWidget(QWidget * parent)
: QTextBrowser(parent){
	setFrameStyle(QFrame::Shape::NoFrame | QFrame::Shadow::Plain);
	setContextMenuPolicy(Qt::CustomContextMenu);
	makeActions();
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenuSlot(const QPoint &)));
}

void QtChatHistoryWidget::showContextMenuSlot(const QPoint & pos){
	LOG_DEBUG("showContextMenuSlot");
	QMenu menu(this);
	menu.addAction(_saveAsAction);
	menu.exec(viewport()->mapToGlobal(pos));
}

void QtChatHistoryWidget::makeActions(){
	_saveAsAction = new QAction(tr("&Save As"), this);
	_saveAsAction->setStatusTip(tr("Save the document to disk"));
	connect(_saveAsAction, SIGNAL(triggered()), SLOT(saveHistoryAsHtmlSlot()));
}

void QtChatHistoryWidget::saveHistoryAsHtmlSlot(){
	LOG_DEBUG("saveHistoryAsHtmlSlot");
	QString contentToSave = toHtml();
	// FIXME Qt HTML
	QFile fileToSave(QFileDialog::getSaveFileName(this, tr("Save As"), "/", "HTML (*.htm *.html)"));
	// TODO add extension to filename
	// TODO find home directory
	fileToSave.open(QIODevice::WriteOnly);
	fileToSave.write(contentToSave.toStdString().c_str(), (long long)contentToSave.length());
	fileToSave.close();
}
