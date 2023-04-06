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

#ifndef GENERALCONFIGWIDGET_H
#define GENERALCONFIGWIDGET_H

#include <observer/Observer.h>

#include <qobject.h>

class Startup;
class QWidget;
class QCheckBox;

/**
 * General configuration component of the ConfigWindow.
 *
 * Contains the option: run WengoPhone when Windows starts.
 *
 * @author Tanguy Krotoff
 */
class GeneralConfigWidget : public QObject/*, public Observer */{
	Q_OBJECT
public:

	GeneralConfigWidget(QWidget * parent);

	~GeneralConfigWidget();

	/**
	 * Gets the low-level widget.
	 *
	 * @return the low-level widget for this gui component
	 */
	QWidget * getWidget() const {
		return _generalConfigWidget;
	}
	
	QCheckBox * getUseHttpCheckBox() {
		return _useHttpCheckBox;
	}

	//void update(const Subject &, const Event & event);

private slots:

	/**
	 * Sets the application as a startup application.
	 *
	 * Contains the option: run WengoPhone when Windows starts.
	 *
	 * @param startup
	 */
	void setStartup(bool startup);

private:

	GeneralConfigWidget(const GeneralConfigWidget &);
	GeneralConfigWidget & operator=(const GeneralConfigWidget &);

	/**
	 * The low-level widget of this gui component.
	 */
	QWidget * _generalConfigWidget;

	QCheckBox * _useHttpCheckBox;

	Startup * _startup;
};

#endif	//GENERALCONFIGWIDGET_H
