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
#ifdef OS_WINDOWS
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
     * Default constructor
     *
     * @param parent the parent widget
     */
    QtBrowser(QWidget * parent = NULL, BrowserMode mode = IEMODE);

    /**
     * Set current url to browse
     *
     * @param url the url to browse
     */
    void setUrl(const std::string & url);
    
    /**
     * Set the brower mode to QTMODE or IEMODE
     *
     * @param mode BrowserMode
     * @return true if mode has been set
     */
    bool setMode(BrowserMode mode);
    
    /**
     * show the browser widget
     */
    void show();
    
    /**
     * return the low level widget,
     * mostely used to reparent the widget
     *
     * @return the low level widget
     */
    void * getWidget() const;

    /**
     * Set current url to browse
     *
     * @return the current url
     */
    std::string getUrl();

    /**
     * Browse backward
     */
    void backward();
    
    /**
     * Browse forward
     */
    void forward();

private Q_SLOTS:
    
    /**
     * Slot called before loading a page in a Microsoft Web Browser.
     */
    void BeforeNavigate(const QString&, int, const QString&, const QVariant&, const QString&, bool&);
    
    /**
     * Slot called before loading a page in a QTextBrowser.
     */
    void BeforeNavigate(const QUrl & link);

private:

    /**
     * current url
     */
    QString _url;

    /**
     * init browser (IE or QT)
     */
    void initBrowser();

    /**
     * BrowserMode: QT or IE
     */
    BrowserMode _mode;

    /**
     * Main widget
     */
    QWidget * _browserWidget;

#ifdef OS_WINDOWS
    /**
     * Microsoft Web Browser ActiveX
     */
    QAxWidget * _ieBrowser;
#endif

    /**
     * Qt html viewer
     */
    QTextBrowser * _qtBrowser;
    
    /**
     * Layout of the widget
     */
    QVBoxLayout * _layout;
};

#endif	//OWQTBROWSER_H
