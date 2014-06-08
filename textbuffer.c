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
#include "textbuffer.h"

static void bufbackward(BUFFER self, int n) {
   const char* before = Buffer.before(self);
   size_t before_len = Buffer.point(self) - 1;
   size_t pos = before_len - 1;

   while (n --> 0) {
      if ( (unsigned char)before[pos] <= 0x70) {
         // ASCII
         Buffer.backward(self, 1);
      } else {
         // UTF-8
         while ( (unsigned char)before[pos] >> 6 == 2) {
            pos--;
         }
         Buffer.backward(self, before_len - pos);
      }
   }
}

static void bufforward(BUFFER self, int n) {
   const char* after = Buffer.after(self);

   while (n --> 0) {
      Buffer.forward(self, 1);
      while ( (unsigned char)*(++after) >> 6 == 2)
         Buffer.forward(self, 1);
   }
}

static void bufdelete(BUFFER self) {
   const char* after = Buffer.after(self);

   Buffer.delete(self);
   while ( (unsigned char)*(++after) >> 6 == 2 )
      Buffer.delete(self);
}

static void bufcput(BUFFER self, int c) {
   Buffer.cput(self, c);
}

static void bufsput(BUFFER self, const char* str) {
   Buffer.sput(self, str);
}

static BUFFER bufnew(size_t hint) {
   return Buffer.new(hint);
}

static void bufdestroy(BUFFER* self) {
   Buffer.destroy(self);
}

static char* bufstring(BUFFER self, char** out, size_t* len) {
   return Buffer.string(self, out, len);
}

static void bufclear(BUFFER self) {
   Buffer.clear(self);
}

static void bufdeletetoend(BUFFER self) {
   Buffer.delete_to_end(self);
}

static void bufdeletetobeginning(BUFFER self) {
   Buffer.delete_to_beginning(self);
}

static size_t bufpoint(BUFFER self) {
   return Buffer.point(self);
}

static const char* bufafter(BUFFER self) {
   return Buffer.after(self);
}

static const char* bufbefore(BUFFER self) {
   return Buffer.before(self);
}

struct buffer_interface TextBuffer = {
   .new = bufnew,
   .destroy = bufdestroy,

   .cput = bufcput,
   .sput = bufsput,
   .string = bufstring,
   .forward = bufforward,
   .backward = bufbackward,
   .delete = bufdelete,
   .clear = bufclear,
   .delete_to_end = bufdeletetoend,
   .delete_to_beginning = bufdeletetobeginning,
   .point = bufpoint,
   .after = bufafter,
   .before = bufbefore,
};
