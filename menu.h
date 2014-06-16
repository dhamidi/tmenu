/*
Copyright 2014 Dario Hamidi

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#define _POSIX_C_SOURCE 200809L
#ifndef TMENU_MENU_H
#define TMENU_MENU_H

#include <stdio.h>
#include "textbuffer.h"

typedef struct menu * MENU;

extern struct menu_interface {
   MENU    (*new)(void);
   void    (*destroy)(MENU*);

   void    (*set_prompt)(MENU, const char*);
   void    (*set_height)(MENU, int);
   void    (*enable_status_line)(MENU, int);
   void    (*add_item)(MENU, const char*);
   void    (*select_next)(MENU);
   void    (*select_prev)(MENU);
   const char* (*selection)(MENU);
   void    (*display)(MENU, FILE*);
   void    (*match)(MENU);
   BUFFER  (*buffer)(MENU);
} Menu;

#endif
