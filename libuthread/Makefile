# Target library
lib := libuthread.a

all: $(lib)

objs := sem.o queue.o tps.o

CFLAGS := -Wall -Werror

$(lib): $(objs)
	ar rcs $(lib) $(objs) thread.o

sem.o: sem.c sem.h
	gcc $(CFLAGS) -c -o sem.o sem.c

queue.o: queue.c queue.h
	gcc $(CFLAGS) -c -o queue.o queue.c

tps.o: tps.c tps.h
	gcc $(CFLAGS) -c -o tps.o tps.c

clean: 
	rm -rf $(objs) $(lib)