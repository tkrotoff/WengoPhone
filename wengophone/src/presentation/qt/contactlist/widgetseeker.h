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

#ifndef WIDGETSEEKER_H
#define WIDGETSEEKER_H

#include <QtGui>

class WidgetSeeker
{
public:
    WidgetSeeker();
    
    QLineEdit * getLineEdit(QWidget * parent, const QString & widgetName);
    QCheckBox * getCheckBox(QWidget * parent, const QString & widgetName);
    QRadioButton * getRadioButton(QWidget * parent, const QString & widgetName);
    QSpinBox * getSpinBox(QWidget * parent, const QString & widgetName);
    QTableWidget * getTableWidget(QWidget * parent, const QString & widgetName);
    QLabel * getLabel(QWidget * parent, const QString & widgetName);
    QPushButton * getPushButton(QWidget * parent, const QString & widgetName);
    QTextEdit * getTextEdit(QWidget * parent, const QString & widgetName);
    QTextBrowser * getTextBrowser(QWidget * parent, const QString & widgetName);
    QTabWidget * getTabWidget(QWidget * parent, const QString & widgetName);
    
    const QString widgetClass(QWidget * widget);
    void  makeWidgetDictionary(QWidget * parent);
protected:
        QMap<QString,QVariant> _dict;
};

#endif
