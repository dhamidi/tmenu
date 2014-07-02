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
#include "menu.h"

#include "util.h"
#include "config.h"
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
   size_t  prompt_len;
   size_t  height;
   size_t  max_height;
   size_t  max_width;
   size_t  y_offset;
   char*   matchbuf;
   size_t  matchbuf_len;
   BUFFER  input;
   int     status_line_enabled;
};

static MENU menunew(void) {
   MENU menu = must_malloc(sizeof(*menu));

   menu->capacity = MENU_ITEM_MAX_SIZE / 2;
   menu->len      = 0;

   menu->items = must_malloc(sizeof(*menu->items) * menu->capacity);
   menu->matches  = must_malloc(sizeof(*menu->matches) * menu->capacity);
   menu->cursor = 0;
   menu->prompt = strdup(MENU_DEFAULT_PROMPT);
   menu->prompt_len = strlen(MENU_DEFAULT_PROMPT);
   menu->height  = MENU_DEFAULT_HEIGHT;

   menu->max_height = 0;
   menu->max_width = 0;

   menu->matchbuf = NULL;
   menu->matchbuf_len = 0;
   menu->input = TextBuffer.new(0);

   menu->status_line_enabled = MENU_DEFAULT_STATUS_LINE;

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

   TextBuffer.destroy( & (*self)->input );

   free( (*self)->items );
   free( (*self)->matches );
   free( (*self)->prompt );
   free( (*self)->matchbuf );
   free( *self );

   *self = NULL;
}

static int canprint(MENU self) {
   return self->max_height - self->y_offset - 1;
}

static void prepare_matches(MENU self) {
   memset(self->matches, 0, sizeof(*self->matches) * self->len);
   TextBuffer.string(self->input, &self->matchbuf, &self->matchbuf_len);
   self->curmatch = 0;
   self->cursor = 0;
}

static const char* menuselection(MENU self) {
   if (self->len > 0) {
      return self->items[self->matches[self->cursor]];
   } else {
      prepare_matches(self);
      return self->matchbuf;
   }
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

   self->prompt_len = newlen;
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

static void addmatch(MENU self, size_t index) {
   self->matches[self->curmatch++] = index;
}

static void menumatch(MENU self) {
   size_t i = 0;

   prepare_matches(self);

   for (i = 0; i < self->len; i++) {
      if ( strstr(self->items[i], self->matchbuf) != NULL ) {
         addmatch(self, i);
      }
   }
}

static BUFFER menubuffer(MENU self) {
   return self->input;
}

static void resetdisplay(MENU self, TERMINAL term) {
   size_t i;
   size_t n = 0;
   if (self->y_offset >= self->max_height) {
      n = self->max_height;
   } else {
      n = self->y_offset;
   };

   for (i = 0; i < n; i++) {
      Terminal.erase(term, 2);
      Terminal.up(term, 1);
   }
}

static void displayprompt(MENU self, TERMINAL term) {
   const char* lbuffer = TextBuffer.before(self->input);
   const char* inputpos = TextBuffer.after(self->input);
   size_t utf8bytes = 1;
   size_t linelen = TextBuffer.length(self->input) + self->prompt_len + 1;
   FILE* out = Terminal.file(term);

   resetdisplay(self, term);
   Terminal.erase(term, 2);      /* whole line */
   Terminal.col(term, 0);

   fprintf(out, "%s %s", self->prompt, lbuffer);

   Terminal.highlight(term, 1);
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
   Terminal.highlight(term, 0);

   fprintf(out, "%s\n", inputpos + utf8bytes);
   self->y_offset = 1 + linelen / self->max_width;
}

static void displaymatch(MENU self, TERMINAL term, size_t i, int selected) {
   FILE* out = Terminal.file(term);
   const char* text = self->items[self->matches[i]];
   size_t c;

   Terminal.erase(term, 2); Terminal.col(term, 0);

   if (i < self->curmatch) {
      if (selected) Terminal.highlight(term, 1);
      for (c = 1; text[c - 1] && canprint(self) > 1; c++) {
         fputc(text[c - 1], out);
         if ( (c+1) % self->max_width == 0) {
            fputc('$', out);
            break;
         }
      }
      if (selected) Terminal.highlight(term, 0);
      fputc('\n', out);
   } else {
      fprintf(out,"\n");
   }
}

static size_t height_for_matches(MENU self) {
   size_t offset = 1 /* prompt */
      + 1 /* trailing newline */
      + (self->status_line_enabled != 0);

   if (self->height > self->max_height - offset) {
      return self->max_height - offset;
   } else {
      return self->height;
   }
}

static void displaymatches(MENU self, TERMINAL term) {
   size_t height = height_for_matches(self);
   size_t page = self->cursor / height;
   size_t item = self->cursor % height;
   size_t i;

   for (i = 0; i < height && canprint(self) > 1; i++) {
      displaymatch(self, term, (page * height) + i, i == item );
      self->y_offset++;
   }
}

static void displayposition(MENU self, TERMINAL term) {
   FILE* out = Terminal.file(term);
   if (!canprint(self)) { return; }
   if (self->status_line_enabled) {
      Terminal.erase(term, 2); Terminal.col(term, 0);
      fprintf(out,"[%ld/%ld match(es)]\n",
              (long int)self->cursor + 1, (long int)self->curmatch);
      self->y_offset++;
   }
}

static void menudisplay(MENU self, TERMINAL term) {
   displayprompt(self, term);
   if (!canprint(self)) { return; }
   if (self->len > 0) {
      displaymatches(self, term);
      displayposition(self, term);
   }
}

static void menusetmaxwidth(MENU self, int width) {
   self->max_width = width;
}

static void menusetmaxheight(MENU self, int height) {
   self->max_height = height;
}

static void menuenablestatusline(MENU self, int enabled) {
   self->status_line_enabled = enabled;
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
   .enable_status_line = menuenablestatusline,
   .display = menudisplay,
   .match = menumatch,
   .buffer = menubuffer,
   .set_max_width = menusetmaxwidth,
   .set_max_height = menusetmaxheight,
};
