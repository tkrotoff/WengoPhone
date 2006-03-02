#include "userwidget.h"


UserWidget::UserWidget(QWidget * parent, Qt::WFlags f) : QWidget( parent, f)
{
    // setupUi(this);
    _widget = WidgetFactory::create(":/forms/contactlist/userWidget.ui", NULL);
    QGridLayout * layout = new QGridLayout();
    layout->addWidget(_widget);
    layout->setMargin(0);
    setLayout(layout);
    
}

void UserWidget::setText(const QString & text){
    _text = text;
    
}

const QString & UserWidget::text(){
    return _text;
}

void UserWidget::paintEvent(QPaintEvent * event){
    QPalette  p = palette();
    QPainter painter(this);
    painter.fillRect(rect(),p.brush(QPalette::Window));
}

QFrame * UserWidget::getFrame(){
    return frame;
}

QPixmap * UserWidget::getTux(){
    return _tux;
}
