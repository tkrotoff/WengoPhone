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

#include "QtChatEditWidget.h"

#include <util/Logger.h>

#include <QtGui/QtGUi>

#include <stdio.h>

QtChatEditWidget::QtChatEditWidget(QWidget *parent)
: QTextEdit(parent){
	setAcceptDrops(true);
	setFrameStyle(QFrame::Shape::NoFrame | QFrame::Shadow::Plain);
}

void QtChatEditWidget::dragEnterEvent(QDragEnterEvent *event){
	LOG_DEBUG("dragEnterEvent");
	if(event->mimeData()->hasFormat("text/uri-list")){
		event->acceptProposedAction();
	}
}

void QtChatEditWidget::dragMoveEvent(QDragMoveEvent *event){
	LOG_DEBUG("dragMoveEvent");
	event->acceptProposedAction();
}

void QtChatEditWidget::dragLeaveEvent(QDragLeaveEvent * event){
	LOG_DEBUG("dragLeaveEvent");
	event->accept();
}

void QtChatEditWidget::dropEvent(QDropEvent *event){
	LOG_DEBUG("dropEvent\n");
    const QMimeData *mimeData = event->mimeData();
	if(mimeData->hasUrls()){
		QList<QUrl> urls = mimeData->urls();
		for(QList<QUrl>::iterator i(urls.begin()); i != urls.end(); i++){
			QString fileName;
			qint64 fileSize;
			QString fileType;
			LOG_DEBUG((*i).toLocalFile().toStdString());
			QFile fileToSend((*i).toLocalFile());
			QFileInfo fileInfo(fileToSend);
			if(fileInfo.exists() && fileInfo.isReadable()){
				fileName = fileInfo.fileName();
				LOG_DEBUG(fileName.toStdString());
				fileSize = fileInfo.size();
				LOG_DEBUG(String::fromLongLong((long long)fileSize));
				fileType = fileInfo.completeSuffix();
				LOG_DEBUG(fileType.toStdString());
				// TODO add the contact name (wengo_id)
				// TODO make a call to send it all to a file transfer plugin (via a controller)
			}
		}
	}
    event->acceptProposedAction();
}

