INCS=-I. -I/cs/course/current/os/lib/ -Luthreads
CFLAGS = -Wall -g -c $(INCS)
LFLAGS = -Wall -g $(INCS)
TARGETS = libuthreads.a
 
all: $(TARGETS)

Thread.o: Thread.cpp Thread.h
	g++ $(CFLAGS) Thread.cpp -o Thread.o 

uthreads.o: uthreads.cpp Thread.h 
	g++ $(CFLAGS) uthreads.cpp -o uthreads.o

libuthreads.a: Thread.o uthreads.o
	ar rc libuthreads.a  Thread.o uthreads.o
	ranlib libuthreads.a

clean:
	rm -r -f *.o libuthreads.a 
 
.PHONY: clean all
	 
	