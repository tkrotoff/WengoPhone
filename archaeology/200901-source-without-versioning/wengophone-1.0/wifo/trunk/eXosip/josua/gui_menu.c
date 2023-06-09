/*
 * josua - Jack's open sip User Agent
 *
 * Copyright (C) 2002,2003   Aymeric Moizard <jack@atosc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with dpkg; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include "gui_menu.h"
#include "gui_new_call.h"
#include "gui_address_book_browse.h"
#include "gui_sessions_list.h"
#include "gui_registrations_list.h"
#include "gui_subscriptions_list.h"
#include "gui_online.h"
#include "gui_setup.h"

extern struct osip_mutex *log_mutex;

gui_t gui_window_menu = {
  GUI_OFF,
  20,
  -999,
  2,
  9,
  NULL,
  &window_menu_print,
  &window_menu_run_command,
  NULL,
  &window_menu_draw_commands,
  -1,
  -1,
  -1,
  NULL
};

static const menu_t josua_menu[]= {
  { "a", " ADDRESS BOOK       -    Update address book",
    &__show_address_book_browse  },
  { "i", " INITIATE SESSION   -    Initiate a session",
    &__show_initiate_session },
  { "u", " SUBSCRIPTIONS LIST -    View pending subscriptions",
    &__show_subscriptions_list },
  { "l", " SESSIONS LIST      -    View pending sessions",
    &__show_sessions_list },
  { "r", " REGISTRATIONS LIST -    View pending registrations",
    &__show_registrations_list  },
  { "s", " SETUP              -    Configure Josua options",
    &__show_setup },
  { "q", " QUIT               -    Quit the Josua program",
    &__josua_quit  },
  { 0 }
};

static int cursor_menu = 0;

int window_menu_print()
{
  int y,x, x1;
  char buf[250];
  int i;
  int pos;
  curseson(); cbreak(); noecho(); nonl(); keypad(stdscr,TRUE);

  getmaxyx(stdscr,y,x);
  if (gui_window_menu.x1<=0)
    x1 = x;
  else x1 = gui_window_menu.x1;

  pos = 0;
  for (i=gui_window_menu.y0; i<gui_window_menu.y1; i++)
    {
      snprintf(buf, x1 - gui_window_menu.x0,
	      "%c%c [%s] %s ",
	      (cursor_menu==pos) ? '-' : ' ',
	      (cursor_menu==pos) ? '>' : ' ',
	      josua_menu[i-gui_window_menu.y0].key,
	      josua_menu[i-gui_window_menu.y0].text);

      attrset(COLOR_PAIR(5));
      attrset((pos==cursor_menu) ? A_REVERSE : A_NORMAL);
      mvaddnstr(i,
		gui_window_menu.x0,
		buf,
		x-gui_window_menu.x0-1);
      pos++;
    }
  return 0;
}

void window_menu_draw_commands()
{
  int x,y;
  char *menu_commands[] = {
    "<-",  "PrevWindow",
    "->",  "NextWindow",
    "^i",  "Toggle Online status",
    "^a",  "Apply Status" ,
    NULL
  };
  getmaxyx(stdscr,y,x);
  josua_print_command(menu_commands, y-5, 0);
}

int window_menu_run_command(int c)
{
  int max = 7;
  switch (c)
    {
    case 9:
      josua_online_status++;
      if (josua_online_status>EXOSIP_NOTIFY_CLOSED)
	josua_online_status = EXOSIP_NOTIFY_ONLINE;
      break;
    case 1:
      /* apply IM change status */
      {
	int k;
	for (k=0;k<MAX_NUMBER_OF_INSUBSCRIPTIONS;k++)
	  {
	    if (jinsubscriptions[k].state != NOT_USED)
	      {
		int i;
		eXosip_lock();
		i = eXosip_notify(jinsubscriptions[k].did, EXOSIP_SUBCRSTATE_ACTIVE, josua_online_status);
		if (i!=0) beep();
		eXosip_unlock();
	      }
	  }
      }
      break;
    case KEY_DOWN:
      cursor_menu++;
      cursor_menu %= max;
      break;
    case KEY_UP:
      cursor_menu += max-1;
      cursor_menu %= max;
      break;
    case 'a':
      cursor_menu = 0;
      break;
    case 'i':
      cursor_menu = 1;
      break;
    case 'u':
      cursor_menu = 2;
      break;
    case 'l':
      cursor_menu = 3;
      break;
    case 'r':
      cursor_menu = 4;
      break;
    case 's':
      cursor_menu = 5;
      break;
    case 'q':
      cursor_menu = 6;
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
      cursor_menu = c-48;
      break;
    case '\n':
    case '\r':
    case KEY_ENTER:
      /* menu selected! */
      josua_menu[cursor_menu].fn();
      break;

    default:
      beep();
      return -1;
    }

  if (gui_window_menu.on_off==GUI_ON)
    window_menu_print();
  return 0;
}

void __show_address_book_browse()
{
  active_gui->on_off = GUI_OFF;
  if (gui_windows[EXTRAGUI]==NULL)
    gui_windows[EXTRAGUI]= &gui_window_address_book_browse;
  else
    {
      gui_windows[EXTRAGUI]->on_off = GUI_OFF;
      josua_clear_box_and_commands(gui_windows[EXTRAGUI]);
      gui_windows[EXTRAGUI]= &gui_window_address_book_browse;
    }

  active_gui = gui_windows[EXTRAGUI];
  active_gui->on_off = GUI_ON;

  window_address_book_browse_print();
}
#if 0
  active_gui->on_off = GUI_OFF;
  if (gui_windows[MENUGUI]==NULL)
    gui_windows[MENUGUI]= &gui_window_address_book_menu;
  else
    {
      josua_clear_box_and_commands(gui_windows[MENUGUI]);
      gui_windows[MENUGUI]->on_off = GUI_OFF;
      gui_windows[MENUGUI]= &gui_window_address_book_menu;
    }

  if (gui_windows[EXTRAGUI]!=NULL)
    {
      josua_clear_box_and_commands(gui_windows[EXTRAGUI]);
      gui_windows[EXTRAGUI] = NULL;
    }

  active_gui = gui_windows[MENUGUI];
  active_gui->on_off = GUI_ON;

  window_address_book_menu_print();
}
#endif


void
__show_initiate_session()
{
  active_gui->on_off = GUI_OFF;
  if (gui_windows[EXTRAGUI]==NULL)
    gui_windows[EXTRAGUI]= &gui_window_new_call;
  else
    {
      gui_windows[EXTRAGUI]->on_off = GUI_OFF;
      josua_clear_box_and_commands(gui_windows[EXTRAGUI]);
      gui_windows[EXTRAGUI]= &gui_window_new_call;
    }

  active_gui = gui_windows[EXTRAGUI];
  active_gui->on_off = GUI_ON;

  window_new_call_print();
}

void
__show_sessions_list()
{
  active_gui->on_off = GUI_OFF;
  if (gui_windows[EXTRAGUI]==NULL)
    gui_windows[EXTRAGUI]= &gui_window_sessions_list;
  else
    {
      gui_windows[EXTRAGUI]->on_off = GUI_OFF;
      josua_clear_box_and_commands(gui_windows[EXTRAGUI]);
      gui_windows[EXTRAGUI]= &gui_window_sessions_list;
    }

  active_gui = gui_windows[EXTRAGUI];
  active_gui->on_off = GUI_ON;

  window_sessions_list_print();
}

void __show_subscriptions_list()
{
  active_gui->on_off = GUI_OFF;
  if (gui_windows[EXTRAGUI]==NULL)
    gui_windows[EXTRAGUI]= &gui_window_subscriptions_list;
  else
    {
      gui_windows[EXTRAGUI]->on_off = GUI_OFF;
      josua_clear_box_and_commands(gui_windows[EXTRAGUI]);
      gui_windows[EXTRAGUI]= &gui_window_subscriptions_list;
    }

  active_gui = gui_windows[EXTRAGUI];
  active_gui->on_off = GUI_ON;

  window_subscriptions_list_print();
}

void __show_registrations_list()
{
  active_gui->on_off = GUI_OFF;
  if (gui_windows[EXTRAGUI]==NULL)
    gui_windows[EXTRAGUI]= &gui_window_registrations_list;
  else
    {
      gui_windows[EXTRAGUI]->on_off = GUI_OFF;
      josua_clear_box_and_commands(gui_windows[EXTRAGUI]);
      gui_windows[EXTRAGUI]= &gui_window_registrations_list;
    }

  active_gui = gui_windows[EXTRAGUI];
  active_gui->on_off = GUI_ON;

  window_registrations_list_print();
}

void __show_setup()
{
  active_gui->on_off = GUI_OFF;
  if (gui_windows[EXTRAGUI]==NULL)
    gui_windows[EXTRAGUI]= &gui_window_setup;
  else
    {
      gui_windows[EXTRAGUI]->on_off = GUI_OFF;
      josua_clear_box_and_commands(gui_windows[EXTRAGUI]);
      gui_windows[EXTRAGUI]= &gui_window_setup;
    }

  active_gui = gui_windows[EXTRAGUI];
  active_gui->on_off = GUI_ON;

  window_setup_print();
}

void __josua_quit() {

  eXosip_quit();
  osip_mutex_destroy(log_mutex);

  cursesoff();  
  exit(1);
}

