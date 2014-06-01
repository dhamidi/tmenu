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

static const char* example_items[] = {
   "hello world",
   "foo bar",
   "this is a test",
   "let's see",
   "underworld",
   "overworld",
   NULL
};

struct display {
   FILE* file;
   int   fd;
};

void setup_display(struct display * ctx) {
   struct termios terminal_settings = { 0 };

   ctx->file = fopen("/dev/tty", "r+");
   if (ctx->file == NULL) {
      perror(PROGRAM_NAME ":open:/dev/tty");
      abort();
   }

   ctx->fd = fileno(ctx->file);

   if ( tcgetattr(ctx->fd, &terminal_settings) == -1 ) {
      fprintf(stderr, "%s: failed to get terminal settings: %s\n",
              PROGRAM_NAME, strerror(errno));
      abort();
   }

   terminal_settings.c_lflag &= ~(ICANON|ECHO);

   if ( tcsetattr(ctx->fd, TCSANOW, &terminal_settings) == -1 ) {
      fprintf(stderr, "%s: failed to set terminal settings: %s\n",
              PROGRAM_NAME, strerror(errno));
      abort();
   }
}

void teardown_display(struct display * ctx) {
   struct termios terminal_settings = { 0 };

   if ( tcgetattr(ctx->fd, &terminal_settings) == -1 ) {
      fprintf(stderr, "%s: failed to get terminal settings: %s\n",
              PROGRAM_NAME, strerror(errno));
      abort();
   }

   terminal_settings.c_lflag |= ICANON|ECHO;

   if ( tcsetattr(ctx->fd, TCSANOW, &terminal_settings) == -1 ) {
      fprintf(stderr, "%s: failed to set terminal settings: %s\n",
              PROGRAM_NAME, strerror(errno));
      abort();
   }

   fclose(ctx->file);
}

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
   BUFFER buf = Menu.buffer(menu);
   const char* after = Buffer.after(buf);

   Buffer.forward(buf, 1);
   while ( (unsigned char)*(++after) >> 6 == 2)
      Buffer.forward(buf, 1);

   return 1;
}

static int action_backward(MENU menu) {
   BUFFER buf = Menu.buffer(menu);
   const char* before = Buffer.before(buf);
   size_t before_len = Buffer.point(buf) - 1;
   size_t pos = before_len - 1;

   if ( (unsigned char)before[pos] <= 0x70) {
      // ASCII
      Buffer.backward(buf, 1);
   } else {
      // UTF-8
      while ( (unsigned char)before[pos] >> 6 == 2) {
         pos--;
      }
      Buffer.backward(buf, before_len - pos);
   }


   return 1;
}

static int action_delete_char(MENU menu) {
   BUFFER buf = Menu.buffer(menu);
   const char* after = Buffer.after(buf);

   Buffer.delete(buf);
   while ( (unsigned char)*(++after) >> 6 == 2 )
      Buffer.delete(buf);

   Menu.match(menu);
   return 1;
}

static int action_delete_char_before(MENU menu) {
   BUFFER buf = Menu.buffer(menu);
   size_t point_before = Buffer.point(buf);
   size_t point_after = point_before;

   action_backward(menu);

   point_after = Buffer.point(buf);

   while ( point_after++ < point_before )
      Buffer.delete(Menu.buffer(menu));

   Menu.match(menu);
   return 1;
}

static int action_delete_to_beginning(MENU menu) {
   Buffer.delete_to_beginning(Menu.buffer(menu));
   Menu.match(menu);
   return 1;
}

static int action_delete_to_end(MENU menu) {
   Buffer.delete_to_end(Menu.buffer(menu));
   Menu.match(menu);
   return 1;
}

static int action_beginning_of_line(MENU menu) {
   Buffer.backward(Menu.buffer(menu), 10000);
   return 1;
}

static int action_end_of_line(MENU menu) {
   Buffer.forward(Menu.buffer(menu), 10000);
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

int main(int argc, char** argv) {
   const char** item = NULL;
   char buf[4] = { 0 };
   MENU menu = Menu.new();
   action_fn action = NULL;
   struct display display;


   Menu.set_prompt(menu, ">>");
   Menu.set_height(menu, 3);

   for (item = example_items; *item; item++) {
      Menu.add_item(menu, *item);
   }

   Menu.match(menu);

   setup_display(&display);

   Menu.display(menu, display.file);

   while ( read(display.fd, &buf, 8) != 0 ) {
      buf[7] = 0;
      action = KEYMAP[ (unsigned char)buf[0] ];

      if (!action && (unsigned char)buf[0] >= KEY_SPACE) {
         Buffer.sput(Menu.buffer(menu), buf);
         Menu.match(menu);
      } else {
         if (action && ! action(menu) ) {
            goto exit;
         }
      }

      Menu.display(menu, display.file);
      memset(&buf, 0, 8);
   }

  exit:
   teardown_display(&display);
   Menu.destroy(&menu);

   return EXIT_SUCCESS;
}
