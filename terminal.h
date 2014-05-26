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
} Terminal;

#endif
