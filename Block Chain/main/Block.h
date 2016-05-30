/*
 * Block.h
 * This class represent one block in the cahinBlock.
 *  Created on: Apr 30, 2015
 */

#ifndef BLOCK_H_
#define BLOCK_H_
#include <cstdio>
class Block{
private:
	int _father;
	int _id;
	size_t _length;
	int _depth;
	char* _data;
	char* _hash;
	bool _toLongest;
public:

	// constructor to Block.
	Block(int father, int id, size_t length, int depth = 0, char* data = 0);

	//Distructor to block.
	~Block();

	//Return the data of the block.
	char* getData() const;

	//Set the hash data of the block.
	void setHash(char* hash);

	//Return the block's Id.
	int getId() const;

	//Return the block's deapth.
	int getDepth() const;

	//Return the stat length of the block.
	size_t getLength() const;

	//Return the block's father's id.
	int getFather() const;

	//Set the block's father's id.
	void setFather(int father);

	//Return true if this block was called by to_londest, false otherwise/
	bool isToLongest() const;

	//Set the block's toLongest field.
	void setToLongest(bool toLongest);
};



#endif /* BLOCK_H_ */
