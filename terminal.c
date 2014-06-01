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

static void mov(FILE* out, enum TERMINAL_DIR dir, int arg) {
   fprintf(out, dirs[dir], arg);
}

static void up(FILE* out, int arg) {
   mov(out, TERMINAL_DIR_UP, arg);
}

static void down(FILE* out, int arg) {
   mov(out, TERMINAL_DIR_DOWN, arg);
}

static void right(FILE* out, int arg) {
   mov(out, TERMINAL_DIR_RIGHT, arg);
}

static void left(FILE* out, int arg) {
   mov(out, TERMINAL_DIR_LEFT, arg);
}

static void erase(FILE* out, int arg) {
   if (arg >= 2) {
      fprintf(out, "\033[%dK", 2);
   } else if (arg <= 0) {
      fprintf(out, "\033[%dK", 0);
   } else {
      fprintf(out, "\033[%dK", 1);
   }
}

static void col(FILE* out, int x) {
   fprintf(out, "\033[%dG", x);
}

static void highlight(FILE* out, int on) {
   fprintf(out, "\033[%dm", on ? 7 : 0);
}

struct terminal_interface Terminal = {
   .left = left,
   .right = right,
   .up = up,
   .down = down,
   .erase = erase,
   .col = col,
   .highlight = highlight,
};
