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

#ifndef OWQTFILETRANSFERWIDGET_H
#define OWQTFILETRANSFERWIDGET_H

#include <QtGui/QWidget>

#include "ui_FileTransferWidget.h"

class ReceiveFileSession;
class SendFileSession;

/**
 * Qt file transfer widget.
 *
 * @author Mathieu Stute
 */
class QtFileTransferWidget : public QWidget {
	Q_OBJECT
public:

	/**
	 * Default constructor.
	 */
	QtFileTransferWidget(QWidget * parent = 0);

	void setDownloadFolder(const QString & folder);

	void addReceiveItem(ReceiveFileSession * fileSession);

	void addSendItem(SendFileSession * fileSession, const std::string & filename, const std::string & contactId);

private Q_SLOTS:

	/**
	 * Clean button has been clicked.
	 */
	void cleanButtonClicked();
	
	/**
	 * Path button has been clicked.
	 */
	void pathButtonClicked();

private:

	void showUploadTab();

	void showDownloadTab();

	Ui::FileTransferWidget _ui;
};

#endif	//OWQTFILETRANSFERWIDGET_H
