/*
 * Thread.h
 *
 *  Created on: Apr 16, 2015
 *      Author: roeia1
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>
#include "uthreads.h"
#include <setjmp.h>
#include <signal.h>

enum State{READY, RUNNING, BLOCKED};

/**
 * Inner class that represent Thread.
 */
class Thread{

private:
	unsigned int _id;
	Priority _pr;
	char* _stack;
	State _state;
	int _quantumCounter;
	sigjmp_buf _env;

public:
	/**
	 * constructor of Thread.
	 */
	Thread(int id, Priority pr,void (*f)(void));

	/**
	 * Distructor for thread,
	 */
	~Thread();

	/**
	 * increment the quantom of a thread.
	 */
	void incQuantom();

	/**
	 * return the priority color of the thread.
	 */
	Priority getPriority();

	/**
	 * return the state of the tread.
	 */
	State getState();

	/**
	 * set the state of the tread.
	 */
	void setState(State st);

	/**
	 * return the quantomCounter of the thread.
	 */
	int getQuantomCounter();

	/**
	 * return the thread Id.
	 */
	int getID();

	/**
	 *
	 */
	sigjmp_buf* getEnv();
};


#endif /* THREAD_H_ */
