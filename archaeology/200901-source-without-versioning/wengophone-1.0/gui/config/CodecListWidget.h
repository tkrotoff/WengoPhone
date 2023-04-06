/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#ifndef CODECLISTWIDGET_H
#define CODECLISTWIDGET_H

#include <qobject.h>

class Codec;
class CodecList;
class QListView;
class QListViewItem;
class QPushButton;

/**
 * Gui component designed to modify
 * codecs preferences.
 *
 * @author Philippe Kajmar
 * @author Tanguy Krotoff
 */
class CodecListWidget : public QObject {
	Q_OBJECT
public:

	/**
	 * Constructs a CodecListWidget using a CodecList.
	 *
	 * @param codecList CodecList that will represent CodecListWidget
	 * @param parent pointer to the parent widget
	 */
	CodecListWidget(CodecList & codecList, QWidget * parent);

	/**
	 * Gets the low-level widget.
	 *
	 * @return the low-level widget for this gui component
	 */
	QWidget * getWidget() const {
		return _codecListWidget;
	}

	/**
	 * Saves the CodecList configuration.
	 */
	void save();

private slots:

	/**
	 * A new QListViewItem (Codec) has been selected from the ContactListWidget.
	 *
	 * @param codecSelected QListViewItem (Codec) that has been selected
	 */
	void selectionChanged(QListViewItem * codecSelected);

	/**
	 * Increases the priority of a Codec.
	 */
	void upButtonClicked();

	/**
	 * Decreases the priority of a Codec.
	 */
	void downButtonClicked();

	/**
	 * Loads the default CodecList.
	 */
	void defaultButtonClicked();

private:

	CodecListWidget(const CodecListWidget &);
	CodecListWidget & operator=(const CodecListWidget &);

	/**
	 * Reads the priorities of the codecs from the gui component.
	 */
	void readPriorities();

	/**
	 * Loads the entire list of Codec into this graphical component.
	 */
	void loadCodecList();

	/**
	 * Enables or not the up and down buttons depending on the item selected.
	 *
	 * @param codecSelected item selected
	 */
	void enableButtons(QListViewItem * codecSelected);

	/**
	 * List of Codec reprensented by this graphical component.
	 */
	CodecList & _codecList;

	/**
	 * Codec selected by the user.
	 */
	Codec * _codecSelected;

	/**
	 * Low-level widget for this gui component.
	 */
	QWidget * _codecListWidget;

	/**
	 * List of Codec (QListView).
	 */
	QListView * _codecListView;

	/**
	 * Up button: sets a Codec with a higher priority.
	 */
	QPushButton * _upButton;

	/**
	 * Down button: sets a Codec with a lower priority.
	 */
	QPushButton * _downButton;

	/**
	 * Loads the default CodecList when clicked.
	 */
	QPushButton * _defaultButton;
};

#endif	//CODECLISTWIDGET_H
