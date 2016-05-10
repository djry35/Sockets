#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

typedef enum vals
{
	INITIAL,
	CONNECTING,
	STABLE,
	LOGGEDIN,
} states_t;

typedef enum cmd
{
	LOGIN,
	NEWUSR,
	SEND,
	LOGOUT, 
	EXIT,
	ERROR
} cmd_t;

//longest message is 3 different args, soooo work with that I guess.
typedef struct msg
{
	cmd_t type;
	char arg0[685];
	char arg1[685];
	char arg2[685];
	char user[33];
} msg_t;

//just to keep track of the address and port of the server.
char addr[16];
char port[7];

//creates a socket for use by the client
//Input:
//	serverAddr - the IP of the server that the client is trying to connect to.
//	portNum - the port that the open server socket is listening on
//Return:
//	<0 - error of some kind, see output
//	>0 - a socket descriptor linked to the open and ready socket
//	0 - ???
int createSocket(char* serverAddr, char* portNum);

//parses the command into pieces for ease of access later. Also checks to make
//sure that the command is one that is implemented and the format of the command
//is correct.
//Input:
//	cmd - the command the user entered
//	tokens - strings that will hold the parsed pieces of the command for later use.
//	state - the current state of the client, used to know what kind of command to expect.
//Return:
//	A cmd_t value corresponding to the type of command found, or ERROR if 
//	the command is not valid.
cmd_t parseCMD(char* cmd, char** tokens, states_t state);

//processes command, depending on the type of command given
//Input: 
//	SD - points to the socket descriptor, which links to the open socket
//	user - currently logged in user
//	tokens - parsed out pieces of the command, for ease of access
//	state - points to current state of client, mostly for error checking
//Return:
//	0 - successful processing of command
//	-1 - error having to do with the network, can be retried by user
//	-2 - fatal error (should never happen), exit client
int sendMsg(int* SD, char* user, char** tokens, states_t* state);
int login(int* SD, char* user, char** tokens, states_t* state);
int newUser(int* SD, char* user, char** tokens, states_t* state);
int logout(int* SD, char* user, char** tokens, states_t* state);
