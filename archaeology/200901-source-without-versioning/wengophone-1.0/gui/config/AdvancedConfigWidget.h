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

#ifndef ADVANCEDCONFIGWIDGET_H
#define ADVANCEDCONFIGWIDGET_H

#include "AdvancedConfig.h"

#include <observer/Observer.h>

#include <qobject.h>

class QWidget;
class QLineEdit;
class QComboBox;
class QPushButton;
class QCheckBox;

/**
 * Class designed to modify advanced settings.
 *
 * @author Philippe Kajmar
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class AdvancedConfigWidget : public QObject {
	Q_OBJECT
public:

	/// nat indexes
	enum index {
		AUTOMATIC_INDEX,
		NONE_INDEX,
		FULL_CONE_INDEX,
		RESTRICT_CONE_INDEX,
		PORT_RESTRICT_CONE_INDEX,
		SYMMETRIC_INDEX
	};

	AdvancedConfigWidget(QWidget * parent);

	/**
	 * Gets the low-level widget.
	 *
	 * @return the low-level widget for this gui component
	 */
	QWidget * getWidget() const {
		return _advancedConfigWidget;
	}

	void update(const Subject &, const Event & event);

	/// save changes
	void save();
	
	QCheckBox * getUseHttpCheckBox() {
		return _useHttpCheckBox;
	}
	
signals:

	/// ask to initialize sip
	//TODO to remove, not used anymore
	void askForInit();
	
private slots:
	
	/// set default settings
	void defaultButtonClicked();
	
	void getProxySettings();
	
	void setUseHttp(bool);
	
private:
	
	AdvancedConfigWidget(const AdvancedConfigWidget &);
	AdvancedConfigWidget & operator=(const AdvancedConfigWidget &);
	
	/// apply changes
	//TODO to remove
	void apply();
	
	/// translate the name of a nat type
	/// into an index for the combo box
	AdvancedConfigWidget::index nameToIndex(const QString & natType);
	
	/// translate an index
	/// into a name of nat type
	const char * indexToName(int index);

	/// graphical component
	QWidget * _advancedConfigWidget;

	/// sip port line edit
	QLineEdit * _portLineEdit;

	/// nat combo box
	QComboBox * _natComboBox;

	QCheckBox * _useHttpCheckBox;

	QLineEdit * _httpUrlLineEdit;

	QLineEdit * _httpPortLineEdit;
	
	QLineEdit * _proxyUserIdLineEdit;

	QLineEdit * _proxyUserPasswordLineEdit;

	QCheckBox * _echoCancellerCheckBox;
	
	QCheckBox * _authenticationCheckBox;

	QCheckBox * _sslCheckBox;
	
	/// default button
	QPushButton * _defaultButton;

	/// subject
	AdvancedConfig & _advancedConfig;
};

#endif	//ADVANCEDCONFIGWIDGET_H
