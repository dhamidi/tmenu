.POSIX:

.c.o: script/compile
	script/compile $< $@

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
	rm -vf ./buffer_test ./*.o
	rm -vf ./*_actual
