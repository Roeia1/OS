/*
 * helper.h
 *
 *  Created on: Jun 11, 2015
 *      Author: roeia1
 */

#ifndef HELPER_H_
#define HELPER_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <string>
#include <netdb.h>
#include <unistd.h>
#include <fstream>
#include <cerrno>
#include <limits.h>
#include <stdexcept>
#include <errno.h>
#include <cstring>

using namespace std;

#define PACKET_SIZE 4096
#define MAX_PORT_NUM 65535
#define MIN_PORT_NUM 1
#define SUCCESS '1'
#define ERROR -1

/*
 * This function represent a system call error,
 * receiving the name of the system call printing to cerr.
 */
void error(string systemCall);

/*
 * Sending data through a buffer
 */
void sendBuffer(int sock, char* buffer, int size,  bool isThread);

/*
 * Converting string to int, if fails print
 */
int stringToInt(char* input, string errorMsg);

#endif /* HELPER_H_ */
