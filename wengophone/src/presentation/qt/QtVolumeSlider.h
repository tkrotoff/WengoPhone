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

#ifndef OWQTVOLUMESLIDER_H
#define OWQTVOLUMESLIDER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>

class ClickableLabel;

/**
 * Volume slider control inside the main window.
 *
 * @see QtConfigPanel
 * @author Tanguy Krotoff
 */
class QtVolumeSlider : public QObject {
	Q_OBJECT
public:

	QtVolumeSlider(QObject * parent);

	void setVolume(int volume);

	void addLabel(ClickableLabel * label, const QString & activePixmapFilename, const QString & pixmapFilename);

Q_SIGNALS:

	void volumeChanged(int volume);

private Q_SLOTS:

	void labelClicked();

private:

	void updateLabels();

	typedef struct _privateData {
		ClickableLabel * _label;
		QString _activePixmapFilename;
		QString _pixmapFilename;
	} privateData;

	typedef QList<privateData> DataList;

	DataList _dataList;

	int _volume;
};

#endif	//OWQTVOLUMESLIDER_H
