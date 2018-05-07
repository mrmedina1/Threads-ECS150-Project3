# Target library
lib := libuthread.a

all: $(lib)

objs := sem.o 

CFLAGS := -Wall -Werror

$(lib): $(objs)
	ar rcs $(lib) $(objs)

sem.o: sem.c sem.h
	gcc $(CFLAGS) -c -o sem.o sem.c

clean: 
	rm -rf $(objs) $(lib)


## TODO: Phase 1 and Phase 2
