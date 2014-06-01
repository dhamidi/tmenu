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
#include "menu.h"

int main(int argc, char** argv) {
   MENU menu = Menu.new();
   Menu.set_prompt(menu, "choose:");
   Menu.set_height(menu, 2);
   Menu.add_item(menu, "hello world");
   Menu.add_item(menu, "goodbye world");
   Menu.add_item(menu, "cruel world");
   Buffer.sput(Menu.buffer(menu), "world");
   Buffer.backward(Menu.buffer(menu), 100);
   Menu.match(menu);
   Menu.select_next(menu);
   Menu.select_next(menu);
   Menu.select_next(menu);
   Menu.select_next(menu);
   Menu.display(menu, stdout);
   Menu.display(menu, stdout);
   Menu.destroy(&menu);
   return 0;
}
