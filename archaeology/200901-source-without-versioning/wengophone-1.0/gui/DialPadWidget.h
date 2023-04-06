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

#ifndef DIALPADWIDGET_H
#define DIALPADWIDGET_H

#include <vector>

#include <qobject.h>
#include <qstring.h>

class QWidget;
class QPushButton;
class QComboBox;
class QStringList;

/**
 * Gui widget for a dial pad: a pad like in standard phones.
 *
 * Used to compose DTMF. Also constituted by the call and hang up buttons.
 *
 * @author Tanguy Krotoff
 */
class DialPadWidget : public QObject {
	Q_OBJECT
public:

	DialPadWidget(QWidget * parent, QComboBox & phoneComboBox);

	/**
	 * Gets the low-level widget.
	 *
	 * @return the low-level widget for this gui component
	 */
	QWidget * getWidget() const {
		return _dialPadWidget;
	}

	/// get the number buttons
	std::vector<QPushButton *> getNumberButtons() const;

	/// get the star button
	QPushButton * getStarButton() const;

	/// get the sharp button
	QPushButton * getSharpButton() const;

private slots:

	/// play 1 button DTMF
	void oneButtonClicked();

	/// play 2 button DTMF
	void twoButtonClicked();

	/// play 3 button DTMF
	void threeButtonClicked();

	/// play 4 button DTMF
	void fourButtonClicked();

	/// play 5 button DTMF
	void fiveButtonClicked();

	/// play 6 button DTMF
	void sixButtonClicked();

	/// play 7 button DTMF
	void sevenButtonClicked();

	/// play 8 button DTMF
	void eightButtonClicked();

	/// play 9 button DTMF
	void nineButtonClicked();

	/// play 0 button DTMF
	void zeroButtonClicked();

	/// play * button DTMF
	void starButtonClicked();

	/// play # button DTMF
	void sharpButtonClicked();

private:

	DialPadWidget(const DialPadWidget &);
	DialPadWidget & operator=(const DialPadWidget &);

	QStringList getListDtmfTheme() const;

	void playDTMF(const QString & soundFile);

	/**
	 * The low-level widget of this gui component.
	 */
	QWidget * _dialPadWidget;

	/**
	 * Button to make a call.
	 */
	QPushButton * _callButton;

	/**
	 * Button to hang up the phone.
	 */
	QPushButton * _hangUpButton;

	/**
	 * To show the DTMF digits.
	 */
	QComboBox & _phoneComboBox;

	/// Button to dial 1
	QPushButton * _oneButton;

	/// Button to dial 2
	QPushButton * _twoButton;

	/// Button to dial 3
	QPushButton * _threeButton;

	/// Button to dial 4
	QPushButton * _fourButton;

	/// Button to dial 5
	QPushButton * _fiveButton;

	/// Button to dial 6
	QPushButton * _sixButton;

	/// Button to dial 7
	QPushButton * _sevenButton;

	/// Button to dial 8
	QPushButton * _eightButton;

	/// Button to dial 9
	QPushButton * _nineButton;

	/// Button to dial 0
	QPushButton * _zeroButton;

	/// Button to dial *
	QPushButton * _starButton;

	/// Button to dial #
	QPushButton * _sharpButton;

	QComboBox * _dtmfThemeComboBox;

	QString _dtmfSoundsPath;
};

#endif	//DIALPADWIDGET_H
