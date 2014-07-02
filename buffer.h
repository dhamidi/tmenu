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
#ifndef TMENU_BUFFER_H
#define TMENU_BUFFER_H

#include <stddef.h>

typedef struct buffer * BUFFER;

extern struct buffer_interface {
   BUFFER (*new)(size_t);
   void  (*destroy)(BUFFER*);
   size_t (*length)(BUFFER);
   void  (*cput)(BUFFER, int);
   void  (*sput)(BUFFER, const char*);
   char* (*string)(BUFFER, char**, size_t*);
   void  (*forward)(BUFFER, int);
   void  (*backward)(BUFFER, int);
   const char* (*before)(BUFFER);
   const char* (*after)(BUFFER);
   size_t   (*point)(BUFFER);
   void  (*delete)(BUFFER);
   void  (*clear)(BUFFER);
   void  (*delete_to_end)(BUFFER);
   void  (*delete_to_beginning)(BUFFER);
} Buffer;

#endif
