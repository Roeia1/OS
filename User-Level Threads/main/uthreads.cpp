/*
 * uthreads.cpp
 *
 *  Created on: Apr 12, 2015
 *      Author: roeia1
 */
#include <map>
#include <list>
#include <cstdlib>
#include <iostream>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <queue>
#include <signal.h>
#include <unistd.h>
#include <stdexcept>
#include "uthreads.h"
#include "Thread.h"
using namespace std;

#define SEC 1000000
#define UNBLOCK_ALARM 0
#define BLOCK_ALARM 1
#define TERMINATE_SIG 32
#define BLOCK_SIG 33
#define THREAD_ERR "thread library error: "
#define SYS_ERR "system error: "
#define LIST_BAD_ALLOC "failed allocate thread list.\n"
#define THREAD_BAD_ALLOC "failed allocate thread.\n"
#define INVALID_ID "invalid thread's id.\n"
#define MASK_FAIL "failed to create mask.\n"

sigset_t blockedMasks;

//number of so far used quantum (of all threads)
int quantumCounter;

//quantum size in u-seconds
int myQuantomUsecs;

//the id of current running thread
int runningThread;

// the priority queues of ready threads.
list<int> redQ, orangeQ, greenQ;

// restor the blocked threads.
list<int> blockedList;

//restore all the available id to a new thread.
priority_queue<int,vector<int>,greater<int> > availbleIDq;

//the actual container of all threads, indexed by thread-id
map<int, Thread*> threadMap;



/**********************
 * forward declaration.
 **********************/
void switchThreads(int sig);
void setAlarm(int flag);
void deleteMap();
unsigned int nextThread(int id);
void resetTimer();
void dummyFunc(){cout<<"very bad"<<endl;}// use for the main entry.



/*
 * Initialize the thread library
 * Return error if the given quantum_usecs is invalid (should be non-negative).
 */
int uthread_init(int quantum_usecs)
{
	if (quantum_usecs <= 0)
	{
		cerr << "thread library error: quantum_usecs input invalid.\n";
		return -1;
	}
	myQuantomUsecs = quantum_usecs;
	quantumCounter = 1;
	runningThread = 0;
	//init the available id queue.
	for (int i = 1; i < MAX_THREAD_NUM; ++i)
	{
		availbleIDq.push(i);
	}
	signal(SIGVTALRM, switchThreads);
	if(sigemptyset(&blockedMasks) != 0)
	{
		cerr<<THREAD_ERR<<MASK_FAIL;
	}

	if(sigaddset(&blockedMasks, SIGVTALRM) != 0){
		cerr<<THREAD_ERR<<MASK_FAIL;
	}
	Thread* mainThread = new Thread(0,ORANGE,dummyFunc);
	mainThread->incQuantom();
	mainThread->setState(RUNNING);
	resetTimer();
	threadMap[0] = mainThread;
	return 0;
}


/*
 * Create a new thread whose entry point is f
 */
int uthread_spawn(void (*f)(void), Priority pr)
{
	if(f==NULL)
	{
		cerr<<THREAD_ERR<<"given function pointer is NULL\n";
		return -1;
	}

	if(availbleIDq.empty())
	{
		cerr<<THREAD_ERR<<"reached the maximal number of threads.\n";
		return -1;
	}
	setAlarm(BLOCK_ALARM);
	Thread* newThread = new Thread(availbleIDq.top(),pr,f);
	availbleIDq.pop();
	switch (pr) {
	case RED:
		try
		{
			redQ.push_back(newThread->getID());
		}
		catch(bad_alloc &e)
		{
			cerr<<THREAD_ERR<<LIST_BAD_ALLOC;
			setAlarm(UNBLOCK_ALARM);
			return -1;
		}
		break;
	case ORANGE:
		try
		{
			orangeQ.push_back(newThread->getID());
		}
		catch(bad_alloc &e)
		{
			cerr<<THREAD_ERR<<LIST_BAD_ALLOC;
			setAlarm(UNBLOCK_ALARM);
			return -1;
		}
		break;
	case GREEN:
		try
		{
			greenQ.push_back(newThread->getID());
		}
		catch(bad_alloc &e)
		{
			cerr<<THREAD_ERR<<LIST_BAD_ALLOC;
			setAlarm(UNBLOCK_ALARM);
			return -1;
		}
		break;
	}
	threadMap[newThread->getID()] = newThread;
	setAlarm(UNBLOCK_ALARM);
	return newThread->getID();
}


/*
 * Terminate a thread
 * Return error if the given id is invalid (not exist).
 */
int uthread_terminate(int tid)
{

	setAlarm(BLOCK_ALARM);
	if (tid == 0)
	{
		deleteMap();
		exit(0);
	}

	if(runningThread == tid)
	{
		switchThreads(TERMINATE_SIG);
	}
	else
	{
		// Check if the thread exists in map
		try
		{
			Thread* threadToDelete = threadMap.at(tid);
			if (threadToDelete->getState() == BLOCKED)
			{
				blockedList.remove(tid);
			}
			else
			{
				switch (threadToDelete->getPriority())
				{
				case RED:
					redQ.remove(tid);
					break;
				case ORANGE:
					orangeQ.remove(tid);
					break;
				case GREEN:
					greenQ.remove(tid);
					break;
				}
			}
			availbleIDq.push(tid);
			threadMap.erase(tid);
			delete threadToDelete;
		}
		catch(out_of_range &e)
		{
			cerr<<THREAD_ERR<<INVALID_ID;
			setAlarm(UNBLOCK_ALARM);
			return -1;
		}
	}
	setAlarm(UNBLOCK_ALARM);
	return 0;
}


/**
 *  Suspend a thread.
 *  Return error if the given id is invalid (not exist).
 */
int uthread_suspend(int tid)
{
	setAlarm(BLOCK_ALARM);
	if(tid == 0) // thread is main error - can't suspend the main.
	{
		cerr<<THREAD_ERR<<"can't suspend main thread.\n";
		setAlarm(UNBLOCK_ALARM);
		return -1;
	}

	if(tid != runningThread) // blocking not the running thread
	{
		try
		{
			Thread* threadToBlock = threadMap.at(tid);
			if(threadToBlock->getState() != BLOCKED)
			{
				switch (threadToBlock->getPriority()) {
				case RED:
					redQ.remove(tid);
					break;
				case ORANGE:
					orangeQ.remove(tid);
					break;
				case GREEN:
					greenQ.remove(tid);
					break;
				}
				blockedList.push_back(tid);
				threadToBlock->setState(BLOCKED);
			}
			setAlarm(UNBLOCK_ALARM);
		}
		catch(out_of_range &e)
		{
			cerr<<THREAD_ERR<<INVALID_ID;
			setAlarm(UNBLOCK_ALARM);
			return -1;
		}
	}
	else // blocking the running thread
	{
		setAlarm(UNBLOCK_ALARM);
		switchThreads(BLOCK_SIG);
	}
	setAlarm(UNBLOCK_ALARM);
	return 0;
}


/**
 *  Resume a thread that was suspended.
 *  If the the given thread is'nt blocked from before - ignore.
 *  Return error if the given id is invalid (not exist).
 */
int uthread_resume(int tid)
{
	setAlarm(BLOCK_ALARM);
	try
	{
		Thread* threadToResume = threadMap.at(tid);
		if(threadToResume->getState() == BLOCKED)
		{
			threadToResume->setState(READY);
			blockedList.remove(tid);
			switch (threadToResume->getPriority()) {
			case RED:
				redQ.push_back(tid);
				break;
			case ORANGE:
				orangeQ.push_back(tid);
				break;
			case GREEN:
				greenQ.push_back(tid);
				break;
			}
		}
	}
	catch(out_of_range &e)
	{
		cerr<<THREAD_ERR<<INVALID_ID;
		setAlarm(UNBLOCK_ALARM);
		return -1;
	}
	setAlarm(UNBLOCK_ALARM);
	return 0;
}


/*
 * Get the id of the calling thread
 */
int uthread_get_tid()
{
	return runningThread;
}

/*
 * Get the total number of library quantums.
 */
int uthread_get_total_quantums()
{
	return quantumCounter;
}


/*
 *  Get the number of thread quantums.
 *  Return error if the given id is invalid (not exist).
 */
int uthread_get_quantums(int tid)
{
	int threadQuantom;
	try
	{
		Thread* t = threadMap.at(tid);
		threadQuantom = t->getQuantomCounter();
	}
	catch(out_of_range &e)
	{
		cerr<<THREAD_ERR<<INVALID_ID;
		return -1;
	}
	return threadQuantom;
}


/**
 * this function switch between two threads according to the transition label (Preempt, Terminate, Blocked).
 */
void switchThreads(int sig)
{
	setAlarm(BLOCK_ALARM);
	int prevThread = runningThread;
	runningThread = nextThread(runningThread);

	//only the main thread is available. so we continue running with it without switching.
	if(runningThread == prevThread)
	{
		Thread* runt = threadMap.at(runningThread);
		quantumCounter++;
		runt->setState(RUNNING);
		runt->incQuantom();
		setAlarm(UNBLOCK_ALARM);
		return;
	}

	Thread* prevt = threadMap.at(prevThread);
	Thread* runt = threadMap.at(runningThread);
	int ret_val = sigsetjmp(*prevt->getEnv(),1);
	if (ret_val == 1) {
		setAlarm(UNBLOCK_ALARM);
		return;
	}
	quantumCounter++;
	runt->setState(RUNNING);
	runt->incQuantom();

	//Switch the threads according to the given signal case.
	switch(sig)
	{

	case BLOCK_SIG:
		blockedList.push_back(prevThread);
		prevt->setState(BLOCKED);
		break;

	case TERMINATE_SIG:
		threadMap.erase(prevThread);
		delete prevt;
		availbleIDq.push(prevThread);
		break;

	case SIGVTALRM:
		switch (prevt->getPriority())
		{
		case RED:
			redQ.push_back(prevThread);
			break;
		case ORANGE:
			orangeQ.push_back(prevThread);
			break;
		case GREEN:
			greenQ.push_back(prevThread);
			break;
		}
		prevt->setState(READY);
		break;
	}

	if (sig != SIGVTALRM)
	{
		resetTimer();
	}
	siglongjmp(*runt->getEnv(),1);
}


/**
 * Return the next thread id to run- according to the priority of the threads.
 */
unsigned int nextThread(int id)
{
	if(!redQ.empty())
	{
		id = redQ.front();
		redQ.pop_front();
		return id;
	}
	if (!orangeQ.empty())
	{
		id = orangeQ.front();
		orangeQ.pop_front();
		return id;
	}
	if(!greenQ.empty())
	{
		id = greenQ.front();
		greenQ.pop_front();
	}
	return id;
}


/*
 * Add or remove clock alarm signal to blocked signals mask
 */
void setAlarm(int flag)
{
	switch (flag) {
	case UNBLOCK_ALARM:

		if(sigprocmask(SIG_UNBLOCK, &blockedMasks, NULL)!=0){
			cerr<<SYS_ERR<<"unblocking signal failed.\n";
		}
		sigset_t sigset;
		sigpending(&sigset);
		int x, res;
		res = sigismember(&sigset, SIGVTALRM);
		if(res)
		{
			sigwait(&blockedMasks, &x);
		}
		break;
	case BLOCK_ALARM:
		if(sigprocmask(SIG_BLOCK, &blockedMasks, NULL)!=0){
			cerr<<SYS_ERR<<"blocking signal failed.\n";
		}
		break;
	default:
		break;
	}
}


/*
 * clear all heap allocated memory
 */
void deleteMap()
{
	map<int, Thread*>::iterator it;
	it = threadMap.begin();
	while(it!=threadMap.end()){
		delete (it->second);
		it++;
	}
}

/**
 * Reset the time structure.
 */
void resetTimer()
{
	struct itimerval tv;
	tv.it_value.tv_sec = myQuantomUsecs/SEC;
	tv.it_value.tv_usec = myQuantomUsecs%SEC;
	tv.it_interval.tv_sec = myQuantomUsecs/SEC;
	tv.it_interval.tv_usec = myQuantomUsecs%SEC;
	if(setitimer(ITIMER_VIRTUAL, &tv, NULL) != 0)
	{
		cerr<<SYS_ERR<<"set timer failed\n";
	}
}
