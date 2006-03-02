#ifndef USERWIDGET_H
#define USERWIDGET_H

#include <WidgetFactory.h>
#include <QtGui>


class UserWidget : public QWidget
{
    Q_OBJECT
public:
    UserWidget(QWidget * parent = 0, Qt::WFlags f=0);
    void setText(const QString & text);
    const QString & text();
    virtual void paintEvent(QPaintEvent * event);
    QFrame *    getFrame();
    QPixmap *   getTux();
    
protected:
    QString     _text;
    QPixmap  *  _tux;
	QFrame   *  frame;
	QWidget * _widget;
};

#endif
