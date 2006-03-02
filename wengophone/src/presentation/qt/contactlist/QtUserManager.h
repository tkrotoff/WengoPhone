#ifndef QtUserManager_H
#define QtUserManager_H

#include <QtGui>


#define MAX_ITEM_SIZE 100
#define MIN_ITEM_SIZE 28


class QtUserManager : public QObject
{
    Q_OBJECT
    
public:
    QtUserManager(QObject * parent = 0, QTreeWidget * target = 0);

public Q_SLOTS:
    void treeViewSelectionChanged();
    void itemClicked ( QTreeWidgetItem * item, int column );
	void itemEntered ( QTreeWidgetItem * item);
	void openUserInfo( QTreeWidgetItem * i);
	void closeUserInfo();
	
protected:
   QTreeWidget      *    _tree;
   QTreeWidgetItem  *    _previous;
   QTreeWidgetItem  *    _lastClicked;
   QSize            *    _itemSize;
   QMenu            *    _menu; 
};

#endif

