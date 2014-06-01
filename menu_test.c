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
   Menu.display(menu, stdout);
   Menu.display(menu, stdout);
   Menu.destroy(&menu);
   return 0;
}
