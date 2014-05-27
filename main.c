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
   "спасибо",
   "дробрый день",
   "да",
   NULL
};

void setup(void) {
   struct termios terminal_settings = { 0 };

   if ( tcgetattr(STDIN_FILENO, &terminal_settings) == -1 ) {
      fprintf(stderr, "%s: failed to get terminal settings: %s\n",
              PROGRAM_NAME, strerror(errno));
      abort();
   }

   terminal_settings.c_lflag &= ~(ICANON|ECHO);

   if ( tcsetattr(STDIN_FILENO, TCSANOW, &terminal_settings) == -1 ) {
      fprintf(stderr, "%s: failed to set terminal settings: %s\n",
              PROGRAM_NAME, strerror(errno));
      abort();
   }
}

void teardown(void) {
   struct termios terminal_settings = { 0 };

   if ( tcgetattr(STDIN_FILENO, &terminal_settings) == -1 ) {
      fprintf(stderr, "%s: failed to get terminal settings: %s\n",
              PROGRAM_NAME, strerror(errno));
      abort();
   }

   terminal_settings.c_lflag |= ICANON|ECHO;

   if ( tcsetattr(STDIN_FILENO, TCSANOW, &terminal_settings) == -1 ) {
      fprintf(stderr, "%s: failed to set terminal settings: %s\n",
              PROGRAM_NAME, strerror(errno));
      abort();
   }
}

int main(int argc, char** argv) {
   const char** item = NULL;
   char buf[4] = { 0 };
   MENU menu = Menu.new();

   Menu.set_prompt(menu, ">>");
   Menu.set_height(menu, 3);

   for (item = example_items; *item; item++) {
      Menu.add_item(menu, *item);
   }

   Menu.match(menu);
   Menu.display(menu, stdout);

   setup();
   while ( read(STDIN_FILENO, &buf, 4) != 0 ) {

      if (buf[0] >= 0x20) {
         Buffer.putc(Menu.buffer(menu), buf[0]);
         Menu.display(menu, stdout);
      }

   }
   teardown();

   Menu.destroy(&menu);

   return EXIT_SUCCESS;
}
