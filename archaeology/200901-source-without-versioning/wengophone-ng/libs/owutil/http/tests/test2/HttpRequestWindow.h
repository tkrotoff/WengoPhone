/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWHTTPREQUESTWINDOW_H
#define OWHTTPREQUESTWINDOW_H

#include <http/HttpRequest.h>

#include <QtGui/QMainWindow>

namespace Ui { class HttpRequestWindow; }

class HttpRequestWindow : public QMainWindow {
	Q_OBJECT
public:

	HttpRequestWindow(QWidget * parent);

	~HttpRequestWindow();

private Q_SLOTS:

	void connectButtonClickedSlot();

	void answerReceivedSlot(int requestId, std::string answer, IHttpRequest::Error error);

private:

	HttpRequest * _httpRequest;

	Ui::HttpRequestWindow * _ui;
};

#endif	//OWHTTPREQUESTWINDOW_H
