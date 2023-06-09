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

#ifndef OWQTDIALPAD_H
#define OWQTDIALPAD_H

#include <util/NonCopyable.h>

#include <QtGui/QWidget>

#include <string>

class QtWengoPhone;

class QStringList;
namespace Ui { class DialpadWidget; }

/**
 * Dialpad widget: where the user clicks for DTMF.
 *
 * @author Tanguy Krotoff
 */
class QtDialpad : public QWidget, NonCopyable {
	Q_OBJECT
public:

	QtDialpad(QtWengoPhone * qtWengoPhone);

	~QtDialpad();

private Q_SLOTS:

	void oneButtonClicked();

	void twoButtonClicked();

	void threeButtonClicked();

	void fourButtonClicked();

	void fiveButtonClicked();

	void sixButtonClicked();

	void sevenButtonClicked();

	void eightButtonClicked();

	void nineButtonClicked();

	void zeroButtonClicked();

	void starButtonClicked();

	void poundButtonClicked();

	void audioSmileysComboBoxActivated(int index);

private:

	/**
	 * FIXME put this intelligence inside model/control.
	 */
	void playTone(const std::string & tone);

	/**
	 * FIXME put this intelligence inside model/control.
	 */
	QStringList getListAudioSmileys() const;

	Ui::DialpadWidget * _ui;

	QtWengoPhone * _qtWengoPhone;
};

#endif	//OWQTDIALPAD_H
