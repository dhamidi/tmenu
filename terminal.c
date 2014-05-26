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
