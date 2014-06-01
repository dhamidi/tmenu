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
#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
   BUFFER buffer = Buffer.new(1024);
   char*  string = NULL;
   size_t string_len = 0;

   Buffer.sput(buffer, "hello, world");
   printf("%ld\n", (long int)Buffer.point(buffer));
   puts(Buffer.string(buffer, &string, &string_len));

   Buffer.backward(buffer, 5);
   fputs(Buffer.before(buffer),stdout);
   fputs("<GAP>", stdout);
   puts(Buffer.after(buffer));

   Buffer.sput(buffer,"new ");
   puts(Buffer.string(buffer, &string, &string_len));

   Buffer.backward(buffer, 100);
   Buffer.forward(buffer, 4);
   Buffer.sput(buffer," t");
   puts(Buffer.string(buffer, &string, &string_len));

   Buffer.delete_to_beginning(buffer);
   puts(Buffer.string(buffer, &string, &string_len));

   Buffer.delete_to_end(buffer);
   puts(Buffer.string(buffer, &string, &string_len));

   Buffer.sput(buffer, "κάθαρσις");
   puts(Buffer.string(buffer, &string, &string_len));

   Buffer.destroy(&buffer);

   free(string);
   return 0;
}
