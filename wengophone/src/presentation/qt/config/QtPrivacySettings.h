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

#ifndef QTPRIVACYSETTINGS_H
#define QTPRIVACYSETTINGS_H


#include <QtGui>
#include <qtutil/WidgetFactory.h>

class QtPrivacySettings : public QWidget
{
    Q_OBJECT

public:
    QtPrivacySettings (QWidget * parent = 0, Qt::WFlags f=0);

    void saveData();

protected:

    QWidget     *   _widget;

	void setupChilds();

	void readConfigData();

    QRadioButton * _allowCallFromAnyoneRadioButton;

    QRadioButton * _allowCallOnlyFromContactListRadioButton;

    QRadioButton * _allowChatsFromAnyoneRadioButton;

    QRadioButton * _allowChatFromOnlyContactListRadioButton;

    QCheckBox * _alwaysSignAsInvisible;

    QPushButton * _manageBlockedUserPushButton;

};
#endif

