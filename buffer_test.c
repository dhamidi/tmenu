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
