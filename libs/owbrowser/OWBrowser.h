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

#ifndef OWBROWSER_H
#define OWBROWSER_H

#include <Event.h>
#include <Interface.h>

#include <string>

/**
 * class OWBrowser, a minimalist embedded browser.
 *
 * @author Mathieu Stute
 */
class OWBrowser : Interface {
    
public:

    /**
     * A link has been clicked
     *
     * @param sender sender of the event
     * @param image captured image
     */
    Event<void (std::string url)> urlClickedEvent;

    /**
     * Set current url to browse
     *
     * @param url the url to browse
     */
    virtual void setUrl(const std::string & url) = 0;
    
    /**
     * Set current url to browse
     *
     * @return  the current url
     */
    virtual std::string getUrl() = 0;
    
    /**
     * return the low level widget
     *
     * @return the low level widget
     */
    virtual void * getWidget() const = 0;
    
    /**
     * show the widget
     */
    virtual void show() = 0;
    
    /**
     * Browse backward
     */
    virtual void backward() = 0;
    
    /**
     * Browse forward
     */
    virtual void forward() = 0;
};

#endif	//OWBROWSER_H
