
#ifndef QtTreeViewDelegate_H
#define QtTreeViewDelegate_H
#include <QtGui>
#include <QItemDelegate>
#include <QLineEdit>
#include <QDebug>
#include "userwidget.h"

class QtTreeViewDelegate : public QItemDelegate
{
    Q_OBJECT
public:
	QtTreeViewDelegate(QObject *parent = 0);
    
	virtual QWidget * createEditor(QWidget *parent,const QStyleOptionViewItem & option ,const QModelIndex &index) const;
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	virtual void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
	void setParent(QWidget * parent);
	virtual QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
protected:
    
    const	QWidget *	_parent;
    QPixmap				_menuIcon;
    
};

#endif

