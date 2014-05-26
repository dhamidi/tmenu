#ifndef TMENU_MENU_H
#define TMENU_MENU_H

#include <stdio.h>
#include "buffer.h"

typedef struct menu * MENU;

extern struct menu_interface {
   MENU    (*new)(void);
   void    (*destroy)(MENU*);

   void    (*set_prompt)(MENU, const char*);
   void    (*set_height)(MENU, int);
   void    (*add_item)(MENU, const char*);
   void    (*select_next)(MENU);
   void    (*select_prev)(MENU);
   void    (*display)(MENU, FILE*);
   void    (*match)(MENU);
   BUFFER  (*buffer)(MENU);
} Menu;

#endif
