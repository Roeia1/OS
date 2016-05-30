CXX=g++
FLAGS= -Wall -Wvla -Wextra -g -std=c++11

all: srftp clftp

srftp : srftp.cpp helper.o
	${CXX} ${FLAGS} -lpthread -o $@ $^
	
helper.o: helper.cpp helper.h
	${CXX} ${FLAGS} -c $<
	
clftp: clftp.cpp helper.o
	${CXX} ${FLAGS} -o $@ $^
	
clean:
	rm -f sftrp clftp *.o

tar: ex5.tar

ex5.tar: srftp.cpp clftp.cpp helper.h helper.cpp README Makefile
	tar -cvf $@ $^
	
.PHONY: tar, clean, all