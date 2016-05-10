#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>

typedef enum cmd
{
	LOGIN,
	NEWUSR,
	SEND,
	LOGOUT,
	ERROR	
} cmd_t;

//basic message structure. Contains each piece of the command that the client sent 
typedef struct msg
{
	cmd_t type;
	char arg0[685];
	char arg1[685];
	char arg2[685];
	char user[33];
} msg_t;

//keep track of it instead of argv
char portNum[7];

//processes command, depending on the type of command given
//Input:
//	msg - the message the server received from the client
//	retMsg - the message that the server will send back to the client
//Return:
//	0 - successful processing of command
//	<0 - error
int login(msg_t msg, msg_t* retMsg);
int sendMsg(msg_t msg, msg_t* retMsg);
int newUser(msg_t msg, msg_t* retMsg);
int logout(msg_t msg, msg_t* retMsg);

int createSocket(char* portNum);
