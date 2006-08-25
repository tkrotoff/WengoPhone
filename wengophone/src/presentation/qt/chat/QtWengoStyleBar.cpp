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

#include "QtWengoStyleBar.h"

#include <qtutil/WengoStyleLabel.h>

#include <QtGui/QGridLayout>
#include <QtCore/QString>

QtWengoStyleBar::QtWengoStyleBar(QWidget * parent) :
	QWidget(parent) {
}

void QtWengoStyleBar::init(WengoStyleLabel * firstLabel, WengoStyleLabel * endLabel) {
	_firstLabel = firstLabel;
	_endLabel = endLabel;

	QGridLayout * layout = new QGridLayout(this);
	layout->addWidget(_firstLabel, 0, 0);
	layout->addWidget(_endLabel, 0, 1);
	layout->setMargin(0);
	layout->setSpacing(0);
}

void QtWengoStyleBar::addLabel(const QString & identifier,
	const QPixmap & normalPixmap, const QPixmap & pressedPixmap,
	const QSize & size, int position) {

	QMutexLocker locker(& _mutex);
	if(!_labels.contains(identifier)){
		QGridLayout * glayout = dynamic_cast<QGridLayout *>(layout());
		WengoStyleLabel * label = new WengoStyleLabel(this);
		label->setPixmaps(normalPixmap, QPixmap(), QPixmap(), pressedPixmap, QPixmap(), QPixmap());
		label->setMinimumSize(size);
		label->setMaximumSize(size);
		_labels.insert(identifier, label);
		if(position < 0 || position > _labelsIndexes.size()){
			position = _labelsIndexes.size();
		}
		_labelsIndexes.insert(position, label);

		// first deplace the end
		glayout->addWidget(_endLabel, 0, glayout->columnCount());
		// then shift the buttons
		for(int i=glayout->columnCount()-3; i>=position+1; i--){
			glayout->addWidget(_labelsIndexes[i], 0, i+1);
		}
		// then add the button
		glayout->addWidget(label, 0, position+1);
	}
}

void QtWengoStyleBar::removeLabel(const QString & identifier){
	if(_labels.contains(identifier)){
		QGridLayout * glayout = dynamic_cast<QGridLayout *>(layout());
		WengoStyleLabel * label = _labels.take(identifier);
		_labelsIndexes.removeAll(label);
		glayout->removeWidget(label);
		delete label;
	}
}

void QtWengoStyleBar::addSeparator(const QString & pixmap, int position) {
	QMutexLocker locker(& _mutex);
	QGridLayout * glayout = dynamic_cast<QGridLayout *>(layout());
	WengoStyleLabel * label = new WengoStyleLabel(this);
	label->setPixmaps(
		QPixmap(pixmap),
		QPixmap(),
		QPixmap(),
		QPixmap(pixmap),
		QPixmap(),
		QPixmap()
	);
	label->setMinimumSize(QSize(3,65));
	label->setMaximumSize(QSize(3,65));
	if(position < 0 || position > _labelsIndexes.size()) {
		position = _labelsIndexes.size();
	}
	_labelsIndexes.insert(position, label);
	_separators.append(label);

	// first deplace the end
	glayout->addWidget(_endLabel, 0, glayout->columnCount());
	// then shift the buttons
	for(int i=glayout->columnCount()-3; i>=position+1; i--) {
		glayout->addWidget(_labelsIndexes[i], 0, i+1);
	}
	// then add the button
	glayout->addWidget(label, 0, position);
}

void QtWengoStyleBar::removeSeparator(int ord) {
	QMutexLocker locker(& _mutex);
	if(ord < 1 || ord >= _separators.size()) {
		return;
	}
	QGridLayout * glayout = dynamic_cast<QGridLayout *>(layout());
	WengoStyleLabel * label = _separators.takeAt(ord);
	_labelsIndexes.removeAll(label);
	_separators.removeAt(ord);
	glayout->removeWidget(label);
	delete label;
}
