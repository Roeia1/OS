/*
 * Block.cpp
 *
 *  Created on: Apr 30, 2015
 *      Author: roeia1
 */
#include "Block.h"
#include <stdlib.h>
#include <cstring>

/**
 * constructor to Block.
 */
Block::Block(int father,int id, size_t length, int depth, char* data):_father(father), _id(id),  _length(length),
_depth(depth), _hash(0), _toLongest(false)
{
	_data = (char*)malloc(length * sizeof(char));
	strcpy(_data, data);
}

/**
 * Destructor to block.
 */
Block::~Block()
{
	free (_data);
	free (_hash);
}

/**
 * Return the data of the block.
 */
char* Block::getData() const
{
	return _data;
}

/**
 * Set the hash data of the block.
 */
void Block::setHash(char* hash)
{
	_hash = hash;
}

/**
 * Return the block's Id.
 */
int Block::getId() const
{
	return _id;
}

/**
 * Return the block's deapth.
 */
int Block::getDepth() const
{
	return _depth;
}

/**
 * Return the stat length of the block.
 */
size_t Block::getLength() const
{
	return _length;
}

/**
 * Return the block's father's id.
 */
int Block::getFather() const
{
		return _father;
}

/**
 * Return true if this block was called by to_londest, false otherwise.
 */
bool Block::isToLongest() const
{
	return _toLongest;
}

/**
 * Set the block's toLongest field.
 */
void Block::setToLongest(bool toLongest)
{
	_toLongest = toLongest;
}

/**
 * Set the block's father's id.
 */
void Block::setFather(int father)
{
	_father = father;
}
