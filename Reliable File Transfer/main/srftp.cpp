/*
 * srftp.cpp
 *
 *  Created on: Jun 9, 2015
 *      Author: roeia1
 */
#include "helper.h"

#define PARAM_NUM 3
#define SERVER_PORT 1
#define MAX_FILE_SIZE 2
#define SERVER_ERROR_MSG "Usage: srftp server-port max-file-size"
#define MAX_LISTEN 5

int maxFileSize;

/*
 * Receiving the file data and creating a new file with the same data.
 */
void recvFileData(int sock, int fileSize, ofstream& fileToCreate)
{
	char* buffer = (char*)malloc(PACKET_SIZE);
	if (buffer == NULL)
	{
		error("malloc");
		pthread_exit(NULL);
	}
	int bytesToWrite = fileSize;
	while (bytesToWrite > PACKET_SIZE)
	{
		recv(sock, buffer, PACKET_SIZE, 0);
		fileToCreate.write(buffer, PACKET_SIZE);
		bytesToWrite -= PACKET_SIZE;
	}
	if (bytesToWrite != 0)
	{
		recv(sock, buffer, bytesToWrite, 0);
		fileToCreate.write(buffer, bytesToWrite);
	}
	free (buffer);
}

/*
 * This function will handle connection for each client to the server.
 */
void* clientHandler(void* sockDesc)
{
	// Get the socket descriptor
	int sock = *(int*)sockDesc;
	// Getting the file size from the client
	char fileSize[sizeof(int)];
	if (recv(sock, fileSize, sizeof(int), 0) == ERROR)
	{
		error("recv");
		pthread_exit(NULL);
	}
	char res[1];
	// Sending the client if the file size is ok
	int nFileSize = *((int*)fileSize);
	(nFileSize <= maxFileSize) ? (res[0] = '1') : (res[0] = '0');
	sendBuffer(sock, res, 1, true);
	// If the file size ok creating the file
	if (res[0] == '1')
	{
		// Getting the file name size
		char nameSize[sizeof(int)];
		if (recv(sock, nameSize, sizeof(int), 0) == ERROR)
		{
			error("recv");
			pthread_exit(NULL);
		}
		// Getting the file name
		int nNameSize = *((int*)nameSize);
		char* fileName = new char[nNameSize + 1];
		if (recv(sock, fileName, nNameSize, 0) == ERROR)
		{
			delete fileName;
			error("recv");
			pthread_exit(NULL);
		}
		fileName[nNameSize] = '\0';
		ofstream fileToCreate(fileName, ofstream::out);
		recvFileData(sock, nFileSize, fileToCreate);
		delete fileName;
	}
	return 0;
}

int main(int argc , char *argv[])
{
	int serverPort = stringToInt(argv[SERVER_PORT], SERVER_ERROR_MSG);
	// Checking num of args, if port legal and if legal max file size
	if (argc != PARAM_NUM || serverPort < MIN_PORT_NUM || serverPort > MAX_PORT_NUM ||
		((maxFileSize = stringToInt(argv[MAX_FILE_SIZE], SERVER_ERROR_MSG)) < 0))
	{
		cout << SERVER_ERROR_MSG << endl;
		exit(EXIT_FAILURE);
	}
	//Prepare the sockaddr_in structure
	char myHostName[MAXHOSTNAMELEN+1];
	struct sockaddr_in sa;
	struct hostent* hp;
	memset(&sa,0,sizeof(struct sockaddr_in));
	gethostname(myHostName, MAXHOSTNAMELEN);
	hp = gethostbyname(myHostName);
	sa.sin_family = hp->h_addrtype;
	memcpy(&sa.sin_addr, hp->h_addr, hp->h_length);
	sa.sin_port = htons((u_short)serverPort);
	//Create socket
	int sockListen;
	if ((sockListen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		error("socket");
		exit(EXIT_FAILURE);
	}
	//Bind
	if(bind(sockListen,(struct sockaddr*)&sa, sizeof(struct sockaddr_in)) < 0)
	{
		close(sockListen);
		error("bind");
		exit(EXIT_FAILURE);
	}
	//Listen
	listen(sockListen, MAX_LISTEN);
	int clientSock;
	while(true)
	{
		if((clientSock = accept(sockListen, NULL, NULL)) < 0)
		{
			error("accept");
			exit(EXIT_FAILURE);
		}
		pthread_t clientThread;
		int* newSock = (int*)malloc(sizeof(int));
		*newSock = clientSock;
		if(pthread_create(&clientThread, NULL, clientHandler, (void*)newSock) < 0)
		{
			error("pthread_create");
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}
