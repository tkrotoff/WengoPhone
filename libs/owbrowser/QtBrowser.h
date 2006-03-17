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

#ifndef OWQTBROWSER_H
#define OWQTBROWSER_H

#include <OWBrowser.h>

#include <global.h>
#include <Event.h>

#include <QObject>

class QWidget;
class QTextBrowser;
class QVBoxLayout;
class QUrl;
#if defined (OS_WINDOWS) && !defined (QT_EDITION_OPENSOURCE)
class QAxWidget;
#endif

/**
 * Qt implementation of OWBrowser.
 *
 * Implemented with:
 *  - QTextBrowser: local url
 *  - Microsoft Web Browser: local & remote url
 *
 * @author Mathieu Stute
 */
class QtBrowser : public QObject, public OWBrowser {
	Q_OBJECT
public:

	/**
	* Browser mode enum
	*/
	enum BrowserMode {
		QTMODE,
		IEMODE
	};

	/**
	 * Default constructor.
	 *
	 * @param parent the parent widget
	 * @param mode browser mode to use (IE or Qt)
	 */
	QtBrowser(QWidget * parent, BrowserMode mode = IEMODE);

	void setUrl(const std::string & url);

	bool setMode(BrowserMode mode);

	void show();

	void * getWidget() const;

	std::string getUrl() const;

	void backward();

	void forward();

private Q_SLOTS:

	/**
	 * Slot called before loading a page in a Microsoft Web Browser.
	 */
	void beforeNavigate(const QString &, int, const QString &, const QVariant &, const QString &, bool &);

	/**
	 * Slot called before loading a page in a QTextBrowser.
	 */
	void beforeNavigate(const QUrl & link);

private:

	/**
	 * Current url.
	 */
	QString _url;

	/**
	 * Inits the browser (IE or Qt).
	 */
	void initBrowser();

	/**
	 * BrowserMode: Qt or IE.
	 */
	BrowserMode _mode;

	/**
	 * Main widget.
	 */
	QWidget * _browserWidget;

#if defined (OS_WINDOWS) && !defined (QT_EDITION_OPENSOURCE)
	/**
	 * Microsoft Web Browser ActiveX.
	 */
	QAxWidget * _ieBrowser;
#endif

	/**
	 * Qt html viewer.
	 */
	QTextBrowser * _qtBrowser;

	/**
	 * Widget layout.
	 */
	QVBoxLayout * _layout;
};

#endif	//OWQTBROWSER_H
