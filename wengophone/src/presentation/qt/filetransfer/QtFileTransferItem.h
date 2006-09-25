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

#ifndef OWQTFILETRANSFERITEM_H
#define OWQTFILETRANSFERITEM_H

#include <QtGui/QWidget>

#include <util/Trackable.h>

#include "ui_FileTransferItem.h"

#include <filesessionmanager/IFileSession.h>

/**
 * Qt file transfer item widget.
 * Represents graphically one file transfer.
 * Auto adapt its appearance to its state.
 *
 * TODO: delete file Session objects
 *
 * @author Mathieu Stute
 */
class QtFileTransferItem : public QWidget, public Trackable {
	Q_OBJECT
public:

	enum Type {
		/** Download type */
		Download,

		/** Upload type */
		Upload
	};

	/**
	 * Default constructor.
	 */
	QtFileTransferItem(QWidget * parent, Type type);

	/**
	 * Returns true if the transfer is not finished.
	 * @return true if finished otherwise false.
	 */
	virtual bool isRunning() const = 0;

Q_SIGNALS:

	void stateChangeEvent(const QString & state);

	void progressChangeEvent(int progress);

	// the int is a IFileSession::IFileSessionEvent
	void updateStateEvent(int event);

private Q_SLOTS:

	/**
	 * Set the progress of the transfer.
	 * @param progress progress.
	 */
	void setProgress(int progress);

	/**
	 * Set the state of the transfer.
	 * @param state state.
	 */
	void setState(const QString & state);

	// the int is a IFileSession::IFileSessionEvent
	void updateState(int event);

protected Q_SLOTS:

	/**
	 * TODO:
	 */
	void remove();

	/**
	 * Open the file with the associated application.
	 */
	void open();

	/**
	 * @see ReceiveFileSession::pause().
	 */
	virtual void pause() = 0;

	/**
	 * @see ReceiveFileSession::resume().
	 */
	virtual void resume() = 0;

	/**
	 * @see ReceiveFileSession::stop().
	 */
	virtual void stop() = 0;

protected:

	/**
	 * Set the tranfer filename.
	 * @param filename filename.
	 */
	void setFilename(const QString & filename);

	/**
	 * Set the total size of the file.
	 * @param size size of the file.
	 */
	void setFileSize(int size);

	/**
	 * Set contact name.
	 * @param contact contact name.
	 */
	void setContact(const QString & contact);

	/**
	 * Disconnect buttons.
	 */
	void disconnectButtons();

	/**
	 * Update gui to state finished.
	 */
	void updateButtonsFinished();

	/**
	 * Update gui to state paused.
	 */
	void updateButtonsPaused();

	/**
	 * Update gui to state paused by peer.
	 */
	void updateButtonsPausedByPeer();

	/**
	 * Update gui to state downloading/uploading.
	 */
	void updateButtonsInProgress();

	/**
	 * Emits stateChangeEvent for uploading and downloading.
	 */
	void stateChangeEventDownUp();

	/**
	 * Type (Upload or Download).
	 */
	Type _type;

	/** complete filename */
	QString _filename;

	Ui::FileTransferItem _ui;
};

#endif	//OWQTFILETRANSFERITEM_H
