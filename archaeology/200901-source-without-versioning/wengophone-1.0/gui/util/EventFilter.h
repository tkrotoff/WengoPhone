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

#ifndef EVENTFILTER_H
#define EVENTFILTER_H

#include <qobject.h>
#include <qsignal.h>

#include <iostream>

/**
 * EventFilter for QObject.
 *
 * Permits to make some special actions on Qt events.
 * Example:
 * <code>
 * QMainWindow * widget = new QMainWindow();
 * CloseEventFilter * closeFilter = new CloseEventFilter(this, SLOT(printHelloWorld()));
 * ResizeEventFilter * resizeFilter = new ResizeEventFilter(this, SLOT(printHelloWorld()));
 * widget->installEventFilter(closeFilter);
 * widget->installEventFilter(resizeFilter);
 * </code>
 *
 * @author Tanguy Krotoff
 */
class EventFilter : public QObject {
public:

	/**
	 * Filters an event.
	 *
	 * @param receiver object receiver of the filter signal
	 * @param member member of the object called by the filter signal
	 */
	EventFilter(QObject * receiver, const char * member)
	: QObject(receiver) {
		_signal = new QSignal();
		_signal->connect(receiver, member);
	}
	
	~EventFilter() {
		delete _signal;
	}

protected:

	/**
	 * Emits the filter signal.
	 */
	void filter() {
		_signal->activate();
	}

	/**
	 * Filters the event.
	 *
	 * @param object watched object
	 * @param event event filtered of the watched object
	 * @return true then stops the event being handled further
	 */
	virtual bool eventFilter(QObject * object, QEvent * event) {
		return QObject::eventFilter(object, event);
	}

private:

	EventFilter(const EventFilter &);
	EventFilter & operator=(const EventFilter &);

	/**
	 * Signal that permits to filter the event.
	 */
	QSignal * _signal;
};

/**
 * Catch ReturnKeyEvent.
 *
 * @author Tanguy Krotoff
 */
class ReturnKeyEventFilter : public EventFilter {
public:

	ReturnKeyEventFilter(QObject * receiver, const char * member)
	: EventFilter(receiver, member) {
	}

protected:

	virtual bool eventFilter(QObject * object, QEvent * event) {
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent * keyEvent = (QKeyEvent *) event;
			int key = keyEvent->key();

			if ((key == Key_Enter || key == Key_Return)
				&& (keyEvent->state() == NoButton || keyEvent->state() == Keypad)) {
				filter();
				return true;
			}
		}
		return EventFilter::eventFilter(object, event);
	}
};

/**
 * Catch DeleteKeyEvent.
 *
 * @author Tanguy Krotoff
 */
class DeleteKeyEventFilter : public EventFilter {
public:

	DeleteKeyEventFilter(QObject * receiver, const char * member)
	: EventFilter(receiver, member) {
	}

protected:

	virtual bool eventFilter(QObject * object, QEvent * event) {
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent * keyEvent = (QKeyEvent *) event;

			if (keyEvent->key() == Key_Delete) {
				filter();
				return true;
			}
		}
		return EventFilter::eventFilter(object, event);
	}
};

/**
 * Catch CloseEvent.
 *
 * @author Tanguy Krotoff
 */
class CloseEventFilter : public EventFilter {
public:

	CloseEventFilter(QObject * receiver, const char * member)
	: EventFilter(receiver, member) {
	}

protected:

	virtual bool eventFilter(QObject * object, QEvent * event) {
		if (event->type() == QEvent::Close) {
			filter();
		}
		return EventFilter::eventFilter(object, event);
	}
};

/**
 * Catch left mouse click event.
 *
 * @author Mathieu Stute
 */
class LeftMouseClickEventFilter : public EventFilter {
public:

	LeftMouseClickEventFilter(QObject * receiver, const char * member)
	: EventFilter(receiver, member) {
	}

protected:

	virtual bool eventFilter(QObject * object, QEvent * event) {
		if (event->type() == QEvent::MouseButtonPress) {
			QMouseEvent * mouseEvent = (QMouseEvent *) event;
			if( mouseEvent->button() == Qt::LeftButton ){
				filter();
				return true;
			}
		}
		return EventFilter::eventFilter(object, event);
	}
};

/**
 * Catch ResizeEvent.
 *
 * @author Mathieu Stute
 */
class ResizeEventFilter : public EventFilter {
public:

	ResizeEventFilter(QObject * receiver, const char * member)
	: EventFilter(receiver, member) {
	}

protected:

	virtual bool eventFilter(QObject * object, QEvent * event) {
		if (event->type() == QEvent::Resize) {
			filter();
		}
		return EventFilter::eventFilter(object, event);
	}
};

/**
 * Catch MousePressEvent.
 *
 * @author Tanguy Krotoff
 */
class MousePressEventFilter : public EventFilter {
public:

	MousePressEventFilter(QObject * receiver, const char * member)
	: EventFilter(receiver, member) {
	}

protected:

	virtual bool eventFilter(QObject * object, QEvent * event) {
		if (event->type() == QEvent::MouseButtonPress) {
			filter();
		}
		return EventFilter::eventFilter(object, event);
	}
};

/**
 * Catch MouseButtonDblClick.
 *
 * @author Tanguy Krotoff
 */
class MouseButtonDblClickEventFilter : public EventFilter {
public:

	MouseButtonDblClickEventFilter(QObject * receiver, const char * member)
	: EventFilter(receiver, member) {
	}

protected:

	virtual bool eventFilter(QObject * object, QEvent * event) {
		if (event->type() == QEvent::MouseButtonDblClick) {
			filter();
		}
		return EventFilter::eventFilter(object, event);
	}
};

/**
 * Catch WheelEvent.
 *
 * @author Tanguy Krotoff
 */
class WheelEventFilter : public EventFilter {
public:

	WheelEventFilter(QObject * receiver, const char * member)
	: EventFilter(receiver, member) {
	}

protected:

	virtual bool eventFilter(QObject * object, QEvent * event) {
		if (event->type() == QEvent::Wheel) {
			filter();
		}
		return EventFilter::eventFilter(object, event);
	}
};

/**
 * Catch KeyPressEvent.
 *
 * @author Tanguy Krotoff
 */
class KeyPressEventFilter : public EventFilter {
public:

	KeyPressEventFilter(QObject * receiver, const char * member)
	: EventFilter(receiver, member) {
	}

protected:

	virtual bool eventFilter(QObject * object, QEvent * event) {
		if (event->type() == QEvent::KeyPress) {
			filter();
		}
		return EventFilter::eventFilter(object, event);
	}
};

/**
 * Catch FKeyPressEvent.
 *
 * @author Tanguy Krotoff
 */
class FKeyPressEventFilter : public EventFilter {
public:

	FKeyPressEventFilter(QObject * receiver, const char * member)
	: EventFilter(receiver, member) {
	}

protected:

	virtual bool eventFilter(QObject * object, QEvent * event) {
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent * keyEvent = (QKeyEvent *) event;
			int key = keyEvent->key();

			if (key == Key_F) {
				filter();
			}
		}
		return EventFilter::eventFilter(object, event);
	}
};

/**
 * Catch F11KeyPressEvent.
 *
 * @author Tanguy Krotoff
 */
class F11KeyPressEvent : public EventFilter {
public:

	F11KeyPressEvent(QObject * receiver, const char * member)
	: EventFilter(receiver, member) {
	}

protected:

	virtual bool eventFilter(QObject * object, QEvent * event) {
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent * keyEvent = (QKeyEvent *) event;
			int key = keyEvent->key();

			if (key == Key_F11) {
				filter();
			}
		}
		return EventFilter::eventFilter(object, event);
	}
};

/**
 * Catch AltReturnKeyPressEvent.
 *
 * @author Tanguy Krotoff
 */
class AltReturnKeyPressEvent : public EventFilter {
public:

	AltReturnKeyPressEvent(QObject * receiver, const char * member)
	: EventFilter(receiver, member) {
	}

protected:

	virtual bool eventFilter(QObject * object, QEvent * event) {
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent * keyEvent = (QKeyEvent *) event;
			int key = keyEvent->key();

			if ((key == Key_Enter || key == Key_Return)
				&& (keyEvent->state() == AltButton)) {
				filter();
				return true;
			}
		}
		return EventFilter::eventFilter(object, event);
	}
};

/**
 * Catch EscapeKeyPressEvent.
 *
 * @author Tanguy Krotoff
 */
class EscapeKeyPressEvent : public EventFilter {
public:

	EscapeKeyPressEvent(QObject * receiver, const char * member)
	: EventFilter(receiver, member) {
	}

protected:

	virtual bool eventFilter(QObject * object, QEvent * event) {
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent * keyEvent = (QKeyEvent *) event;
			int key = keyEvent->key();

			if (key == Key_Escape) {
				filter();
				return true;
			}
		}
		return EventFilter::eventFilter(object, event);
	}
};

/**
 * Catch ShowEvent.
 *
 * @author Mathieu Stute
 */
class ShowEventFilter : public EventFilter {
public:

	ShowEventFilter(QObject * receiver, const char * member)
	: EventFilter(receiver, member) {
	}

protected:

	virtual bool eventFilter(QObject * object, QEvent * event) {
		if (event->type() == QEvent::Show) {
			filter();
		}
		return EventFilter::eventFilter(object, event);
	}
};

#endif	//EVENTFILTER_H
