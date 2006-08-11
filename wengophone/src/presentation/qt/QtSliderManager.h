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

#ifndef QTSLIDERMANAGER_H
#define QTSLIDERMANAGER_H

#include <QtGui/QtGui>

class QtClickableLabel;

class QtSliderManager : public QObject
{
    Q_OBJECT

public:

    QtSliderManager(QObject * parent = 0);

    void setVolume(int volume);

    void addLabel(QtClickableLabel * label, const QString & onFileName, const QString offFileName);

Q_SIGNALS:

    void volumeChanged(int volume);

public Q_SLOTS:

    void labelClicked();

protected:

    typedef struct _privateData{
        QtClickableLabel * _label;
        QString _onFileName;
        QString _offFileName;
    } privateData;

    typedef QList<privateData> DataList;

    void updateLabels();

    DataList _dataList;

    int _volume;

};

#endif // QTSLIDERMANAGER_H
