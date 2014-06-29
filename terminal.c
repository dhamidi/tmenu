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
#include "terminal.h"

#include <termios.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "util.h"

struct terminal {
   FILE* file;
   int   fd;
};

static const char* dirs[] = {
   "\033[%dA",
   "\033[%dB",
   "\033[%dC",
   "\033[%dD",
};

enum TERMINAL_DIR {
   TERMINAL_DIR_UP,
   TERMINAL_DIR_DOWN,
   TERMINAL_DIR_RIGHT,
   TERMINAL_DIR_LEFT,
};

enum TERMINAL_MODE {
   TERMINAL_MODE_INTERACTIVE,
   TERMINAL_MODE_STANDARD,
};


static TERMINAL termnew(const char* filename) {
   TERMINAL term = must_malloc(sizeof(*term));
   int err = 0;

   term->file = fopen(filename, "r+");
   err = errno;
   if (term->file == NULL) {
      fprintf(stderr, "%s:open:%s: %s\n", PROGRAM_NAME, filename, strerror(err));
      abort();
   }

   term->fd = fileno(term->file);


   return term;
}

static TERMINAL_ERROR termsetmode(TERMINAL self, enum TERMINAL_MODE mode) {
   struct termios terminal_settings = { 0 };

   if ( tcgetattr(self->fd, &terminal_settings) == -1 ) {
      fprintf(stderr, "%s: failed to get terminal settings: %s\n",
              PROGRAM_NAME, strerror(errno));
      return TERMINAL_GET_ATTR_FAIL;
   }

   switch (mode) {
      case TERMINAL_MODE_INTERACTIVE:
         terminal_settings.c_lflag &= ~(ICANON|ECHO);
         break;
      case TERMINAL_MODE_STANDARD:
         terminal_settings.c_lflag |= ICANON|ECHO;
         break;
      default:
         fprintf(stderr, "%s: unknown terminal mode: %X\n",
                 PROGRAM_NAME, (int)mode);
         return TERMINAL_UNKNOWN_MODE;
   }

   if ( tcsetattr(self->fd, TCSANOW, &terminal_settings) == -1 ) {
      fprintf(stderr, "%s: failed to set terminal settings: %s\n",
              PROGRAM_NAME, strerror(errno));
      return TERMINAL_SET_ATTR_FAIL;
   }

   return TERMINAL_OK;
}

static TERMINAL_ERROR terminteractivemode(TERMINAL self) {
   return termsetmode(self, TERMINAL_MODE_INTERACTIVE);
}

static TERMINAL_ERROR termstandardmode(TERMINAL self) {
   return termsetmode(self, TERMINAL_MODE_STANDARD);
}

static void termdestroy(TERMINAL* self) {
   TERMINAL term = *self;
   termsetmode(term, TERMINAL_MODE_STANDARD);
   fclose(term->file);
   free(term);
   *self = NULL;
}

static void mov(TERMINAL self, enum TERMINAL_DIR dir, int arg) {
   fprintf(self->file, dirs[dir], arg);
}

static void up(TERMINAL self, int arg) {
   mov(self, TERMINAL_DIR_UP, arg);
}

static void down(TERMINAL self, int arg) {
   mov(self, TERMINAL_DIR_DOWN, arg);
}

static void right(TERMINAL self, int arg) {
   mov(self, TERMINAL_DIR_RIGHT, arg);
}

static void left(TERMINAL self, int arg) {
   mov(self, TERMINAL_DIR_LEFT, arg);
}

static void erase(TERMINAL self, int arg) {
   if (arg >= 2) {
      fprintf(self->file, "\033[%dK", 2);
   } else if (arg <= 0) {
      fprintf(self->file, "\033[%dK", 0);
   } else {
      fprintf(self->file, "\033[%dK", 1);
   }
}

static void col(TERMINAL self, int x) {
   fprintf(self->file, "\033[%dG", x);
}

static void highlight(TERMINAL self, int on) {
   fprintf(self->file, "\033[%dm", on ? 7 : 0);
}

static int termfd(TERMINAL self) {
   return self->fd;
}

static FILE* termfile(TERMINAL self) {
   return self->file;
}

struct terminal_interface Terminal = {
   .new = termnew,
   .destroy = termdestroy,

   .left = left,
   .right = right,
   .up = up,
   .down = down,
   .erase = erase,
   .col = col,
   .highlight = highlight,

   .file = termfile,
   .fd = termfd,

   .interactive_mode = terminteractivemode,
   .standard_mode = termstandardmode,
};
