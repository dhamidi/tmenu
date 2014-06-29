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
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <termios.h>

#include "config.h"
#include "terminal.h"
#include "buffer.h"
#include "menu.h"

enum key {
   KEY_CTRL_A = 0x1,
   KEY_CTRL_B = 0x2,
   KEY_CTRL_C = 0x3,
   KEY_CTRL_D = 0x4,
   KEY_CTRL_E = 0x5,
   KEY_CTRL_F = 0x6,
   KEY_CTRL_G = 0x7,
   KEY_CTRL_H = 0x8,
   KEY_CTRL_I = 0x9,
   KEY_CTRL_J = 0xa,
   KEY_CTRL_K = 0xb,
   KEY_CTRL_L = 0xc,
   KEY_CTRL_M = 0xd,
   KEY_CTRL_N = 0xe,
   KEY_CTRL_O = 0xf,
   KEY_CTRL_P = 0x10,
   KEY_CTRL_Q = 0x11,
   KEY_CTRL_R = 0x12,
   KEY_CTRL_S = 0x13,
   KEY_CTRL_T = 0x14,
   KEY_CTRL_U = 0x15,
   KEY_CTRL_V = 0x16,
   KEY_CTRL_X = 0x17,
   KEY_CTRL_Y = 0x18,
   KEY_CTRL_Z = 0x19,
   KEY_SPACE  = 0x20,
   KEY_ENTER  = 0xa,
   KEY_BACKSPACE = 0x7f,
};

typedef int (*action_fn)(MENU);

static int action_select_next(MENU menu) {
   Menu.select_next(menu);
   return 1;
}

static int action_select_prev(MENU menu) {
   Menu.select_prev(menu);
   return 1;
}

static int action_forward(MENU menu) {
   TextBuffer.forward(Menu.buffer(menu), 1);
   return 1;
}

static int action_backward(MENU menu) {
   TextBuffer.backward(Menu.buffer(menu), 1);
   return 1;
}

static int action_delete_char(MENU menu) {
   TextBuffer.delete(Menu.buffer(menu));
   Menu.match(menu);
   return 1;
}

static int action_delete_char_before(MENU menu) {
   BUFFER buf = Menu.buffer(menu);
   size_t point_before = TextBuffer.point(buf);
   size_t point_after = point_before;

   action_backward(menu);

   point_after = TextBuffer.point(buf);

   while ( point_after++ < point_before )
      TextBuffer.delete(Menu.buffer(menu));

   Menu.match(menu);
   return 1;
}

static int action_delete_to_beginning(MENU menu) {
   TextBuffer.delete_to_beginning(Menu.buffer(menu));
   Menu.match(menu);
   return 1;
}

static int action_delete_to_end(MENU menu) {
   TextBuffer.delete_to_end(Menu.buffer(menu));
   Menu.match(menu);
   return 1;
}

static int action_beginning_of_line(MENU menu) {
   TextBuffer.backward(Menu.buffer(menu), 10000);
   return 1;
}

static int action_end_of_line(MENU menu) {
   TextBuffer.forward(Menu.buffer(menu), 10000);
   return 1;
}

static int action_confirm(MENU menu) {
   fprintf(stdout, "%s\n", Menu.selection(menu));
   return 0;
}

static action_fn KEYMAP[256] = {
   [KEY_CTRL_P] = action_select_prev,
   [KEY_CTRL_N] = action_select_next,
   [KEY_CTRL_A] = action_beginning_of_line,
   [KEY_CTRL_E] = action_end_of_line,
   [KEY_CTRL_F] = action_forward,
   [KEY_CTRL_B] = action_backward,
   [KEY_CTRL_U] = action_delete_to_beginning,
   [KEY_CTRL_K] = action_delete_to_end,
   [KEY_CTRL_D] = action_delete_char,
   [KEY_ENTER]  = action_confirm,
   [KEY_BACKSPACE] = action_delete_char_before,
};

static int getitem(char* buf, size_t len, FILE* in) {
   size_t i = 0;
   int c;

   memset(buf, 0, len);

   for ( c = fgetc(in); c != EOF; c = fgetc(in) ) {
      if (c == '\n') { break; }

      if ( i < len - 1 ) {
         buf[i++] = c;
      }
   }

   return c != EOF;
}

int main(int argc, char** argv) {
   char item[MENU_ITEM_MAX_SIZE] = { 0 };
   char buf[8] = { 0 };
   action_fn action = NULL;
   int opt, fd_in;

   TERMINAL term = Terminal.new("/dev/tty");
   MENU menu = Menu.new();

   fd_in = Terminal.fd(term);

   while ( (opt = getopt(argc, argv, "p:l:")) != -1 ) {
      switch (opt) {
         case 'p':
            Menu.set_prompt(menu, optarg);
            break;
         case 'l':
            Menu.set_height(menu, atoi(optarg));
            break;
         default:
            exit(EXIT_FAILURE);
      }
   }

   while ( getitem(item, MENU_ITEM_MAX_SIZE, stdin) ) {
      Menu.add_item(menu, item);
   }

   if (Terminal.interactive_mode(term) != TERMINAL_OK) {
      goto exit;
   }

   Menu.match(menu);

   /* avoid clobbering the user's prompt */
   fputs("\n", Terminal.file(term));

   Menu.display(menu, term);

   while ( read(fd_in, &buf, 8) != 0 ) {
      buf[7] = 0;
      action = KEYMAP[ (unsigned char)buf[0] ];

      if (!action && (unsigned char)buf[0] >= KEY_SPACE) {
         TextBuffer.sput(Menu.buffer(menu), buf);
         Menu.match(menu);
      } else {
         if (action && ! action(menu) ) {
            goto exit;
         }
      }

      Menu.display(menu, term);
      memset(&buf, 0, 8);
   }

  exit:
   Terminal.destroy(&term);
   Menu.destroy(&menu);

   return EXIT_SUCCESS;
}
