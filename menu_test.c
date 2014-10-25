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
#define _POSIX_C_SOURCE 200809L
#include <assert.h>

#include "menu.c"

void test_menugrow_updates_capacity() {
   MENU menu = menunew();
   size_t capacity_before_grow = menu->capacity;
   menugrow(menu);
   assert(menu->capacity  == (capacity_before_grow + MENU_GROW_BY));
   menudestroy(&menu);
}

int main(int argc, char** argv) {
   test_menugrow_updates_capacity();
}
