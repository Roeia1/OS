/*
 * clftp.cpp
 *
 *  Created on: Jun 9, 2015
 *      Author: roeia1
 */

#include "helper.h"

#define PARAM_NUM 5
#define SERVER_PORT 1
#define SERVER_HOST_NAME 2
#define FILE_TO_TRANSFER 3
#define FILENAME_IN_SERVER 4
#define CLIENT_ERROR_MSG "Usage: clftp server-port server-hostname file-to-transfer filename-in-server"
#define BIG_FILE_MSG "Transmission failed: too big file"

/*
 * Sending the file data
 */
void sendFileData(int sock, int fileSize, ifstream& ifs)
{
	char* buffer = (char*)malloc(PACKET_SIZE);
	if (buffer == NULL)
	{
		error("malloc");
		exit(EXIT_FAILURE);
	}
	int bytesToSend = fileSize;
	while (bytesToSend > PACKET_SIZE)
	{
		ifs.read(buffer, PACKET_SIZE);
		sendBuffer(sock, buffer, PACKET_SIZE, false);
		bytesToSend -= PACKET_SIZE;
	}
	if (bytesToSend != 0)
	{
		ifs.read(buffer, bytesToSend);
		sendBuffer(sock, buffer, bytesToSend, false);
	}
	free (buffer);
}

/*
 * Getting the file size
 */
int getFileSize(ifstream &ifs)
{
	long begin = ifs.tellg();
	ifs.seekg (0, ios::end);
	long end = ifs.tellg();
	ifs.seekg(ios::beg);
	return end - begin;
}

int main(int argc , char *argv[])
{
	int serverPort = stringToInt(argv[SERVER_PORT], CLIENT_ERROR_MSG);
	ifstream fileToTransfer(argv[FILE_TO_TRANSFER], ifstream::in);
	// Checking num of args, if port legal and if the file exists
	if (argc != PARAM_NUM || serverPort < MIN_PORT_NUM || serverPort > MAX_PORT_NUM ||
		strlen(argv[FILENAME_IN_SERVER]) > NAME_MAX || fileToTransfer == NULL)
	{
		cout << CLIENT_ERROR_MSG << endl;
		exit(EXIT_FAILURE);
	}
	int fileSize = getFileSize(fileToTransfer);
    int sock;
    // Init the address
	struct sockaddr_in sa;
    struct hostent* hp;
	hp = gethostbyname(argv[SERVER_HOST_NAME]);
	memset(&sa, 0, sizeof(sa));
    sa.sin_family = hp->h_addrtype;
    memcpy((char*)&sa.sin_addr, hp->h_addr, hp->h_length);
    sa.sin_port = htons((u_short)serverPort);
    // Create socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		error("socket");
		exit(EXIT_FAILURE);
	}
    //Connect to remote server
    if (connect(sock , (struct sockaddr*)&sa , sizeof(sa)) < 0)
    {
    	cerr << strerror(errno) << endl;
    	close(sock);
        return 1;
    }
	sendBuffer(sock, (char*)&fileSize, sizeof(int), false);
    char checkSize[1];
    if (recv(sock, checkSize, 1, 0) == ERROR)
    {
    	error("recv");
		exit(EXIT_FAILURE);
    }
    if (checkSize[0] == SUCCESS)
    {
    	// Sending the file name in server size
    	int nameSize = strlen(argv[FILENAME_IN_SERVER]);
    	sendBuffer(sock, (char*)&nameSize, sizeof(int), false);
    	// Sending the file name in server
    	sendBuffer(sock, argv[FILENAME_IN_SERVER], nameSize, false);
    	// Sending the file data
    	sendFileData(sock, fileSize, fileToTransfer);
    }
    else
    {
    	cout << BIG_FILE_MSG << endl;
    }
    close(sock);
    fileToTransfer.close();
    return 0;
}
