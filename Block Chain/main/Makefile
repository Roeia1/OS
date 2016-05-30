all: libblockchain.a

blockchain.o: blockchain.cpp blockchain.h hash.h Block.h 
	g++ -Wall -Wextra -Wvla -std=c++11 -c blockchain.cpp 

libblockchain.a: Block.o  blockchain.o
	ar rcs libblockchain.a Block.o blockchain.o

Block.o: Block.h Block.cpp
	g++ -Wall -Wextra -Wvla -std=c++11 -c Block.cpp 

tar:
	tar -cvf ex3.tar Block.h Block.cpp blockchain.cpp  README Makefile

clean:
	rm -f blockchain.o Block.o  libblockchain.o libblockchain.a 


.PHONY: clean tar

