#define _POSIX_C_SOURCE 200809L
#include "menu.h"

#include "util.h"
#include "config.h"
#include "terminal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct menu {
   char**  items;
   size_t  capacity;
   size_t  len;
   size_t* matches;
   size_t  curmatch;
   size_t  cursor;
   char*   prompt;
   size_t  height;
   size_t  y_offset;
   char*   matchbuf;
   size_t  matchbuf_len;
   BUFFER  input;
};

static MENU menunew(void) {
   MENU menu = must_malloc(sizeof(*menu));

   menu->capacity = MENU_ITEM_MAX_SIZE / 2;
   menu->len      = 0;

   menu->items = must_malloc(sizeof(*menu->items) * menu->capacity);
   menu->matches  = must_malloc(sizeof(*menu->matches) * menu->capacity);
   menu->cursor = 0;
   menu->prompt = strdup(MENU_DEFAULT_PROMPT);

   menu->matchbuf = NULL;
   menu->matchbuf_len = 0;
   menu->input = Buffer.new(0);

   return menu;
}

static void menudestroy(MENU* self) {
   size_t i;

   if (self == NULL) {
      return;
   }

   for (i = 0; i < (*self)->len; i++) {
      free( (*self)->items[i] );
   }

   Buffer.destroy( & (*self)->input );

   free( (*self)->items );
   free( (*self)->matches );
   free( (*self)->prompt );
   free( (*self)->matchbuf );
   free( *self );

   *self = NULL;
}

static const char* menuselection(MENU self) {
   return self->items[self->matches[self->cursor]];
}

static void menusetprompt(MENU self, const char* prompt) {
   size_t newlen = strlen(prompt);
   size_t oldlen = strlen(self->prompt);

   if (oldlen >= newlen) {
      memset(self->prompt, 0, oldlen);
      strncpy(self->prompt, prompt, newlen + 1); /* trailing 0 byte */
   } else {
      free(self->prompt);
      self->prompt = strdup(prompt);
   }
}

static void menusetheight(MENU self, int height) {
   if (height < 0) {
      height = height * -1;
   }

   self->height = height;
}

static int menugrow(MENU self) {
   size_t new_capacity = self->capacity + MENU_GROW_BY;
   size_t i;
   char** new_items = NULL;
   size_t* new_matches = NULL;

   if (self->capacity >= MENU_MAX_ITEMS) {
      fprintf(stderr, "%s: cannot hold more than %d items.\n", PROGRAM_NAME, MENU_MAX_ITEMS);
      return 0;
   }

   new_items = must_malloc(sizeof(*new_items) * new_capacity);
   new_matches = must_malloc(sizeof(*new_matches) * new_capacity);

   for (i = 0; i < self->len; i++) {
      new_items[i] = self->items[i];
      new_matches[i] = self->matches[i];
   }

   free( self->items );
   free( self->matches );

   self->items = new_items;
   self->matches = new_matches;

   return 1;
}

static void menuadditem(MENU self, const char* item) {
   if (self->len == self->capacity) {
      if (!menugrow(self)) {
         fprintf(stderr, "%s: ignoring item %s\n", PROGRAM_NAME, item);
         return;
      }
   }

   self->items[self->len++] = strdup(item);
}

static void menuselectnext(MENU self) {
   if (self->cursor < self->curmatch - 1) {
      self->cursor++;
   }
}

static void menuselectprev(MENU self) {
   if (self->cursor > 0) {
      self->cursor--;
   }
}

static void prepare_matches(MENU self) {
   memset(self->matches, 0, sizeof(*self->matches) * self->len);
   self->curmatch = 0;
}

static void addmatch(MENU self, size_t index) {
   self->matches[self->curmatch++] = index;
}

static void menumatch(MENU self) {
   size_t i = 0;

   prepare_matches(self);

   Buffer.string(self->input, &self->matchbuf, &self->matchbuf_len);
   for (i = 0; i < self->len; i++) {
      if ( strstr(self->items[i], self->matchbuf) != NULL ) {
         addmatch(self, i);
      }
   }
}

static BUFFER menubuffer(MENU self) {
   return self->input;
}

static void displayprompt(MENU self, FILE* out) {
   const char* inputpos = Buffer.after(self->input);
   size_t utf8bytes = 1;

   Terminal.up(out, self->y_offset);
   Terminal.erase(out, 2);      /* whole line */
   Terminal.col(out, 0);

   fprintf(out, "%s %s", self->prompt, Buffer.before(self->input));

   Terminal.highlight(out, 1);
   if ( inputpos[0] ) {
      fprintf(out, "%c", inputpos[0]);
      if ( (unsigned char)inputpos[0] > 0x70 ) {
         // UTF-8
         while ( (unsigned char)inputpos[utf8bytes] >> 6 == 2) {
            fprintf(out, "%c", inputpos[utf8bytes]);
            utf8bytes++;
         }
      }
   } else {
      fprintf(out, " ");
   }
   Terminal.highlight(out, 0);

   fprintf(out, "%s\n", inputpos + utf8bytes);

   self->y_offset = 1;
}

static void displaymatch(MENU self, FILE* out, size_t i) {
   int selected = i % self->height == self->cursor % self->height;

   Terminal.erase(out, 2); Terminal.col(out, 0);

   if (i < self->curmatch) {
      if (selected) Terminal.highlight(out, 1);
      fprintf(out,"%s\n", self->items[self->matches[i]]);
      if (selected) Terminal.highlight(out, 0);
   } else {
      fprintf(out,"\n");
   }
}

static void displaymatches(MENU self, FILE* out) {
   size_t i, j;
   for (i = self->cursor / self->height, j = 0; j < self->height; j++) {
      displaymatch(self, out, i + j);
      self->y_offset++;
   }
}

static void displayposition(MENU self, FILE* out) {
   Terminal.erase(out, 2); Terminal.col(out, 0);
   fprintf(out,"[%ld/%ld match(es)]\n",
           (long int)self->cursor + 1, (long int)self->curmatch);
   self->y_offset++;
}

static void menudisplay(MENU self, FILE* out) {
   displayprompt(self, out);
   displaymatches(self, out);
   displayposition(self, out);
}

struct menu_interface Menu = {
   .new = menunew,
   .destroy = menudestroy,
   .set_prompt = menusetprompt,
   .set_height = menusetheight,
   .add_item = menuadditem,
   .select_next = menuselectnext,
   .select_prev = menuselectprev,
   .selection = menuselection,
   .display = menudisplay,
   .match = menumatch,
   .buffer = menubuffer,
};
