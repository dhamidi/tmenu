.POSIX:

PREFIX=/usr/local

.c.o: script/compile
	script/compile $< $@

install: tmenu test
	mkdir -p $(PREFIX)/bin
	cp tmenu $(PREFIX)/bin/
	chmod 755 $(PREFIX)/bin/tmenu
	mkdir -p $(PREFIX)/share/man/man1
	cp tmenu.1 $(PREFIX)/share/man/man1

tmenu: main.o terminal.o buffer.o menu.o util.o
	script/link $@ main.o terminal.o buffer.o menu.o util.o

run: tmenu
	./tmenu

main.o: main.c script/compile config.h terminal.h buffer.h menu.h

terminal.o: terminal.c terminal.h script/compile

util.o: util.h util.c script/compile

buffer.o: util.o config.h script/compile buffer.h buffer.c

buffer_test: buffer_test.o buffer.o util.o script/link
	script/link $@ buffer_test.o buffer.o util.o


menu.o: util.o config.h buffer.o script/compile menu.h menu.c

menu_test: menu_test.o buffer.o util.o menu.o terminal.o script/link
	script/link $@ menu_test.o buffer.o util.o menu.o terminal.o

test: script/test buffer_test menu_test
	script/test buffer_test
	script/test menu_test

clean:
	rm -vf  tmenu
	rm -vf ./*_test ./*.o
	rm -vf ./*_actual
