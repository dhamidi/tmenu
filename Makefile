# Copyright 2014 Dario Hamidi
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

.POSIX:

PREFIX=/usr/local

CC?=gcc

CFLAGS=-g -pedantic -Wall -Werror -std=c99

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

tmenu: main.o terminal.o textbuffer.o buffer.o menu.o util.o
	$(CC) main.o terminal.o textbuffer.o buffer.o menu.o util.o -o $@

install: tmenu test
	mkdir -p $(PREFIX)/bin
	cp tmenu $(PREFIX)/bin/
	chmod 755 $(PREFIX)/bin/tmenu
	mkdir -p $(PREFIX)/share/man/man1
	cp tmenu.1 $(PREFIX)/share/man/man1

run: tmenu
	./tmenu

main.o: main.c script/compile config.h terminal.h textbuffer.h buffer.h menu.h

terminal.o: terminal.c terminal.h script/compile

util.o: util.h util.c script/compile

textbuffer.o: textbuffer.h textbuffer.c buffer.h buffer.c

buffer.o: util.o config.h script/compile buffer.h buffer.c

buffer_test: buffer_test.o buffer.o util.o script/link
	$(CC) -o $@ buffer_test.o buffer.o util.o

menu.o: util.o config.h textbuffer.o buffer.o script/compile menu.h menu.c

menu_test: menu_test.o textbuffer.o buffer.o util.o menu.o terminal.o script/link
	$(CC) -o $@ menu_test.o textbuffer.o buffer.o util.o menu.o terminal.o

test: script/test buffer_test menu_test
	script/test buffer_test
	script/test menu_test

clean:
	rm -vf  tmenu
	rm -vf ./*_test ./*.o
	rm -vf ./*_actual
