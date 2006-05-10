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

#include "QtSliderManager.h"
#include <qtutil/QtClickableLabel.h>

QtSliderManager::QtSliderManager(QObject * parent) : QObject (parent){
}


void QtSliderManager::setVolume(int volume){
    _volume = volume;
    updateLabels();
}

void QtSliderManager::addLabel(QtClickableLabel * label, const QString & onFileName, const QString offFileName){
    privateData data;

    data._label = label;
    data._onFileName = onFileName;
    data._offFileName = offFileName;

    connect (label,SIGNAL(clicked()), SLOT (labelClicked()));

    _dataList.append(data);
}

void QtSliderManager::labelClicked(){

    QObject * s = sender();

    if (s){
        QtClickableLabel * label = dynamic_cast<QtClickableLabel *>(s);
        // Search the index
        for (int i =0; i < _dataList.size(); i++){
            if ( _dataList[i]._label->objectName() == label->objectName() )
            {
                // i is the label index now ( 0 = volume 0% , 6 = volume 100% )
                if ( i == 0 ){
                    _volume = 0;
                    updateLabels();
                    volumeChanged(0);
                    return;
                }
                if ( i == 6 ){
                    _volume = 100;
                    updateLabels();
                    volumeChanged(100);
                }
                _volume = i * 16;
                updateLabels();
                volumeChanged( _volume );
                return;
            }
        }
    }
}


void QtSliderManager::updateLabels(){

    QStringList pixNameList;
    int pos;
    int i = 0;

    if ( _volume == 0 || _volume == 100 ){
        if ( _volume == 0 )
            pos = 0;
        if ( _volume == 100)
            pos = 6;
    }else{
        pos = _volume / 16;
    }

    switch (pos){
        case 0: // If pos == 0, all pixmap are on "OFF"
            for ( int index = 0; index < _dataList.size(); index++)
                pixNameList << _dataList[index]._offFileName;
            break;
        case 6: // If pos == 6, all pixmap are on "ON"
            for ( int index = 0; index < _dataList.size(); index++)
                pixNameList << _dataList[index]._onFileName;
            break;
        default:

            for (i = 0; i <= pos; i++ ){
                pixNameList << _dataList[i]._onFileName;
            }
            for ( int j = i; j < _dataList.size(); j++ ){
                pixNameList << _dataList[j]._offFileName;
            }
    }
    // Setup the label's pixmap
    for ( i = 0; i < _dataList.size(); i++){
        _dataList[i]._label->setPixmap(pixNameList[i]);
        _dataList[i]._label->update();
    }
}

