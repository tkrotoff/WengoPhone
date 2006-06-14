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

#include "QtTreeViewDelegate.h"

#include "QtUserWidgetEventFilter.h"
#include "QtUserWidget.h"
#include "QtUserList.h"
#include "QtContactPixmap.h"
#include "QtContactList.h"

#include <QModelIndex>
#include <QWidget>
#include <QStyleOptionViewItem>
#include <QAbstractItemModel>

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <util/Logger.h>
#include <util/StringList.h>

#include <qtutil/Object.h>

static int GROUP_WIDGET_FRAME_HEIGHT = 22;

QtTreeViewDelegate::QtTreeViewDelegate(CWengoPhone & cWengoPhone, QObject * parent)
: QItemDelegate(parent), _cWengoPhone(cWengoPhone) {
}

void QtTreeViewDelegate::setParent(QWidget * parent) {
	_parent = parent;
}

QWidget * QtTreeViewDelegate::createEditor(QWidget * parent,
const QStyleOptionViewItem &,
const QModelIndex & index) const {
	QtUserList * ul = QtUserList::getInstance();
	QtContact * user = ul->getUser(index.data().toString());

	QtUserWidget * widget = new QtUserWidget(user->getId().toStdString(), _cWengoPhone, parent);

	QWidget * userWidget = Object::findChild < QWidget * > (widget, "UserWidget");

	QFrame * userTitleFrame = Object::findChild < QFrame * > (userWidget, "userTitleFrame");

	userWidget->installEventFilter(new QtUserWidgetEventFilter((QObject *) this, userWidget, user));

	return (QWidget *) widget;
}

void QtTreeViewDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const {
	QtUserWidget * widget = qobject_cast < QtUserWidget * > (editor);

	if (!widget) {
		return;
	}
	widget->setText(index.model()->data(index).toString());
}

void QtTreeViewDelegate::setModelData(QWidget * editor, QAbstractItemModel * model,
    const QModelIndex & index) const {
	QtUserWidget * widget = qobject_cast < QtUserWidget * > (editor);

	if (!widget) {
		return;
	}
	model->setData(index, widget->text());
}

void QtTreeViewDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
	if (!index.parent().isValid()) {
		drawGroup(painter, option, index);
	} else {
		QtUserList * ul = QtUserList::getInstance();
		if (ul) {
			ul->paintUser(painter, option, index);
		}
	}
}

QSize QtTreeViewDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const {
	QSize orig = QItemDelegate::sizeHint(option, index);
	QtUserList * ul = QtUserList::getInstance();
	QtContact * user = ul->getUser(index.data().toString());

	if (user) {
		return QSize(orig.width(), ul->getUser(index.data().toString())->getHeight());
	} else {
		if (!index.parent().isValid()) {
			return (QSize(orig.width(), GROUP_WIDGET_FRAME_HEIGHT));
		}
	}
	return orig;
}
bool QtTreeViewDelegate::checkForUtf8(const unsigned char * text, int size) const {
	bool isUtf8 = false;
	if (size==0)
		return true;

	if ( (text[0]<=0x7F) && size==1){
		return true;
	}
	for (int i=0;i<size;i++){
		if (text[i] == 0 ){
			return false;
		}
		if (text[i]>0x7F){
			if ((text[i] & 0xC0) == 0xC0){
				if (i+1 > size)
					return false;
				if ((text[i+1] & 0xC0) == 0x80){
					isUtf8=true;
					return isUtf8;
				}
			}
		}
	}
	return isUtf8;
}
void QtTreeViewDelegate::drawGroup(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
	QRect r;
	QtContactPixmap * spx;
	QPixmap px;
	int x;
	spx = QtContactPixmap::getInstance();
	painter->setPen(option.palette.text().color());
	r = option.rect;

	QLinearGradient lg(QPointF(1, option.rect.top()), QPointF(1, option.rect.bottom()));
	lg.setColorAt(.8, QColor(212, 208, 200));
	lg.setColorAt(0, QColor(255, 255, 255));
	painter->fillRect(option.rect, QBrush(lg));

	QFont f = option.font;
	f.setBold(true);
	painter->setFont(f);

	if (option.state & QStyle::State_Open)
		px = spx->getPixmap(QtContactPixmap::ContactGroupOpen);
	else px = spx->getPixmap(QtContactPixmap::ContactGroupClose);

	x = option.rect.left();

	painter->drawPixmap(x, r.top() + 3, px);
	x += px.width() + 3;
	r.setLeft(x);

	QFont font = painter->font();

	int y = ((r.bottom() - r.top()) - QFontMetrics(font).height()) / 2;

	r.setTop(y + r.top());

	r.setLeft(r.left() + 10);

	// Number of child
	int nbchild = index.model()->rowCount(index);
	QString groupName;
	std::string groupId;
	std::string groupNameTmp;

	if (index.data().toString() == QtContactList::DEFAULT_GROUP_NAME){
		groupName = tr("Contacts list");
		groupNameTmp=std::string(groupName.toUtf8().data());
	} else {
		groupName = index.data().toString();
		groupId = groupName.toStdString();
		groupNameTmp = _cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCContactList().getContactGroupName(groupId);
	}
	if (checkForUtf8((const unsigned char *)(groupNameTmp.c_str()), groupNameTmp.size())){
			groupName=QString::fromUtf8(groupNameTmp.c_str(), groupNameTmp.size());
	} else {
		groupName=QString::fromStdString(groupNameTmp);
	}
	QString str = groupName;
	painter->drawText(r, Qt::AlignLeft, str, 0);
}
