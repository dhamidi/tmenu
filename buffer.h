#ifndef TMENU_BUFFER_H
#define TMENU_BUFFER_H

#include <stddef.h>

typedef struct buffer * BUFFER;

extern struct buffer_interface {
   BUFFER (*new)(size_t);
   void  (*destroy)(BUFFER*);

   void  (*putc)(BUFFER, int);
   void  (*puts)(BUFFER, const char*);
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
