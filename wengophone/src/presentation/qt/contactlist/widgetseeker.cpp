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

#include <widgetseeker.h>
#include <QDebug>

WidgetSeeker::WidgetSeeker()
{
}

QLineEdit * WidgetSeeker::getLineEdit(QWidget * parent, const QString & widgetName){
    QLineEdit * tmp;
    tmp = parent->findChild<QLineEdit *>(widgetName);
    if (!tmp)
        qDebug() << "Warning ! Unable to find Widget"<< widgetName;
    return tmp;
}

QCheckBox * WidgetSeeker::getCheckBox(QWidget * parent, const QString & widgetName){
    QCheckBox * tmp;
    tmp = parent->findChild<QCheckBox *>(widgetName);
    if (!tmp)
        qDebug() << "Warning ! Unable to find Widget"<< widgetName;
    return tmp;
}
QTableWidget * WidgetSeeker::getTableWidget(QWidget * parent, const QString & widgetName){
    QTableWidget * tmp;
    tmp = parent->findChild<QTableWidget *>(widgetName);
    if (!tmp)
        qDebug() << "Warning ! Unable to find Widget"<< widgetName;
    return tmp;
}

QRadioButton * WidgetSeeker::getRadioButton(QWidget * parent, const QString & widgetName){
    QRadioButton * tmp;
    tmp = parent->findChild<QRadioButton *>(widgetName);
    if (!tmp)
        qDebug() << "Warning ! Unable to find Widget"<< widgetName;
    return tmp;
}

QSpinBox * WidgetSeeker::getSpinBox(QWidget * parent, const QString & widgetName){
    QSpinBox * tmp;
    tmp = parent->findChild<QSpinBox *>(widgetName);
    if (!tmp)
        qDebug() << "Warning ! Unable to find Widget"<< widgetName;
    return tmp;
}

QLabel * WidgetSeeker::getLabel(QWidget * parent, const QString & widgetName){
    QLabel * tmp;
    tmp = parent->findChild<QLabel *>(widgetName);
    if (!tmp)
        qDebug() << "Warning ! Unable to find Widget"<< widgetName;
    return tmp;
}
QPushButton * WidgetSeeker::getPushButton(QWidget * parent, const QString & widgetName)
{
    QPushButton * tmp;
    tmp = parent->findChild<QPushButton *>(widgetName);
    if (!tmp)
        qDebug() << "Warning ! Unable to find Widget"<< widgetName;
    return tmp;
}

QTextEdit * WidgetSeeker::getTextEdit(QWidget * parent, const QString & widgetName)
{
    QTextEdit * tmp;
    tmp = parent->findChild<QTextEdit *>(widgetName);
    if (!tmp)
        qDebug() << "Warning ! Unable to find Widget"<< widgetName;
    return tmp;
}
QTextBrowser * WidgetSeeker::getTextBrowser(QWidget * parent, const QString & widgetName)
{
    QTextBrowser * tmp;
    tmp = parent->findChild<QTextBrowser *>(widgetName);
    if (!tmp)
        qDebug() << "Warning ! Unable to find Widget"<< widgetName;
    return tmp;
}

QTabWidget * WidgetSeeker::getTabWidget(QWidget * parent, const QString & widgetName)
{
    QTabWidget * tmp;
    tmp = parent->findChild<QTabWidget *>(widgetName);
    if (!tmp)
        qDebug() << "Warning ! Unable to find Widget"<< widgetName;
    return tmp;

}

const QString WidgetSeeker::widgetClass(QWidget * widget){
    QString tmp(widget->metaObject()->className());
    return tmp;
}



void WidgetSeeker::makeWidgetDictionary(QWidget * parent){
    QCheckBox * checkBox;
    
    QList<QWidget *> allWidgets = parent->findChildren<QWidget *>();
    for (int i=0;i<allWidgets.size();i++){
        
        if ( widgetClass(allWidgets[i]) == "QLineEdit" ){
            _dict[allWidgets[i]->objectName()]=qobject_cast<QLineEdit *>(allWidgets[i])->text();
            
        }
        if ( widgetClass(allWidgets[i]) == "QCheckBox" ){
            checkBox = qobject_cast<QCheckBox *>(allWidgets[i]);
            
            switch (checkBox->checkState())
            {
                case Qt::Checked:
                    _dict[allWidgets[i]->objectName()]="yes";
                    break;
                case Qt::Unchecked:
                    _dict[allWidgets[i]->objectName()]="no";
                    break;
                case Qt::PartiallyChecked:
                    _dict[allWidgets[i]->objectName()]="partial";
                    break;
            }
        }
    }
}
