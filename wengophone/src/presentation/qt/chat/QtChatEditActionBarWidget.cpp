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

#include "QtChatEditActionBarWidget.h"

#include <presentation/qt/chat/QtChatWidget.h>

#include <qtutil/WengoStyleLabel.h>

#include <QtCore/QString>

static const QString CHAT_EMOTICONS_LABEL_OFF_BEGIN = ":/pics/chat/chat_emoticon_button.png";
static const QString CHAT_EMOTICONS_LABEL_OFF_END = ":/pics/profilebar/bar_separator.png";
static const QString CHAT_EMOTICONS_LABEL_OFF_FILL = ":/pics/profilebar/bar_fill.png";
static const QString CHAT_EMOTICONS_LABEL_ON_BEGIN = ":/pics/chat/chat_emoticon_button_on.png";
static const QString CHAT_EMOTICONS_LABEL_ON_END = ":/pics/profilebar/bar_separator.png";
static const QString CHAT_EMOTICONS_LABEL_ON_FILL = ":/pics/profilebar/bar_on_fill.png";
static const QString CHAT_FONT_LABEL_OFF_END = ":/pics/profilebar/bar_end.png";
static const QString CHAT_FONT_LABEL_OFF_FILL = ":/pics/profilebar/bar_fill.png";
static const QString CHAT_FONT_LABEL_ON_END = ":/pics/profilebar/bar_on_end.png";
static const QString CHAT_FONT_LABEL_ON_FILL = ":/pics/profilebar/bar_on_fill.png";

QtChatEditActionBarWidget::QtChatEditActionBarWidget(QtChatWidget * chatWidget, QWidget * parent) :
	QtWengoStyleBar(parent), _chatWidget(chatWidget) {

	setMaximumSize(QSize(10000, 70));
	setMinimumSize(QSize(16, 70));

	_emoticonsLabel = new WengoStyleLabel(this);
	_emoticonsLabel->setPixmaps(
		QPixmap(CHAT_EMOTICONS_LABEL_OFF_BEGIN),
		QPixmap(CHAT_EMOTICONS_LABEL_OFF_END),
		QPixmap(CHAT_EMOTICONS_LABEL_OFF_FILL),
		QPixmap(CHAT_EMOTICONS_LABEL_ON_BEGIN),
		QPixmap(CHAT_EMOTICONS_LABEL_ON_END),
		QPixmap(CHAT_EMOTICONS_LABEL_ON_FILL)
	);
	_emoticonsLabel->setMaximumSize(QSize(120,65));
	_emoticonsLabel->setMinimumSize(QSize(120,65));
	_emoticonsLabel->setText(tr(" emoticons  "));
	_emoticonsLabel->setTextColor(Qt::white);
	_emoticonsLabel->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	connect(_emoticonsLabel, SIGNAL(clicked()), SLOT(emoticonsLabelClickedSlot()));

	_endLabel = new WengoStyleLabel(this);
	_endLabel->setPixmaps(
		QPixmap(),
		QPixmap(":/pics/profilebar/bar_end.png"),
		QPixmap(":/pics/profilebar/bar_fill.png"),
		QPixmap(),
		QPixmap(":/pics/profilebar/bar_end.png"),
		QPixmap(":/pics/profilebar/bar_fill.png")
	);
	_endLabel->setMinimumSize(QSize(16, 65));

	init(_emoticonsLabel, _endLabel);

	// add font label
	addLabel(QString("fontLabel"),
			 QPixmap(":/pics/chat/invite.png"),
			 QPixmap(":/pics/chat/invite_on.png"),
			 QSize(36, 65)
	);

	connect(_labels["fontLabel"], SIGNAL(clicked()), SLOT(fontLabelClickedSlot()));
	////
}

void QtChatEditActionBarWidget::emoticonsLabelClickedSlot() {
	emoticonsLabelClicked();
}

void QtChatEditActionBarWidget::fontLabelClickedSlot() {
	fontLabelClicked();
}
