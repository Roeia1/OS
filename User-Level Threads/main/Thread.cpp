/*
 * Thread.cpp
 *
 *  Created on: Apr 16, 2015
 *      Author: roeia1
 */

#include "Thread.h"
#include "uthreads.h"
#include <iostream>
using namespace std;
#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
 Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
	address_t ret;
	asm volatile("xor    %%fs:0x30,%0\n"
			"rol    $0x11,%0\n"
			: "=g" (ret)
			  : "0" (addr));
	return ret;
}

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5

/* A translation is required when using an address of a variable.
 Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
	address_t ret;
	asm volatile("xor    %%gs:0x18,%0\n"
			"rol    $0x9,%0\n"
			: "=g" (ret)
			  : "0" (addr));
	return ret;
}

#endif

/**
 * constructor of Thread.
 */
Thread::Thread(int id, Priority pr, void (*f)(void)) :
				_id(id), _pr(pr), _state(READY), _quantumCounter(0)
{
	_stack = new char[STACK_SIZE];
	address_t sp, pc;

	sp = (address_t)_stack + STACK_SIZE - sizeof(address_t);
	pc = (address_t) f;

	sigsetjmp(_env, 1);
	(_env->__jmpbuf)[JB_SP] = translate_address(sp);
	(_env->__jmpbuf)[JB_PC] = translate_address(pc);
	sigemptyset(&_env->__saved_mask);

}

/**
 * Distructor for thread,
 */
Thread::~Thread()
{
	delete (_stack);
}

/**
 * increment the quantom of a thread.
 */
void Thread::incQuantom()
{
	_quantumCounter++;
}

/**
 * return the priority color of the thread.
 */
Priority Thread::getPriority()
{
	return _pr;
}

/**
 * return the state of the tread.
 */
State Thread::getState()
{
	return _state;
}

/**
 * set the state of the tread.
 */
void Thread::setState(State st)
{
	_state = st;
}

/**
 * return the quantomCounter of the thread.
 */
int Thread::getQuantomCounter()
{
	return _quantumCounter;
}

/**
 * return the thread Id.
 */
int Thread::getID()
{
	return _id;
}
/**
 *
 */
	sigjmp_buf* Thread::getEnv()
{
	return &_env;
}

