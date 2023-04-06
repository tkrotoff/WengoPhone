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

#ifndef SEARCHPAGEWIDGET_H
#define SEARCHPAGEWIDGET_H

#include <qobject.h>
#include <qvariant.h>
#include <observer/Observer.h>

class QWidget;
class QAxWidget;
class QString;

/**
 * Search buddies widget
 * Based on Internet Explorer ActiveX
 *
 * @author Mathieu Stute
 */
class SearchPageWidget : public QObject, public Observer {
	Q_OBJECT
public:
	
	/**
	 * Constructor
	 *
	 * @param parent the parent widget
	 */
	SearchPageWidget(QWidget * parent);
	
	/**
	 * Destructor
	 */
	~SearchPageWidget();
	
	/**
	 * Reload page
	 */
	void reloadPage();
	
	/**
	 * Gets the low-level gui widget of this class.
	 *
	 * @return low-level widget
	 */
	QWidget * getWidget() {
		return _searchPageWidget;
	}
	
	void update(const Subject &, const Event & event);
	
private slots:
	
	/**
	 * Slot called before a url change
	 */
	void BeforeNavigateSlot(const QString&,int,const QString&,const QVariant&,const QString&,bool&);
	
private:
	
	SearchPageWidget(const SearchPageWidget &);
	SearchPageWidget & operator=(const SearchPageWidget &);
	
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
	 * @param url the url
	 * @param postData the post data
	 */
	void setUrl(const QString & url, const QString & postData);

	/**
	 * Return the anchor of an url
	 * @return the anchor of the url. An empty QString if the url contains no anchor
	 */
	QString getAnchor(QString url);
	
	/**
	 * find if an anchor is a callto one
	 * @param anchor the anchor
	 * @return true if anchor is a callto anchor
	 */
	bool isCallToAnchor(QString anchor);
	
	/**
	 * find if an anchor is a addcontact one
	 * @param anchor the anchor
	 * @return true if anchor is a addcontact anchor
	 */
	bool isAddContactAnchor(QString anchor);
	
	/**
	 * extract a pseudo or a number form a callto anchor
	 * @param anchor the anchor
	 * @return the pseudo / number or a null string if failse
	 */
	QString extractContactFromAnchor(QString anchor);
	
	/**
	 * Low-level widget of this class.
	 */
	QWidget * _searchPageWidget;
	
	/**
	 * The Internet Exporer ActiveX
	 */
	QAxWidget * _ieActiveX;
	
	static const QString ANCHOR_CALLCONTACT;
	
	static const QString ANCHOR_ADDCONTACT;
	
	static const QString ANCHOR_SELFCARE;

	static const QString ANCHOR_FORUM;
};

#endif	//SEARCHPAGEWIDGET_H
