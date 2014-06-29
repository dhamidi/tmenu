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
#ifndef TMENU_TERMINAL_H
#define TMENU_TERMINAL_H

#include <stdio.h>

typedef struct terminal * TERMINAL;

enum terminal_error {
   TERMINAL_OK = 0,
   TERMINAL_GET_ATTR_FAIL,
   TERMINAL_SET_ATTR_FAIL,
   TERMINAL_UNKNOWN_MODE,
};
typedef enum terminal_error TERMINAL_ERROR;

extern struct terminal_interface {
   TERMINAL (*new)(const char*);
   void (*destroy)(TERMINAL*);

   TERMINAL_ERROR (*interactive_mode)(TERMINAL);
   TERMINAL_ERROR (*standard_mode)(TERMINAL);

   void (*left)(TERMINAL, int);
   void (*right)(TERMINAL, int);
   void (*up)(TERMINAL, int);
   void (*down)(TERMINAL, int);
   void (*erase)(TERMINAL, int);
   void (*col)(TERMINAL, int);
   void (*highlight)(TERMINAL,int);
   FILE* (*file)(TERMINAL);
   int  (*fd)(TERMINAL);
} Terminal;

#endif
