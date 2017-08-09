/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Name: Jacob Wilson
* Course: CS372_400
* Assignment: Project 1 - Client/Server Chat
* Description: The program serves as the client. It opens
*  a port connection with the server, obtains a unique
*  handle from the user, and performs an alternating chat
*  with the connected server program.
* See References in README file
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

// function protobypes
void getHandle(char*);
int initConnection(int, struct sockaddr_in);
void sendMessage(int, char*, int);

// main function
int main(int argc, char *argv[])
{
	// variables for loop control and string editing
	int quitMessage = 0;
	char *pos;

	// variables for connection and messages
	int sockFD, portNumber, n;
	char messageBuffer[500];
	char sendBuffer[511];
	char receiveBuffer[511];
	char handle[11];

	// variables for socket programming
	struct sockaddr_in serverAddress;
	struct hostent *serverHost;

	// check for correct arguments
	if (argc < 3) {
		fprintf(stderr, "USAGE: chatclient [host] [port]");
		exit(0);
	}

	// set up the connection; taken from my previous CS344 assignment
	memset((char*)&serverAddress, '\0', sizeof(serverAddress));
	portNumber = atoi(argv[2]);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(portNumber);
	serverHost = gethostbyname(argv[1]);
	if (serverHost == NULL) {
		fprintf(stderr, "ERROR: hostname not accepted\n");
		exit(0);
	}
	memcpy((char*)&serverAddress.sin_addr, (char*)serverHost->h_addr, serverHost->h_length);

	// get the user name and connect to the socket
	getHandle(handle);
	sockFD = initConnection(sockFD, serverAddress);
	printf("Connected to the server on PORT: %d\n", portNumber);

	// send initial connection message to server
	sendMessage(sockFD, "Client has connected.", 21);

	// main loop for sending and receiving messages
	while (!quitMessage) {
		// display user name and accept a new message
		printf(handle);
		fflush(stdout);
		fgets(messageBuffer, sizeof(messageBuffer), stdin);
		// swap the newline at the end of string for a NULL
		if ((pos = strchr(messageBuffer, '\n')) != NULL) {
		*pos = '\0';
		}

		//check for quit message
		if (strncmp(messageBuffer, "\\quit", 5) == 0) {
			quitMessage = 1;
		}

		// set up the message and send it to the server
		strcpy(sendBuffer, handle);
		strcat(sendBuffer, messageBuffer);
		sendMessage(sockFD, sendBuffer, n);

		// if the user did not choose to quit, receive a message from the server
		if (!quitMessage) {
			// receive the message
			fflush(stdout);
			memset((char*)&receiveBuffer, '\0', sizeof(receiveBuffer));
			n = read(sockFD, receiveBuffer, sizeof(receiveBuffer));
			if (n < 0) {
				fprintf(stderr, "ERROR reading from socket\n");
				exit(1);
			}

			// display the message from the server, or quit if the server chose to quit
			if (strncmp(receiveBuffer, "quit", 4) != 0) {
				printf("server> %s\n", receiveBuffer);
			} else {
				printf("Quit message received from server, closing connection.\n");
				quitMessage = 1;
			}
		} else {
			printf("Shutting down connection.\n");
		}
	}
	// once the user or server chooses to quit, close the socket and end the program
	close(sockFD);
	return 0;
}

// FUNCTIONS

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Name: getHandle
* Input: char array which holds the user name
* Output: none
* Description: The function accepts input from the user
*  to use as their name. The newline character at the end
*  of user input is swapped with a NULL character.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void getHandle(char *handle)
{
	char *pos;
	int c;

	printf("Please enter your handle: ");
	fflush(stdout);
	fgets(handle, 11, stdin);

	// this section validates input, see references
	if (handle[strlen(handle) - 1] != '\n') {
		int extras = 0;
		// checks if more characters than allowed
		while (fgetc(stdin) != '\n') {
			extras++;
		}

		// if more chars are counted than allowed, ask for handle again
		if (extras > 0) {
			printf("Handle cannot be more than 10 characters.\n");
			getHandle(handle);
		} else {
			strcat(handle, "> ");
		}
	} else {
		// swap end of line char with NULL
		if ((pos = strchr(handle, '\n')) != NULL) {
			*pos = '\0';
		}
		strcat(handle, "> ");
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Name:initConnection
* Input: socket as int, struct holding server info
* Output: int for newly formed socket
* Description: The function sets up the socket using
*  socket-based functions. It returns the file descriptor
*  (int) for the newly formed socket. Will error if there
*  is failure to connect.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int initConnection(int sockFD, struct sockaddr_in serverAddress)
{
	sockFD = socket(AF_INET, SOCK_STREAM, 0);
	if (sockFD < 0) {
		fprintf(stderr, "ERROR opening socket\n");
		exit(1);
	}

	if (connect(sockFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
		perror("ERROR: problem connecting\n");
		exit(1);
	}	

	return sockFD;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Name:sendMessage
* Input: socket as int, char array holding message to send
*  number of bytes sent as int
* Output: none
* Description: THe function uses the provided socket to
*  send the provided message to the server. Will error
*  if there is failure to send.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void sendMessage(int sockFD, char *sendBuffer, int n)
{
	n = write(sockFD, sendBuffer, strlen(sendBuffer));
		if (n < 0) {
			fprintf(stderr, "ERROR writing to socket\n");
			exit(1);
		}
}
