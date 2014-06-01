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
#ifndef TMENU_TERMINAL_H
#define TMENU_TERMINAL_H

#include <stdio.h>

extern struct terminal_interface {
   void (*left)(FILE*, int);
   void (*right)(FILE*, int);
   void (*up)(FILE*, int);
   void (*down)(FILE*, int);
   void (*erase)(FILE*, int);
   void (*col)(FILE*, int);
   void (*highlight)(FILE*,int);
} Terminal;

#endif
