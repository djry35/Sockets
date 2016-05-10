#include "sockets.h"

int login(int* SD, char* user, char** tokens, states_t* state)
{	
	//can't login if you are already logged in
	if(*state == LOGGEDIN)
	{
		printf("You are already logged in! Please logout to become a new user.\n");
		return -1;
	}
	else
	{
		//a socket might have already been made for some reason,
		//so have to check this in case there is no socket. 
		if(*state == CONNECTING)
		{
			*SD = createSocket(addr, port);
			if(*SD < 0)
			{
				printf("Could not establish connection. Try again.\n");
				*state = INITIAL;
				return -1;
			}
		}
		//prep a message that will be sent to the server.
		//for login, that's "login <username> <password>"
		msg_t m;
		strcpy(m.arg0, tokens[0]);
		strcpy(m.arg1, tokens[1]);
		strcpy(m.arg2, tokens[2]);
		//actually not needed, I think, since user isn't logged in yet.
		strcpy(m.user, user);
		m.type = LOGIN;
		//send message, and wait for response
		//(MSG_WAITALL prevents recv() from finishing until entire 
		//message is received)
		write(*SD, (void*)&m, sizeof(msg_t));
		msg_t rm;
		read(*SD, (void*)&rm, sizeof(msg_t));
		if(strcasecmp(rm.arg0, "OK") == 0)
		{
			printf("You have logged in. Welcome %s\n", rm.arg1);
			strcpy(user, rm.arg1);
			*state = LOGGEDIN;
			return 0;
		}
		//else if(timeout)
		//{  }
		else
		{
			printf("Invalid credentials. Try again.\n");
			*state = INITIAL;
			return -1;
		}	
	}
}

int newUser(int* SD, char* user, char** tokens, states_t* state)
{
	//for this implemenation, you can't create a new user unless you are
	//logged out. I think that's normal for most websites, etc.
	if(*state == LOGGEDIN)
	{
		printf("You are already logged in! Please logout to become a new user.\n");
		return -1;
	}
	else
	{
		//a socket might have already been made for some reason,
		//so have to check this in case there is no socket.
		if(*state == CONNECTING)	
		{
			*SD = createSocket(addr, port);
			if(*SD < 0)
			{
				printf("Could not establish connection. Try again.\n");
				*state = INITIAL;
				return -1;
			}
		}
		//prep a message that will be sent to the server.
		//for new user, that's "newuser <username> <password>"
		msg_t m;
		strcpy(m.arg0, tokens[0]);
		strcpy(m.arg1, tokens[1]);
		strcpy(m.arg2, tokens[2]);
		//actually not needed, I think, since user isn't logged in yet.
		strcpy(m.user, user);
		m.type = NEWUSR;
		//send message, and wait for response
		//(MSG_WAITALL prevents recv() from finishing until entire 
		//message is received)
		write(*SD, (void*)&m, sizeof(msg_t));
		msg_t rm;
		read(*SD, (void*)&rm, sizeof(msg_t));
		if(strcasecmp(rm.arg0, "OK") == 0)
		{
			printf("You have logged in. Welcome %s\n", rm.arg1);
			strcpy(user, rm.arg1);
			*state = LOGGEDIN;
			return 0;
		}
		//else if(timeout)
		//{  }
		else if(strcasecmp(rm.arg0, "EXISTS") == 0)
		{
			printf("Error: user already exists.\n");
			//log
			*state = INITIAL;
			return -1;
		}
		else
		{
			printf("Something went wrong. Try again.\n");
			//log
			*state = INITIAL;
			return -1;
		}	
	}
}

int sendMsg(int* SD, char* user, char** tokens, states_t* state)
{
	//can't message unless logged in
	if(*state != LOGGEDIN)
	{
		//should never get here due to logic in sockets.c, so fatal error.
		printf("An error occurred. Please restart the client.\n");
		//log
		return -2;
	}
	else
	{
		//prep a message that will be sent to the server.
		//for send, that's "send <message>"
		msg_t m;
		strcpy(m.arg0, tokens[0]);
		strcpy(m.arg1, tokens[1]);
		strcpy(m.user, user);
		m.type = SEND;
		//send message, and wait for response
		//(MSG_WAITALL prevents recv() from finishing until entire 
		//message is received)
		write(*SD, (void*)&m, sizeof(msg_t));
		msg_t rm;
		read(*SD, (void*)&rm, sizeof(msg_t));
		if(strcasecmp(rm.arg0, "OK") == 0)
		{
			printf("%s: %s\n", rm.user, rm.arg1);
			return 0;
		}
		//else if(timeout)
		//{  }
		else
		{
			printf("Something went wrong. Try again.\n");
			//log
			return -1;
		}	
	}
}

int logout(int* SD, char* user, char** tokens, states_t* state)
{
	//can't logout unless logged in
	if(*state != LOGGEDIN)
	{
		//should never get here due to logic in sockets.c, so fatal error.
		printf("An error occurred. Please restart the client.\n");
		//log
		return -2;
	}
	msg_t m;
	strcpy(m.arg0, tokens[0]);
	strcpy(m.user, user);
	m.type = LOGOUT;
	//send message
	//no confirmation is needed, I believe. Not sure though.
	write(*SD, (void*)&m, sizeof(msg_t));
	close(*SD);
	*state = INITIAL;
	printf("You have logged out. Please wait 60 seconds before attempting to reconnect.\n");
	return 0;
}
