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

#ifndef HOMEPAGEWIDGET_H
#define HOMEPAGEWIDGET_H

#include <qobject.h>
#include <qvariant.h>

class QWidget;
class QAxWidget;
class QString;

/**
 * Home page of the softphone.
 * Integrate the Internet Explorer ActiveX
 *
 * @author Mathieu Stute
 */
class HomePageWidget : public QObject {
	Q_OBJECT
public:

	HomePageWidget(QWidget * parent);

	~HomePageWidget();
	
	/**
	 * Set active page to the Self Care of the current user
	 */
	void goToSoftphoneWeb(bool Switch);

	/**
	 * Gets the low-level gui widget of this class.
	 *
	 * @return low-level widget
	 */
	QWidget * getWidget() {
		return _homePageWidget;
	}
	
	/**
	 * Gets the IE ActiveX widget of this class.
	 *
	 * @return Internet Explorer ActiveX widget
	 */
	QAxWidget * getIEActiveX(){
		return _ieActiveX;
	}
	
	/**
	 * Set the Internet Explorer AcitiveX url
	 *
	 * @param url
	 */
	void setUrl(const QString & url);
	
	/**
	 * Set the Internet Explorer AcitiveX url with post data
	 *
	 * @param url 
	 * @param postdata
	 */
	void setUrl(const QString & url, const QString & postData);
	

private slots:
	
	void BeforeNavigateSlot(const QString&,int,const QString&,const QVariant&,const QString&,bool&);

private:

	HomePageWidget(const HomePageWidget &);
	HomePageWidget & operator=(const HomePageWidget &);

	/**
	 * Return the anchor of an url
	 * @return the anchor of the url. An empty QString if the url contains no anchor
	 */
	QString getAnchor(QString url);

	/**
	 * Low-level widget of this class.
	 */
	QWidget * _homePageWidget;
	
	/**
	 * The Internet Exporer ActiveX
	 */
	QAxWidget * _ieActiveX;
	
	static const QString ANCHOR_CONTACTLIST;
	
	static const QString ANCHOR_HISTORY;
	
	static const QString ANCHOR_CONFIGURATION;
	
	static const QString ANCHOR_DIALPAD;
	
	static const QString ANCHOR_ADDCONTACT;
	
	static const QString ANCHOR_SELFCARE;

	static const QString ANCHOR_FORUM;
};

#endif	//HOMEPAGEWIDGET_H
