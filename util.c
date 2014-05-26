#include "util.h"

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void* must_malloc(size_t n) {
   void* buffer = calloc(1, n);
   if (buffer == 0) {
      int error = errno;
      fprintf(stderr, "%s: failed to allocate buffer of size %ld: %s\n",
              PROGRAM_NAME, (long int)n, strerror(error));
      abort();
   }

   return buffer;
}
