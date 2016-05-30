/*
 * helper.cpp
 *
 *  Created on: Jun 11, 2015
 *      Author: roeia1
 */

#include "helper.h"

/*
 * This function represent a system call error,
 * receiving the name of the system call printing to cerr.
 */
void error(string systemCall)
{
	cerr << "Error: function:" << systemCall << " errno:" << strerror(errno) << ".\n" << endl;
}

/*
 * Sending data through a buffer
 */
void sendBuffer(int sock, char* buffer, int size, bool isThread)
{
	int bytesSent = 0;
	int sent;
	while (bytesSent < size)
	{
		sent = send(sock, buffer + bytesSent, size - bytesSent, 0);
		if (sent == ERROR)
		{
			error("send");
			if (isThread)
			{
				pthread_exit(NULL);
			}
			exit(EXIT_FAILURE);
		}
		bytesSent += sent;
	}
}

/*
 * Converting string to int, if fails print
 */
int stringToInt(char* input, string errorMsg)
{
	int num;
	try
	{
		num = std::stoi(input);
	}
	catch (exception &e)
	{
		cout << errorMsg << endl;
		exit(EXIT_FAILURE);
	}
	return num;
}

