#include "buffer.h"
#include <stdio.h>

int main(int argc, char** argv) {
   BUFFER buffer = Buffer.new(1024);
   char*  string = NULL;
   size_t string_len = 0;

   Buffer.puts(buffer, "hello, world");
   puts(Buffer.string(buffer, &string, &string_len));

   Buffer.backward(buffer, 5);
   fputs(Buffer.before(buffer),stdout);
   fputs("<GAP>", stdout);
   puts(Buffer.after(buffer));

   Buffer.puts(buffer,"new ");
   puts(Buffer.string(buffer, &string, &string_len));

   Buffer.backward(buffer, 100);
   Buffer.forward(buffer, 4);
   Buffer.puts(buffer," t");
   puts(Buffer.string(buffer, &string, &string_len));
   return 0;
}
