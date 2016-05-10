#include "sockets.h"

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		printf("Correct usage: [sudo] <Executable> <Server Address> <Port Number> \n");
		return -1;	
	}
	int SD;
	//EDIT: turns out, this screws with things, so I'll take it out for now.
	/*
	//before we do anything, we need to test to make sure there is a 
	//connection. If there is no connection, we can't do anything. 
	int SD = createSocket(argv[1], argv[2]);
	if(SD < 0)
	{
		printf("Could not establish connection. Try again later.\n");
		return -1;
	}
	//if we get a connection, save the args so we can use them later.
	memcpy(addr, argv[1], strlen(argv[1]) + 1);
	memcpy(port, argv[2], strlen(argv[2]) + 1);
	//close the test socket, don't need it.
	msg_t m;
	strcpy(m.arg0, "exit");
	m.type = LOGOUT;
	write(SD, (void*)&m, sizeof(msg_t));
	close(SD);	
	*/
	memcpy(addr, argv[1], strlen(argv[1]) + 1);
	memcpy(port, argv[2], strlen(argv[2]) + 1);
	//where the command is stored
	char cmd[2048];
	memset((void*)cmd, 0, 2047*sizeof(char));
	//will hold the pieces of the cmd for interpretation
	char** tokens = malloc(sizeof(char*)*3);
	int numTokens = 0;
	for(numTokens = 0; numTokens < 3; numTokens++)
	{
		tokens[numTokens] = malloc(sizeof(char)*685);
	}
	//will be the actual command we are executing
	cmd_t cmdVal;
	//state of the chat room, e.g., are we logged in, etc
	states_t state;
	//name of the user currently logged in.
	char user[33];	
	//fxns for each valid command
	int (*cmdProc[4])(int*, char*, char**, states_t*);
	cmdProc[LOGIN] = login;
	cmdProc[NEWUSR] = newUser;
	cmdProc[SEND] = sendMsg;
	cmdProc[LOGOUT] = logout;

	printf("Welcome to the chat room!\n");
	
	while(1)
	{
		state = INITIAL;
		//at first, the user can only login or create a new user.
		//Or exit, I guess.
		printf("Please login or create a new user.\n");
		printf("> ");
		fflush(stdin);
		memset((void*)cmd, 0, 2047*sizeof(char));
		for(numTokens = 0; numTokens < 3; numTokens++)
		{
			memset(tokens[numTokens], 0, sizeof(char)*685);
		}
		fgets(cmd, 2047, stdin);
		while((cmdVal = parseCMD(cmd, tokens, state)) == ERROR)
		{
			//parseCMD checks for valid commands that are 
			//implemented for the client
			printf("Invalid command. Please login or create a new user.\n");
			printf("> ");
			memset((void*)cmd, 0, 2047*sizeof(char));
			for(numTokens = 0; numTokens < 3; numTokens++)
			{
				memset(tokens[numTokens], 0, sizeof(char)*685);
			}
			fflush(stdin);
			fgets(cmd, 2047, stdin);
		}		
		state = CONNECTING;	
		while(state != INITIAL)
		{
			//by now, they will first be logging in or creating
			//a new user, but after that we know they won't be 
			//able to login or create new user unless they log out
			//first.
			if(cmdVal == EXIT)
			{
				printf("Come back soon!\n");
				if(state == LOGGEDIN)
				{
					cmdProc[LOGOUT](&SD, user, tokens, &state);
				}
				return 0;
			}
			else if(cmdVal == ERROR)
			{
				//should never actually get here. 
				printf("A fatal error occurred. Please restart client\n");
				if(state == LOGGEDIN)
				{
					cmdProc[LOGOUT](&SD, user, tokens, &state);
				}
				return -1;
			}
			else
			{
				//command is good, and not exit, so process it.
				int err = cmdProc[cmdVal](&SD, user, tokens, &state);
				//fatal error for the client. Anything else
				//will just retrieve a new command.
				if(err == -2)
				{
					close(SD);
					return -1;
				}
			}	
			if(state != INITIAL)
			{
				//get next command
				printf("> ");
				memset((void*)cmd, 0, 2047*sizeof(char));
				for(numTokens = 0; numTokens < 3; numTokens++)
				{
					memset(tokens[numTokens], 0, sizeof(char)*685);
				}
				fflush(stdin);
				fgets(cmd, 2047, stdin);
				while((cmdVal = parseCMD(cmd, tokens, state)) == ERROR)
				{
					//see above parseCMD note
					printf("Invalid command. Try a different command.\n");
					printf("> ");
					memset((void*)cmd, 0, 2047*sizeof(char));
					for(numTokens = 0; numTokens < 3; numTokens++)
					{
						memset(tokens[numTokens], 0, sizeof(char)*685);
					}
					fflush(stdin);
					fgets(cmd, 2047, stdin);
				}
			}
		}
	}
	
}

int createSocket(char* serverAddr, char* portNum)
{
	//okay, so steps we need to take:
	//1. set up the configuration for the socket that is allowed on server side.
	//2. knowing this, find the addresses that can be linked to using the given IP for the server.
	//3. test each address to see if we can connect a client socket to the open server socket 
	//assuming this is all okay, we will link sockets. Otherwise, an error occurs. 
	
	//so this guy has configurations that we want. Everything else can be 0ed out.
	struct addrinfo hints;
	memset((void*)&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	//then, we get a linked list of addresses we can test the socket on. 
	//I believe how it works is that the addresses are all server addrs
	//that have open sockets 	
	struct addrinfo* res;
	if(getaddrinfo(serverAddr, portNum, &hints, &res) < 0)
	{
		perror("Address Error");
		printf("Check your parameters and try again.\n");
		return -1;
	}
	struct addrinfo* rptr = res;
	int SD;
	while(rptr)
	{
		//so for each address, we try to create a socket for it. 
		if((SD = socket(rptr->ai_family, rptr->ai_socktype || SOCK_NONBLOCK, 
			rptr->ai_protocol)) != -1)
		{
			//if a socket is made successfully, try to link the 
			//socket to the open server socket
			if(connect(SD, rptr->ai_addr, rptr->ai_addrlen) >= 0)
			{
				break;
			}
			close(SD);
		}
		rptr = rptr->ai_next;
	}
	
	//no longer needed
	freeaddrinfo(res);	

	//tells us that all addresses were tested, and none of them worked.
	if(!rptr)
	{
		//turns out you must run the program as root to have
		//permissions to the sockets
		printf("No connections established. Are you root? Make sure that the server is online as well.\n");
		return -1;
	}

	printf("connection established.\n");
	
	return SD;
}
