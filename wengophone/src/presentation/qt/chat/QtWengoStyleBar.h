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

#ifndef OWQTWENGOSTYLEBAR_H
#define OWQTWENGOSTYLEBAR_H

#include <QtGui/QWidget>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QMutex>

class QtWengoStyleLabel;
class QString;

/**
 *
 * @author Nicolas Couturier
 * @author Mathieu Stute
 */
class QtWengoStyleBar : public QWidget {
	Q_OBJECT
public:

	/**
	 * Constructor.
	 */
	QtWengoStyleBar(QWidget * parent);

	/**
	 * Initialize the bar with the first & the last label.
	 */
	void init(QtWengoStyleLabel * firstLabel, QtWengoStyleLabel * endLabel);

	/**
	 * Adds a new button in the action bar associated to a unique text-based identifier.
	 *
	 * @param identifier : the text based identifier to associate with the button.
	 * @param normalPixmap : the image to be used for the button.
	 * @param pressedPixmap : the image to be used when the button is pressed.
	 * @param size : the size of the button.
	 * @param position : if specified, th e button is inserted at that position, else inserted at the end.
	 */
	void addLabel(const QString & identifier, const QPixmap & normalPixmap,
		const QPixmap & pressedPixmap, const QSize & size, int position = -1);

	/**
	 * Removes the button associated to the text-based identifier.
	 *
	 * @param identifier : the text based identifier of the button to remove.
	 */
	void removeLabel(const QString & identifier);

	/**
	 * Add a separator.
	 *
	 * @param position : if specified, the position where to insert the separator, else insert a the end.
	 */
	void addSeparator(const QString & pixmap, int position = -1);

	/**
	 * Removes a separator at a certain rank.
	 *
	 * @param ord : the rank of the separator to remove.
	 */
	void removeSeparator(int ord);

	/**
	 * Used to keep track of the added labels by string indentifiers.
	 */
	QMap<QString, QtWengoStyleLabel *> _labels;

private:

	/**
	 * The first QtWengoStyleLabel.
	 */
	QtWengoStyleLabel * _firstLabel;

	/**
	 * The "unremovable" end of the action bar.
	 */
	QtWengoStyleLabel * _endLabel;

	/**
	 * Used to keep track of the indexes of the labels in the layout and be abel to get them.
	 */
	QList<QtWengoStyleLabel *> _labelsIndexes;

	/**
	 * Used to keep track of the separators.
	 */
	QList<QtWengoStyleLabel *> _separators;

	QMutex _mutex;
};

#endif	//OWQTWENGOSTYLEBAR_H
