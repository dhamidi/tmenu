#include "buffer.h"

#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct buffer {
   size_t capacity;
   size_t gapstart;
   size_t gapend;
   char* data;
};

static BUFFER bufnew(size_t hint) {
   BUFFER buf = NULL;

   if (hint < 1024) {
      hint = 1024;
   }

   buf = must_malloc(sizeof(*buf));

   buf->capacity = hint;
   buf->data = must_malloc(hint);
   buf->gapstart = 1;
   buf->gapend = hint - 1;

   return buf;
}

static const char* bufbefore(BUFFER self) {
   return self->data;
}

static const char* bufafter(BUFFER self) {
   return self->data + self->gapend - 1;
}

static void bufdestroy(BUFFER* self) {
   if (self == NULL) {
      return;
   }

   free( (*self)->data );
   free( *self );

   *self = NULL;
}

static int bufisfull(BUFFER self) {
   return (self->gapstart + 1) == self->gapend;
}

static void bufexpand(BUFFER self) {
   char *new_data;
   char *old_data = self->data;
   size_t new_capacity = self->capacity + BUFFER_GROW_BY;
   if (new_capacity <= self->capacity) {
      fprintf(stderr, "%s: cannot expand buffer: integer overflow\n",
              PROGRAM_NAME);
      abort();
   }

   new_data = must_malloc(new_capacity);
   memcpy(new_data, bufbefore(self), self->gapstart);
   memcpy(new_data + (new_capacity - self->gapend),
          bufafter(self),
          self->capacity - self->gapend);

   self->data = new_data;
   self->gapend = new_capacity - self->gapend;

   free(old_data);
}

static void bufputc(BUFFER self, int c) {
   unsigned char byte = (unsigned char)c;
   if (bufisfull(self)) {
      bufexpand(self);
   }

   self->data[self->gapstart - 1] = byte;
   self->data[self->gapstart] = 0;
   self->gapstart++;
}

static void bufputs(BUFFER self, const char* str) {
   while (*str) {
      bufputc(self, *str++);
   }
}

static char* bufstring(BUFFER self, char** buf, size_t* bufsize) {
   size_t new_size = self->gapstart + (self->capacity - self->gapend);

   if ( (*bufsize) < new_size || (*buf) == NULL ) {
      *bufsize = new_size;
      free(*buf);
      *buf = must_malloc(new_size);
   }

   strcpy( (*buf), bufbefore(self) );
   strcpy( (*buf) + self->gapstart - 1 , bufafter(self) );

   return *buf;
}

static void bufforward(BUFFER self, int by) {
   while (by --> 0 && self->gapend < self->capacity - 1) {
      self->data[self->gapstart - 1] = self->data[self->gapend - 1];
      self->data[self->gapend - 1] = 0;
      self->gapstart++;
      self->gapend++;
   }
}

static void bufbackward(BUFFER self, int by) {
   while (by --> 0 && self->gapstart > 1) {
      self->data[self->gapend - 2] = self->data[self->gapstart - 2];
      self->data[self->gapstart - 2] = 0;
      self->gapstart--;
      self->gapend--;
   }
}


static size_t bufpoint(BUFFER self) {
   return self->gapstart;
}

static void bufdelete(BUFFER self) {
   if ( self->gapend < self->capacity - 1 ) {
      self->gapend++;
      self->data[self->gapend] = 0;
   }
}

static void bufclear(BUFFER self) {
   memset(self->data, 0, self->capacity);
   self->gapstart = 1;
   self->gapend = self->capacity - 1;
}

static void bufdeletetoend(BUFFER self) {
   char* after = (char*)bufafter(self);
   memset(after, 0, strlen(after));
   self->gapend = self->capacity - 1;
}

static void bufdeletetobeginning(BUFFER self) {
   char* before = (char*)bufbefore(self);
   memset(before, 0, strlen(before));
   self->gapstart = 1;
}

struct buffer_interface Buffer = {
   .new = bufnew,
   .destroy = bufdestroy,

   .putc = bufputc,
   .puts = bufputs,
   .string = bufstring,
   .forward = bufforward,
   .backward = bufbackward,
   .before = bufbefore,
   .after = bufafter,
   .point = bufpoint,
   .delete = bufdelete,
   .clear = bufclear,
   .delete_to_end = bufdeletetoend,
   .delete_to_beginning = bufdeletetobeginning,
};
