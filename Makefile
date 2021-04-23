CC    = gcc
CFLAGS       = -fPIC -g -Wall -Werror -pedantic
LDFLAGS      = -shared

run: library
	gcc test_code/MT/one.c -lotothread
	./a.out

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
	sudo rm -f la.out libotothread.so libmtothread.so thread.o mtothread.o thread.h.gch /usr/lib/libotothread.so /usr/lib/libmtothread.so
