#ifndef TMENU_MENU_H
#define TMENU_MENU_H

typedef struct MENU MENU;

extern struct menu_interface {
   void    (*set_prompt)(MENU*, const char*);
   void    (*add_item)(MENU*, const char*);
   void    (*display)(MENU*, FILE*);
   void    (*match)(MENU*, const char*);
   BUFFER* (*buffer)(MENU*);
} Menu;

#endif
