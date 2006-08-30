#ifndef UI_CHATCONTACTLISTITEM_H
#define UI_CHATCONTACTLISTITEM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

class Ui_ChatContactListItem
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *spacerItem;
    QLabel *pictureLabel;
    QLabel *nicknameLabel;

    void setupUi(QWidget *ChatContactListItem)
    {
    ChatContactListItem->setObjectName(QString::fromUtf8("ChatContactListItem"));
    ChatContactListItem->resize(QSize(120, 50).expandedTo(ChatContactListItem->minimumSizeHint()));
    gridLayout = new QGridLayout(ChatContactListItem);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(9);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(spacerItem, 0, 2, 1, 1);

    pictureLabel = new QLabel(ChatContactListItem);
    pictureLabel->setObjectName(QString::fromUtf8("pictureLabel"));
    pictureLabel->setPixmap(QPixmap(QString::fromUtf8(":/pics/config/accounts.png")));
    pictureLabel->setScaledContents(false);

    gridLayout->addWidget(pictureLabel, 0, 0, 1, 1);

    nicknameLabel = new QLabel(ChatContactListItem);
    nicknameLabel->setObjectName(QString::fromUtf8("nicknameLabel"));

    gridLayout->addWidget(nicknameLabel, 0, 1, 1, 1);

    retranslateUi(ChatContactListItem);

    QMetaObject::connectSlotsByName(ChatContactListItem);
    } // setupUi

    void retranslateUi(QWidget *ChatContactListItem)
    {
    ChatContactListItem->setWindowTitle(QApplication::translate("ChatContactListItem", "Form", 0, QApplication::UnicodeUTF8));
    pictureLabel->setText(QApplication::translate("ChatContactListItem", "", 0, QApplication::UnicodeUTF8));
    nicknameLabel->setText(QApplication::translate("ChatContactListItem", "robobob", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(ChatContactListItem);
    } // retranslateUi

};

namespace Ui {
    class ChatContactListItem: public Ui_ChatContactListItem {};
} // namespace Ui

#endif // UI_CHATCONTACTLISTITEM_H
