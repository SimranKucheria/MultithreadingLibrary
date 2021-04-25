CC    = gcc
CFLAGS       = -fPIC -g -Wall -Werror -Wextra -Wshadow -Wpedantic 
LDFLAGS      = -shared

run: library
	sudo chmod +x test.sh
	./test.sh

library: libotothread.so libmtothread.so
	sudo cp libotothread.so /usr/lib
	sudo chmod 0755 /usr/lib/libotothread.so
	sudo cp libmtothread.so /usr/lib
	sudo chmod 0755 /usr/lib/libmtothread.so
	sudo ldconfig

libotothread.so: thread.o
	$(CC) ${LDFLAGS} -o $@ thread.o

thread.o:  
	$(CC) -c ${CFLAGS} thread.h thread.c 


libmtothread.so: mtothread.o
	$(CC) ${LDFLAGS} -o $@ mtothread.o

mtothread.o: 
	$(CC) -c ${CFLAGS} thread.h mtothread.c 

clean: 
	sudo rm -f a.out libotothread.so libmtothread.so thread.o mtothread.o thread.h.gch /usr/lib/libotothread.so /usr/lib/libmtothread.so test1 test2 test3 test4 test5 test6 test7 test8 test9 test10 test11 test12 test13 output.txt test_code/MT/a.out
