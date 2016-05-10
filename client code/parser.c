#include "sockets.h"

cmd_t parseCMD(char* cmd, char** tokens, states_t state)
{
	//command is blank. Invalid command.
	if(*cmd == '\n')
	{
		return ERROR;	
	}	

	char* tokenizer = strtok(cmd, " \n");
	strcpy(tokens[0], tokenizer);

	if(state == INITIAL)
	{
		//if the state is initial, that means the user hasn't connected
		//yet. Only valid commands are login and newuser. Or exit.
		if(strcasecmp(cmd, "exit") == 0)
		{
			tokens[0] = cmd;
			return EXIT;
		}
		if(strcasecmp(tokens[0], "login") != 0 && strcasecmp(tokens[0], "newuser") != 0)
		{
			return ERROR;
		}
		else
		{
			//so we know it's either login or newuser, don't really care yet which one.
			tokenizer = strtok(NULL, " \n");
			if(!tokenizer) 
			{
				return ERROR;
			}
			//ok, now we found a username.
			strcpy(tokens[1], tokenizer);
			
			tokenizer = strtok(NULL, " \n");
			if(!tokenizer) 
			{
				return ERROR;
			}
			//and finally, found a password.
			strcpy(tokens[2], tokenizer);

			tokenizer = strtok(NULL, " \n");
			if(tokenizer)
			{
				return ERROR;
			}
			//now we just figure out what kind of command it is.
			//Everything else is okay.
			if(strcasecmp(tokens[0], "login") == 0)
			{
				return LOGIN;
			}
			else
			{
				return NEWUSR;	
			}
			//NOTE: only works because login and newuser commands
			//have same format.	
		}
		//SHOULD NEVER GET HERE
		return ERROR;
	}
	else
	{
		//user has logged in, anything goes.
		//Except login and newuser, actually. That's accidentally handled.
		//one word command. Valid ones are logout and exit only.
		if(strcasecmp(cmd, "logout") == 0)
		{
			tokens[0] = cmd;
			return LOGOUT;
		}
		else if(strcasecmp(cmd, "exit") == 0)
		{
			tokens[0] = cmd;
			return EXIT;
		}
		else
		{
			//there is more than one token. The only other possible
			//command it could be that is allowed is send. 
			if(strcasecmp(tokenizer, "send") != 0)
			{
				//if it isn't send, BAD
				return ERROR;
			}
			//since it's send, we can just copy the entire 
			//rest of the command as the message. 
			char* tmp = tokenizer + strlen(tokenizer) + 1;
			strcpy(tokens[1], tmp);
			return SEND;
		}
		//anything else is bad. This includes login and newuser (see above).
		return ERROR;
	}
	//SHOULD NEVER GET HERE 
	return ERROR;
}
